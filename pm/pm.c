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
#include "pmhelper.h"
#include "parse_json.h"

#define PM_BACKLOG 128
#define BUFSIZE 65536
#define PM_SOCK_DIR ".neat"
#define PM_SOCK_NAME "neat_pm_socket"

#define NUM_CANDIDATES 10

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array)[0]))

uv_loop_t *loop;
const char *pm_socket_path;

typedef struct client_req {
    char *buffer;
    size_t len;
} client_req_t;

char *
make_pm_socket_path()
{
    char path[PATH_MAX];
    struct stat st;

    snprintf(path, PATH_MAX, "%s/%s/", get_home_dir(), PM_SOCK_DIR);

    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
        printf("created directory %s\n", path);
    }

    return strncat(path, PM_SOCK_NAME, PATH_MAX - strlen(path));
}

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
    json_t *value, *attr;
    const char *key;
    size_t n;
    uint i;

    /* json values for default props */
    char *default_props[] = { "score", "evaluated"};
    char *default_types[] = { "i", "b" };
    int  default_values[] = { 0, 0 };

    json_object_foreach(request, key, value) {
        for (i = 0; i < ARRAY_SIZE(default_props); i++) {

            /* handle array of values */
            if (json_is_array(value)) {
                json_array_foreach(value, n, attr) {
                    json_t *tmp_prop = json_pack("{sO}", key, attr);
                    add_default_values(tmp_prop);
                    json_decref(tmp_prop); // RISKY seems that attr is not dereferenced?
                }
                break;
            }
            /* add default property if not found */
            if (json_object_get(request, default_props[i]) == NULL) {
                json_object_set(value, default_props[i], json_pack(default_types[i], default_values[i]));
            }
        }
    }
}

json_t *
lookup(const json_t *reqs)
{

    json_t *updated_requests;
    json_t *request;
    json_t *candidate;
    json_t *candidates = json_array();
    json_t *policy_candidates;
    json_t *cib_candidates = json_array();
    json_t *cib_lookup_result; //TODO RENAME
    size_t i, j, k;


    json_t* requests = process_special_properties(json_deep_copy(reqs));
    printf("\nspecial prop:\n%s\n\n", json_dumps(requests, 0));

    json_array_foreach(requests, i, request) {
        add_default_values(request);
    }

    if (pre_resolve(requests)) {
        printf("__request_type is pre-resolve, skipping lookup...\n");
        return requests;
    }

    json_array_foreach(requests, i, request) {

        /* Profile lookup */
        updated_requests = profile_lookup(request);

        /* CIB lookup */
        json_array_foreach(updated_requests, j, candidate){
            cib_lookup_result = cib_lookup(candidate);
            json_array_foreach(cib_lookup_result, k, candidate){
                if(!array_contains_value(cib_candidates, candidate)){
                    //printf("Candidate is not in cib_candidates! Adding...\n");
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

    //printf("finished reading\n");
    //printf("buffer = %s\n", client_req->buffer);

    request_json = json_loads(client_req->buffer, 0, &json_error);
    printf("\nPM request: \n%s\n", json_dumps(request_json, 0));
    if (!request_json) {
        fprintf(stderr, "error: on line %d: %s\n", json_error.line, json_error.text);
        return;
    }

    json_t *candidates = lookup(request_json);

    response_buf.base = json_dumps(candidates, 0);
    response_buf.len = strlen(response_buf.base);

    write_req = malloc(sizeof(uv_write_t));
    uv_write(write_req, client, &response_buf, 1, NULL);
    printf("\nPM result: \n%s\n", json_dumps(candidates, 0));
    printf("\nPM sent candidates..\n");

    free(response_buf.base);
    json_decref(candidates);
    free(write_req);
}

void
alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buffer)
{
    buffer->base = calloc(suggested_size, sizeof(char));
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
        printf("error on client read\n");
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
on_new_connection(uv_stream_t *pm_server, int status)
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

    if (uv_accept(pm_server, (uv_stream_t *) client) == 0) {
        printf("\nAccepted client request\n");
        uv_read_start((uv_stream_t *) client, alloc_buffer, on_client_read);
    }
    else {
        uv_close((uv_handle_t *) client, NULL);
    }

}

void
pm_close(int sig)
{
    printf("\nClosing policy manager...\n");
    uv_fs_t req;
    uv_fs_unlink(loop, &req, pm_socket_path, NULL);

    pib_close();
    cib_close();
}


int
pm_start(int argc, char **argv)
{
    printf("\n\n--Start PM--\n\n");
    generate_cib_from_ifaces();
    cib_start();
    pib_start();

    uv_pipe_t pm_server;
    int r;

    pm_socket_path = make_pm_socket_path();

    printf("\nsocket created in %s\n\n", pm_socket_path);

    loop = uv_default_loop();
    uv_pipe_init(loop, &pm_server, 0);

    signal(SIGINT, pm_close);

    if ((r = uv_pipe_bind(&pm_server, pm_socket_path)) != 0) {
        fprintf(stderr, "Bind error %s\n", uv_err_name(r));
        return 1;
    }
    if ((r = uv_listen((uv_stream_t*) &pm_server, PM_BACKLOG, on_new_connection))) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(r));
        return 2;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}
