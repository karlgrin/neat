#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <uv.h>
#include <jansson.h>

#include "pib.h"
#include "cib.h"
#include "pm_helper.h"
#include "parse_json.h"

#define PM_BACKLOG 128
#define BUFSIZE 65536
#define NUM_CANDIDATES 10
#define PRECEDENCE_OPTIONAL 1

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array)[0]))

uv_loop_t *loop;

typedef struct client_req {
    char *buffer;
    size_t len;
} client_req_t;


bool
pre_resolve(const json_t *requests)
{
    bool pre_resolve = false;
    json_t *request;
    size_t i;

    json_array_foreach(requests, i, request) {
        json_t *req_type = json_object_get(request, "__request_type");
        if (req_type) {
            const char *req_type_val = json_string_value(json_object_get(req_type, "value"));
            if (!strcmp(req_type_val, "pre-resolve")) {
                json_object_del(request, "__request_type");
                pre_resolve = true;
            }
        }

    }
    return pre_resolve;
}

/* Adds the default values for `score' (0) and `evaluated' (False) to each property in each request.
   Should be executed before the logic of the main lookup routine. */
void
add_default_values(json_t *request)
{
    json_t *property, *attr;
    const char *key;
    size_t n;
    uint i;

    /* json values for default props */
    char *default_props[] = { "score", "evaluated", "precedence"};
    char *default_types[] = { "i", "b", "i"};
    int  default_values[] = { 0, 0, PRECEDENCE_OPTIONAL };

    json_object_foreach(request, key, property) {
        for (i = 0; i < ARRAY_SIZE(default_props); i++) {

            /* handle array of values */
            if (json_is_array(property)) {
                json_array_foreach(property, n, attr) {
                    json_t *tmp_prop = json_pack("{sO}", key, attr);
                    add_default_values(tmp_prop);
                    json_decref(tmp_prop); // RISKY seems that attr is not dereferenced?
                }
                break;
            }
            /* add default property if not found */
            if (json_object_get(property, default_props[i]) == NULL) {
                json_object_set(property, default_props[i], json_pack(default_types[i], default_values[i]));
            }
        }
    }
}

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
                    write_log(__FILE__, __func__, LOG_DEBUG, "Candidate is not in cib_candidates! Adding..");
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
    write_log(__FILE__, __func__, LOG_NORMAL, "Request received");
    write_log(__FILE__, __func__, LOG_DEBUG, "Request: \n%s", json_dumps(request_json, 0));

    if (!request_json) {
        write_log(__FILE__, __func__, LOG_ERROR, "Error with request, json-error-text: %s", json_error.text);
        return;
    }

    json_t *candidates = lookup(request_json);

    response_buf.base = json_dumps(candidates, 0);
    response_buf.len = strlen(response_buf.base);

    write_req = malloc(sizeof(uv_write_t));
    uv_write(write_req, client, &response_buf, 1, NULL);

    write_log(__FILE__, __func__, LOG_DEBUG, "PM result: \n%s", json_dumps(candidates, 0));
    write_log(__FILE__, __func__, LOG_NORMAL, "Request handled, sendig candidates");

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
on_new_connection(uv_stream_t *pm_server, int status)
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
        write_log(__FILE__, __func__, LOG_NORMAL, "Accepted client request");
        uv_read_start((uv_stream_t *) client, alloc_buffer, on_client_read);
    }
    else {
        uv_close((uv_handle_t *) client, NULL);
    }

}

void
pm_close(int sig)
{
    write_log(__FILE__, __func__, LOG_NORMAL, "Closing policy manager...\n");
    uv_fs_t req;
    uv_fs_unlink(loop, &req, SOCKET_PATH, NULL);

    pib_close();
    cib_close();
}


//this function never returns, see documentation "uv_run"
int
create_socket(){
    uv_pipe_t pm_server;
    int r;

    loop = uv_default_loop();
    uv_pipe_init(loop, &pm_server, 0);

    signal(SIGINT, pm_close);

    unlink(SOCKET_PATH);
    write_log(__FILE__, __func__, LOG_NORMAL, "Socket created in %s", SOCKET_PATH);

    if ((r = uv_pipe_bind(&pm_server, SOCKET_PATH)) != 0) {
        write_log(__FILE__, __func__, LOG_ERROR, "Socket bind error %s", uv_err_name(r));
        return 1;
    }
    if ((r = uv_listen((uv_stream_t*) &pm_server, PM_BACKLOG, on_new_connection))) {
        write_log(__FILE__, __func__, LOG_ERROR, "Socket listen error %s", uv_err_name(r));
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
    write_log(__FILE__, __func__, LOG_NORMAL,"\n--Start PM--\n");

    parse_arguments(argc, argv);
    create_folders();

    generate_cib_from_ifaces();
    cib_start();
    pib_start();

    return create_socket();
}
