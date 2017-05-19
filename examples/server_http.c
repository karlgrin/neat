#include <neat.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "util.h"
#include "picohttpparser.h"

/**********************************************************************

    http server

**********************************************************************/

static char *config_property = "{\
    \"transport\": [\
        {\
            \"value\": \"SCTP\",\
            \"precedence\": 1\
        },\
        {\
            \"value\": \"TCP\",\
            \"precedence\": 1\
        }\
    ]\
}";
static uint16_t config_log_level = 1;
#define BUFFERSIZE 33768

static char *http_header_connection_close       = "Connection: Close";
static char *http_header_connection_keep_alive  = "Connection: Keep-Alive";


static neat_error_code on_writable(struct neat_flow_operations *opCB);

struct http_flow {
    unsigned char buffer[BUFFERSIZE];
    char *method;
    char *path;
    int minor_version;
    int pret;
    struct phr_header headers[100];
    size_t buffer_len;
    size_t buffer_len_prev;
    size_t method_len;
    size_t path_len;
    size_t num_headers;
    uint8_t keep_alive;
};

static int
prepare_http_response(struct http_flow *http_flow, unsigned char **buffer, uint32_t *buffer_len) {

    int header_length               = 0;
    unsigned char *header_buffer    = NULL;
    int payload_length              = 0;
    unsigned char *payload_buffer   = NULL;
    int i                           = 0;
    char misc_buffer[512];

    // iterate through header fields
    for (i = 0; i < (int)http_flow->num_headers; i++) {
        // build string from name/value pair
        snprintf(misc_buffer, 512, "%.*s: %.*s",
            (int)http_flow->headers[i].name_len,
            http_flow->headers[i].name,
            (int)http_flow->headers[i].value_len,
            http_flow->headers[i].value);

        if (strncasecmp(misc_buffer, http_header_connection_close, strlen(http_header_connection_close)) == 0) {
            printf(">>>>>>>>>>> CLOSE !!!!!!!!!!!!!!!!\n");
        } else if (strncasecmp(misc_buffer, http_header_connection_keep_alive, strlen(http_header_connection_keep_alive)) == 0) {
            printf(">>>>>>>>>>> KEEP-ALIVE !!!!!!!!!!!!!!!!\n");
            http_flow->keep_alive = 1;
        }

    }

    // XXX needs refactoring - just shit ... shame on me... :/
    if (http_flow->path_len == 1 && http_flow->path[0] == '/') {
        // request root "/" --> index.html
        snprintf(misc_buffer, sizeof(misc_buffer), "index.html");
    } else if (http_flow->path_len > 1 && http_flow->path[0] == '/') {
        // path begins with "/"
        snprintf(misc_buffer, sizeof(misc_buffer), "%.*s", (int)http_flow->path_len - 1, http_flow->path + 1);
    } else {
        // path does not begin with "/"
        snprintf(misc_buffer, sizeof(misc_buffer), "%.*s", (int)http_flow->path_len, http_flow->path);
    }

    payload_length = read_file(misc_buffer, (char **) &payload_buffer);

    // error when reading file - read inde.html
    if (payload_length < 0) {
        fprintf(stderr, "%s - reading failed -  delivering index.html\n", __func__);
        snprintf(misc_buffer, sizeof(misc_buffer), "index.html");
        payload_length = read_file(misc_buffer, (char **) &payload_buffer);
    }

    if (payload_length < 0 ) {
        // we have a serious problem here...
        exit(EXIT_FAILURE);
    }

    // prepare response header
    header_length = asprintf((char **)&header_buffer,
        "HTTP/1.1 200 OK\r\n"
        "Server: NEAT super fancy webserver\r\n"
        "Content-Length: %u\r\n"
        "Connection: %s\r\n"
        "\r\n",
        payload_length,
        http_flow->keep_alive ? "Keep-Alive" : "Close");

    if (header_length == -1) {
        fprintf(stderr, "%s - asprintf failed\n", __func__);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "%s\n", header_buffer);

    header_buffer = realloc(header_buffer, header_length + payload_length);

    if (header_buffer == NULL) {
        fprintf(stderr, "%s - realloc failed\n", __func__);
    }

    memcpy(header_buffer + header_length, payload_buffer, payload_length);

    *buffer = header_buffer;
    *buffer_len = header_length + payload_length;

    fprintf(stderr, "%s - pointer: %p - len: %u\n",__func__, (void *)*buffer, *buffer_len);

    return NEAT_OK;
}



