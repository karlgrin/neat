#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <uv.h>
#include <jansson.h>
#include <pthread.h>

#include "pib.h"
#include "rest.h"
#include "cib.h"
#include "pm_helper.h"
#include "parse_json.h"

#define PM_BACKLOG 128
#define BUFSIZE 65536
#define PM_SOCK_DIR ".neat"
#define PM_SOCK_NAME "neat_pm_socket"
#define CIB_SOCK_NAME "neat_cib_socket"
#define PIB_SOCK_NAME "neat_pib_socket"

#define NUM_CANDIDATES 10

uv_loop_t *loop;
const char *pm_socket_path;
const char *cib_socket_path;
const char *pib_socket_path;
pthread_t thread_id_rest;

typedef struct client_req {
    char *buffer;
    size_t len;
} client_req_t;

void
make_pm_socket_path()
{
    char path[PATH_MAX], cib_path[PATH_MAX], pib_path[PATH_MAX];;
    struct stat st;

    snprintf(path, PATH_MAX, "%s/%s/", get_home_dir(), PM_SOCK_DIR);

    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
        printf("created directory %s\n", path);
    }

    pm_socket_path = strncat(path, PM_SOCK_NAME, PATH_MAX - strlen(path));
    snprintf(cib_path, PATH_MAX, "%s/%s/", get_home_dir(), PM_SOCK_DIR);
    cib_socket_path = strncat(cib_path, CIB_SOCK_NAME, PATH_MAX - strlen(path));
    snprintf(pib_path, PATH_MAX, "%s/%s/", get_home_dir(), PM_SOCK_DIR);
    pib_socket_path = strncat(pib_path, PIB_SOCK_NAME, PATH_MAX - strlen(path));
}

/* Adds the default values for `score' (0) and `evaluated' (False) to each property in each request.
   Should be executed before the logic of the main lookup routine. */

json_t *
lookup(json_t *reqs)
{
    json_t *updated_requests;
    json_t *request;
    json_t *candidate;
    json_t *candidates = json_array();
    json_t *policy_candidates;
    json_t *cib_candidates = json_array();
    json_t *cib_lookup_result; //TODO RENAME
    size_t i, j, k;

    json_t* req_expand = expand_properties(reqs);

    json_t* requests = process_special_properties(json_deep_copy(req_expand));

    json_array_foreach(requests, i, request) {
        add_default_values(request);
    }

    if (pre_resolve(requests)) {
        write_log(__FILE__, __func__, LOG_DEBUG, "__request_type is pre-resolve, skipping lookup.");
        return requests;
    }
    requests = expand_values(json_deep_copy(requests));

    json_array_foreach(requests, i, request) {

        /* Profile lookup */
        updated_requests = profile_lookup(request);

        /* CIB lookup */
        json_array_foreach(updated_requests, j, candidate){
            cib_lookup_result = cib_lookup(candidate);
            json_array_foreach(cib_lookup_result, k, candidate){
                if(!array_contains_value(cib_candidates, candidate)){
                    //write_log(__FILE__, __func__, LOG_DEBUG, "Candidate is not in cib_candidates! Adding..");
                    json_array_append(cib_candidates, candidate);
                }
            }
        }

        /* Policy lookup */
        json_array_foreach(cib_candidates, j, candidate) {
            policy_candidates = policy_lookup(candidate);
            json_array_foreach(policy_candidates, k, candidate){
                if(!array_contains_value(candidates, candidate)){
                    json_array_append(candidates, candidate);
                }
            }
        }
    }

    json_array_foreach(candidates, i, request) {
        add_default_values(request);
    }

    candidates = sort_json_array(candidates);
    candidates = limit_json_array(candidates, NUM_CANDIDATES);

    return candidates; // TODO free
}

void
handle_request(uv_stream_t *client)
{
    client_req_t *client_req = (client_req_t *) client->data;
    uv_buf_t response_buf;
    uv_write_t *write_req;
    json_t *request_json;
    json_error_t json_error;

    request_json = json_loads(client_req->buffer, 0, &json_error);
    write_log(__FILE__, __func__, LOG_EVENT, "Request received");
    if(debug_enabled) {
        char* json_string = json_dumps(request_json, 0);
        write_log(__FILE__, __func__, LOG_DEBUG, "Request: \n%s\n", json_string);
        free(json_string);
    }

    if (!request_json) {
        write_log(__FILE__, __func__, LOG_ERROR, "Error with request, json-error-text: %s", json_error.text);
        return;
    }

    json_t *candidates = lookup(request_json);

    response_buf.base = json_dumps(candidates, 0);
    response_buf.len = strlen(response_buf.base);

    write_req = malloc(sizeof(uv_write_t));
    uv_write(write_req, client, &response_buf, 1, NULL);

    write_log(__FILE__, __func__, LOG_EVENT, "Request handled, sending candidates\n");
    if(debug_enabled) {
        char* json_string = json_dumps(candidates, 0);
        write_log(__FILE__, __func__, LOG_DEBUG, "PM result: \n%s\n", json_string);
        free(json_string);
    }

    free(response_buf.base);
    json_decref(candidates);
    free(write_req);
}

