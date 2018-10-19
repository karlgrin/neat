#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <pwd.h>
#include <sys/stat.h>
#include <uv.h>
#include <jansson.h>

// TODO
// should provide lookup functions
// maybe also initalization of main pib/cib objects
#include "pib.h"
#include "cib.h"

#define PM_BACKLOG 128
#define BUFSIZE 65536
#define PM_SOCK_DIR ".neat"
#define PM_SOCK_NAME "pm_socket"

uv_loop_t *loop;
const char *pm_socket_path;

typedef struct client_req {
    char *buffer;
    size_t len;
} client_req_t;

char *
make_pm_socket_path()
{
    char pm_socket_path[PATH_MAX];
    char *homedir;
    struct stat st;

    homedir = getenv("HOME");

    if (homedir == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    snprintf(pm_socket_path, PATH_MAX, "%s/%s/", homedir, PM_SOCK_DIR);

    if (stat(pm_socket_path, &st) == -1) {
        mkdir(pm_socket_path, 0700);
        printf("created directory %s\n", pm_socket_path);
    }

    return strncat(pm_socket_path, PM_SOCK_NAME, PATH_MAX - strlen(pm_socket_path));
}

void
handle_request(uv_stream_t *client)
{
    client_req_t *client_req = (client_req_t *) client->data;
    uv_buf_t response_buf;
    uv_write_t *write_req;
    json_t *request_json;
    json_error_t json_error;

    printf("finished reading\n");
    printf("buffer = %s\n", client_req->buffer);

    request_json = json_loads(client_req->buffer, 0, &json_error);

    if (!request_json) {
        fprintf(stderr, "error: on line %d: %s\n", json_error.line, json_error.text);
        return;
    }
    response_buf.base = "no candidates yet";
    response_buf.len = strlen(response_buf.base);

    // TODO
    /* json_t *candidates = pib_lookup(cib_lookup(profile_lookup(req))); */
    /* response_buf.base = json_dumps(candidates); */
    /* response_buf.len = strlen(response_buf.base); */
    /* free(response_buf.base); */
    /* json_decref(candidates); */

    write_req = malloc(sizeof(uv_write_t));
    uv_write(write_req, client, &response_buf, 1, NULL);

    printf("sent candidate list\n");

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
        printf("read: %s\n", buffer->base);

        strncpy(c_req->buffer + c_req->len, buffer->base, BUFSIZE - c_req->len);
        c_req->len += nread;

        printf("buffer: %s (%zu bytes)\n", c_req->buffer, c_req->len);
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

    uv_pipe_init(loop, client, 0);

    if (uv_accept(pm_server, (uv_stream_t *) client) == 0) {
        printf("accepted client request\n");
        uv_read_start((uv_stream_t *) client, alloc_buffer, on_client_read);
    }
    else {
        uv_close((uv_handle_t *) client, NULL);
    }

}

void
close_pm(int sig)
{
    printf("\nClosing policy manager...\n");
    uv_fs_t req;
    uv_fs_unlink(loop, &req, pm_socket_path, NULL);

    pib_close();
    cib_close();
    exit(0);
}


int
main(int argc, char **argv)
{
    printf("--Start PM--\n\n");
    generate_cib_from_ifaces();
    cib_start();
    pib_start();

    //print_nodes(pib_profiles);
 
    uv_pipe_t pm_server;
    int r;

    pm_socket_path = make_pm_socket_path();

    printf("\nsocket created in %s\n\n", pm_socket_path);

    loop = uv_default_loop();
    uv_pipe_init(loop, &pm_server, 0);

    signal(SIGINT, close_pm);

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