/*
    print usage and exit
*/
static void
print_usage()
{
    if (config_log_level >= 2) {
        fprintf(stderr, "%s()\n", __func__);
    }

    printf("server_http [OPTIONS]\n");
    printf("\t- P <filename> \tneat properties, default properties:\n%s\n", config_property);
    printf("\t- v \tlog level 0..2 (%d)\n", config_log_level);
}

/*
    Error handler
*/
static neat_error_code
on_error(struct neat_flow_operations *opCB)
{
    if (config_log_level >= 2) {
        fprintf(stderr, "%s()\n", __func__);
    }

    exit(EXIT_FAILURE);
}

static neat_error_code
on_readable(struct neat_flow_operations *opCB)
{
    // data is available to read
    neat_error_code code;
    struct http_flow *http_flow = opCB->userData;
    uint32_t buffer_filled = 0;

    if (config_log_level >= 2) {
        fprintf(stderr, "%s()\n", __func__);
    }

    code = neat_read(opCB->ctx, opCB->flow, http_flow->buffer + http_flow->buffer_len, BUFFERSIZE - http_flow->buffer_len, &buffer_filled, NULL, 0);
    if (code != NEAT_OK) {
        if (code == NEAT_ERROR_WOULD_BLOCK) {
            return NEAT_OK;
        } else {
            fprintf(stderr, "%s - neat_read failed - code: %d\n", __func__, (int)code);
            return on_error(opCB);
        }
    }

    if (config_log_level >= 1) {
        printf("received data - %d byte\n", buffer_filled);
    }
#if 0
    if (config_log_level >= 2) {
        fwrite(http_flow->buffer, sizeof(char), buffer_filled, stdout);
        printf("\n");
        fflush(stdout);
    }
#endif

    http_flow->buffer_len_prev = http_flow->buffer_len;
    http_flow->buffer_len += buffer_filled;
    http_flow->num_headers = sizeof(http_flow->headers) / sizeof(http_flow->headers[0]);

    http_flow->pret = phr_parse_request((const char *) http_flow->buffer,
        http_flow->buffer_len,
        (const char **) &(http_flow->method),
        &(http_flow->method_len),
        (const char **) &(http_flow->path),
        &(http_flow->path_len),
        &(http_flow->minor_version),
        http_flow->headers,
        &(http_flow->num_headers),
        http_flow->buffer_len_prev);

    if (http_flow->pret > 0) {
        fprintf(stderr, "%s - request parsed!\n", __func__);
        opCB->on_writable = on_writable;
        neat_set_operations(opCB->ctx, opCB->flow, opCB);
        return NEAT_OK;
    } else if (http_flow->pret == -1) {
        fprintf(stderr, "%s - error : parsing request!\n", __func__);
        neat_close(opCB->ctx, opCB->flow);
        return NEAT_OK;
    }

    assert(http_flow->pret == -2);
    if (http_flow->buffer_len == sizeof(http_flow->buffer)) {
        fprintf(stderr, "%s - error : request to long!!\n", __func__);
        neat_close(opCB->ctx, opCB->flow);
        return NEAT_OK;
    }

    neat_set_operations(opCB->ctx, opCB->flow, opCB);
    return NEAT_OK;
}

static neat_error_code
on_all_written(struct neat_flow_operations *opCB)
{
    struct http_flow *http_flow = opCB->userData;

    if (config_log_level >= 2) {
        fprintf(stderr, "%s()\n", __func__);
    }

    if (http_flow->keep_alive == 1) {
        memset(http_flow, 0, sizeof(struct http_flow));
        opCB->on_all_written = NULL;
        opCB->on_readable = on_readable;
        neat_set_operations(opCB->ctx, opCB->flow, opCB);
    } else {
        neat_close(opCB->ctx, opCB->flow);
    }

    return NEAT_OK;
}

static neat_error_code
on_writable(struct neat_flow_operations *opCB)
{
    neat_error_code code;
    struct http_flow *http_flow = opCB->userData;
    unsigned char *buffer       = NULL;
    uint32_t buffer_len         = 0;

    if (config_log_level >= 2) {
        fprintf(stderr, "%s()\n", __func__);
    }

    // print request information
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    printf("request is %d bytes long\n", http_flow->pret);
    printf("method is %.*s\n", (int)http_flow->method_len, http_flow->method);
    printf("path is %.*s\n", (int)http_flow->path_len, http_flow->path);
    printf("HTTP version is 1.%d\n", http_flow->minor_version);
    printf("headers:\n");
    for (int i = 0; i != (int)http_flow->num_headers; ++i) {
        printf("%.*s: %.*s\n",
            (int)http_flow->headers[i].name_len,
            http_flow->headers[i].name,
            (int)http_flow->headers[i].value_len,
            http_flow->headers[i].value);

    }
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

    if (prepare_http_response(http_flow, &buffer, &buffer_len) != NEAT_OK) {
        exit(-1);
    }

    printf("%s - buffer len %d - pointer %p\n", __func__, buffer_len, (void *) buffer);


    code = neat_write(opCB->ctx, opCB->flow, buffer, buffer_len, NULL, 0);
    if (code != NEAT_OK) {
        fprintf(stderr, "%s - neat_write failed - code: %d\n", __func__, (int)code);
        return on_error(opCB);
    }

    opCB->on_writable = NULL;
    opCB->on_all_written = on_all_written;
    neat_set_operations(opCB->ctx, opCB->flow, opCB);


    return NEAT_OK;
}