void
alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buffer)
{
    buffer->base = calloc(suggested_size, sizeof(char));

    if(buffer->base == NULL) {
        write_log(__FILE__, __func__, LOG_ERROR, "Failed to allocate memory");
    }

    buffer->len = suggested_size;
}

void
on_client_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buffer)
{
    client_req_t *c_req = (client_req_t *) client->data;

    if (nread == UV_EOF) { /* -4095? */
        handle_request(client);
    }
    else if (nread < 0) {
        write_log(__FILE__, __func__, LOG_ERROR, "Socket client read failure");
        uv_close((uv_handle_t *) client, NULL);
        return;
    }
    else {
        strncpy(c_req->buffer + c_req->len, buffer->base, BUFSIZE - c_req->len);
        c_req->len += nread;
    }
    free(buffer->base);
}

void
on_new_pm_connection(uv_stream_t *pm_server, int status)
{
    uv_pipe_t *client;

    if (status == -1) {
        write_log(__FILE__, __func__, LOG_ERROR, "Socket new connection");
        return;
    }

    client = malloc(sizeof(uv_pipe_t));
    client->data = malloc(sizeof(client_req_t)); /* stores json request */
    ((client_req_t*) client->data)->buffer = malloc(BUFSIZE);
    ((client_req_t*) client->data)->len = 0;

    uv_pipe_init(loop, client, 0);

    if (uv_accept(pm_server, (uv_stream_t *) client) == 0) {
        write_log(__FILE__, __func__, LOG_EVENT, "Accepted client request");
        uv_read_start((uv_stream_t *) client, alloc_buffer, on_client_read);
    }
    else {
        uv_close((uv_handle_t *) client, NULL);
    }
}

/*-----------------------*/
/*----- PIB ACTIONS -----*/
/*-----------------------*/

void
handle_pib_request(uv_stream_t *client)
{
    client_req_t *client_req = (client_req_t *) client->data;
    uv_buf_t response_buf;
    uv_write_t *write_req;

    json_t *request_json;
    json_error_t json_error;
    request_json = json_loads(client_req->buffer, 0, &json_error);
    add_pib_node(request_json);
}

void
on_pib_socket_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buffer)
{
    client_req_t *c_req = (client_req_t *) client->data;

    if (nread == UV_EOF) { /* -4095? */
        handle_pib_request(client);
    }
    else if (nread < 0) {
        printf("error on pib_socket read\n");
        uv_close((uv_handle_t *) client, NULL);
        return;
    }
    else {
        /* printf("read: %s\n", buffer->base); */

        strncpy(c_req->buffer + c_req->len, buffer->base, BUFSIZE - c_req->len);
        c_req->len += nread;

        /* printf("buffer: %s (%zu bytes)\n", c_req->buffer, c_req->len); */
    }
    free(buffer->base);
}

void
on_new_pib_connection(uv_stream_t *pib_server, int status)
{
    uv_pipe_t *client;

    if (status == -1) {
        fprintf(stderr, "new connection error\n");
        return;
    }

    client = malloc(sizeof(uv_pipe_t));
    client->data = malloc(sizeof(client_req_t)); /* stores json request */
    ((client_req_t*) client->data)->buffer = malloc(BUFSIZE);
    ((client_req_t*) client->data)->len = 0;

    uv_pipe_init(loop, client, 0);

    if (uv_accept(pib_server, (uv_stream_t *) client) == 0) {
        printf("\nAccepted pib_socket request\n");
        uv_read_start((uv_stream_t *) client, alloc_buffer, on_pib_socket_read);
    }
    else {
        uv_close((uv_handle_t *) client, NULL);
    }
}

/*-----------------------*/
/*----- CIB ACTIONS -----*/
/*-----------------------*/

void
handle_cib_request(uv_stream_t *client)
{
    client_req_t *client_req = (client_req_t *) client->data;
    uv_buf_t response_buf;
    uv_write_t *write_req;

    json_t *request_json;
    json_error_t json_error;
    request_json = json_loads(client_req->buffer, 0, &json_error);
    add_cib_node(json_array_get(request_json, 0));
}