static neat_error_code
on_close(struct neat_flow_operations *opCB)
{
    fprintf(stderr, "%s - flow closed OK!\n", __func__);
    free(opCB->userData);
    return NEAT_OK;
}

static neat_error_code
on_connected(struct neat_flow_operations *opCB)
{
    if (config_log_level >= 2) {
        fprintf(stderr, "%s()\n", __func__);
    }

    if (config_log_level >= 1) {
        printf("peer connected\n");
    }

    if ((opCB->userData = calloc(1, sizeof(struct http_flow))) == NULL) {
        fprintf(stderr, "%s - could not allocate http_flow\n", __func__);
        exit(EXIT_FAILURE);
    }

    opCB->on_readable = on_readable;
    opCB->on_writable = NULL;
    opCB->on_all_written = NULL;
    opCB->on_close = on_close;
    neat_set_operations(opCB->ctx, opCB->flow, opCB);

    return NEAT_OK;
}

int
main(int argc, char *argv[])
{
    // uint64_t prop;
    int arg, result;
    char *arg_property = NULL;
    struct neat_ctx *ctx = NULL;
    struct neat_flow *flow = NULL;
    struct neat_flow_operations ops;

    memset(&ops, 0, sizeof(ops));

    result = EXIT_SUCCESS;

    while ((arg = getopt(argc, argv, "P:v:")) != -1) {
        switch(arg) {
        case 'P':
            if (read_file(optarg, &arg_property) < 0) {
                fprintf(stderr, "Unable to read properties from %s: %s",
                        optarg, strerror(errno));
                result = EXIT_FAILURE;
                goto cleanup;
            }
            if (config_log_level >= 1) {
                printf("option - properties: %s\n", arg_property);
            }
            break;
        case 'v':
            config_log_level = atoi(optarg);
            if (config_log_level >= 1) {
                printf("option - log level: %d\n", config_log_level);
            }
            break;
        default:
            print_usage();
            goto cleanup;
            break;
        }
    }

    if (optind != argc) {
        fprintf(stderr, "%s - argument error\n", __func__);
        print_usage();
        goto cleanup;
    }

    if ((ctx = neat_init_ctx()) == NULL) {
        fprintf(stderr, "%s - neat_init_ctx failed\n", __func__);
        result = EXIT_FAILURE;
        goto cleanup;
    }

    if (config_log_level == 0) {
        neat_log_level(ctx, NEAT_LOG_ERROR);
    } else if (config_log_level == 1){
        neat_log_level(ctx, NEAT_LOG_WARNING);
    } else {
        neat_log_level(ctx, NEAT_LOG_DEBUG);
    }

    // new neat flow
    if ((flow = neat_new_flow(ctx)) == NULL) {
        fprintf(stderr, "%s - neat_new_flow failed\n", __func__);
        result = EXIT_FAILURE;
        goto cleanup;
    }

    // set properties
    if (neat_set_property(ctx, flow, arg_property ? arg_property : config_property)) {
        fprintf(stderr, "%s - neat_set_property failed\n", __func__);
        result = EXIT_FAILURE;
        goto cleanup;
    }

    // set callbacks
    ops.on_connected = on_connected;
    ops.on_error = on_error;

    if (neat_set_operations(ctx, flow, &ops)) {
        fprintf(stderr, "%s - neat_set_operations failed\n", __func__);
        result = EXIT_FAILURE;
        goto cleanup;
    }

    // wait for on_connected or on_error to be invoked
    if (neat_accept(ctx, flow, 8080, NULL, 0)) {
        fprintf(stderr, "%s - neat_accept failed\n", __func__);
        result = EXIT_FAILURE;
        goto cleanup;
    }

    if (chdir("htdocs")) {
        printf("%s - chdir failed\n", __func__);
    }

    neat_start_event_loop(ctx, NEAT_RUN_DEFAULT);

    // cleanup
cleanup:
    if (arg_property)
        free(arg_property);

    if (ctx != NULL) {
        neat_free_ctx(ctx);
    }
    exit(result);
}