void
on_cib_socket_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buffer)
{
    client_req_t *c_req = (client_req_t *) client->data;

    if (nread == UV_EOF) { /* -4095? */
        handle_cib_request(client);
    }
    else if (nread < 0) {
        printf("error on cib_socket read\n");
        uv_close((uv_handle_t *) client, NULL);
        return;
    }
    else {
        /* printf("read: %s\n", buffer->base); */

        strncpy(c_req->buffer + c_req->len, buffer->base, BUFSIZE - c_req->len);
        c_req->len += nread;

        /* printf("buffer: %s (%zu bytes)\n", c_req->buffer, c_req->len); */
    }
    free(buffer->base);
}

void
on_new_cib_connection(uv_stream_t *cib_server, int status)
{
    uv_pipe_t *client;

    if (status == -1) {
        fprintf(stderr, "new connection error\n");
        return;
    }

    client = malloc(sizeof(uv_pipe_t));
    client->data = malloc(sizeof(client_req_t)); /* stores json request */
    ((client_req_t*) client->data)->buffer = malloc(BUFSIZE);
    ((client_req_t*) client->data)->len = 0;

    uv_pipe_init(loop, client, 0);

    if (uv_accept(cib_server, (uv_stream_t *) client) == 0) {
        printf("\nAccepted cib_socket request\n");
        uv_read_start((uv_stream_t *) client, alloc_buffer, on_cib_socket_read);
    }
    else {
        uv_close((uv_handle_t *) client, NULL);
    }
}

void
pm_close(int sig)
{
    write_log(__FILE__, __func__, LOG_EVENT, "\nClosing policy manager...\n");
    uv_fs_t req;
    uv_fs_unlink(loop, &req, pm_socket_path, NULL);
    uv_fs_unlink(loop, &req, cib_socket_path, NULL);
    uv_fs_unlink(loop, &req, pib_socket_path, NULL);

    pib_close();
    cib_close();
    pm_helper_close();

    exit(sig);
}


//this function never returns, see documentation "uv_run"
int
create_socket()
{
    printf("\n\n--Start PM--\n\n");

    generate_cib_from_ifaces();
    cib_start();
    pib_start();

    pthread_create(&thread_id_rest, NULL, rest_start, NULL);  //start policy manager
    //print_nodes(pib_profiles);

    uv_pipe_t pm_server;
    uv_pipe_t cib_server;
    uv_pipe_t pib_server;
    int r, s, t;

    make_pm_socket_path();

    printf("\nsocket created in %s\n", pm_socket_path);
    printf("\nsocket created in %s\n", cib_socket_path);
    printf("\nsocket created in %s\n\n", pib_socket_path);

    loop = uv_default_loop();
    uv_pipe_init(loop, &pm_server, 0);
    uv_pipe_init(loop, &cib_server, 0);
    uv_pipe_init(loop, &pib_server, 0);

    signal(SIGINT, pm_close);

    unlink(SOCKET_PATH);
    write_log(__FILE__, __func__, LOG_EVENT, "Socket created in %s\n", SOCKET_PATH);

    if ((r = uv_pipe_bind(&pm_server, SOCKET_PATH)) != 0) {
        write_log(__FILE__, __func__, LOG_ERROR, "Socket bind error %s", uv_err_name(r));
        return 1;
    }
    if ((r = uv_listen((uv_stream_t*) &pm_server, PM_BACKLOG, on_new_pm_connection))) {
        write_log(__FILE__, __func__, LOG_ERROR, "Socket listen error %s", uv_err_name(r));
        return 2;
    }

    if ((s = uv_pipe_bind(&cib_server, cib_socket_path)) != 0) {
        fprintf(stderr, "Bind error %s\n", uv_err_name(s));
        return 1;
    }
    if ((s = uv_listen((uv_stream_t*) &cib_server, PM_BACKLOG, on_new_cib_connection))) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(s));
        return 2;
    }

    if ((t = uv_pipe_bind(&pib_server, pib_socket_path)) != 0) {
        fprintf(stderr, "Bind error %s\n", uv_err_name(t));
        return 1;
    }
    if ((t = uv_listen((uv_stream_t*) &pib_server, PM_BACKLOG, on_new_pib_connection))) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(t));
        return 2;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}

void
parse_arguments(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++) {
        char* input = argv[i];
        if(strcmp(input,"-debug") == 0) {
            enable_debug_message(true);
        }
        else if(strcmp(input,"-log") == 0) {
            enable_log_file(true);
        }
        else {
            write_log(__FILE__, __func__, LOG_ERROR, "Unexpected input: %s", input);
        }
    }
}

int
main(int argc, char *argv[])
{
    write_log(__FILE__, __func__, LOG_EVENT,"\n--Start PM--\n");

    parse_arguments(argc, argv);
    create_folders();

    generate_cib_from_ifaces();
    cib_start();
    pib_start();

    return create_socket();
}
