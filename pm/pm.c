#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include <limits.h>
#include <pwd.h>
#include <sys/stat.h>

#include<uv.h>

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
alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buffer)
{
    buffer->base = malloc(suggested_size);
    buffer->len = suggested_size;
}

void
on_client_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buffer)
{
    uv_write_t *write_req;
    uv_buf_t req_buf;
    client_req_t *c_req = (client_req_t *) client->data;

    if (nread == UV_EOF) { /* -4095? */
        printf("finished reading\n");

        printf("nread = %zu\n", nread);
        printf("buffer = %s\n", c_req->buffer);

        write_req = malloc(sizeof(uv_write_t));
        // TODO req.base = candidates(c_req->buffer);
        req_buf.base = "no candidates yet";
        req_buf.len = strlen(req_buf.base);

        uv_write(write_req, client, &req_buf, 1, NULL);

        printf("sent candidate list\n");

        free(write_req);
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
on_client_write(uv_write_t *req, int status)
{
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
remove_sock(int sig)
{
    uv_fs_t req;
    uv_fs_unlink(loop, &req, pm_socket_path, NULL);
    exit(0);
}

int
main(int argc, char **argv)
{
    uv_pipe_t pm_server;

    pm_socket_path = make_pm_socket_path();

    printf("socket created in %s\n", pm_socket_path);

    loop = uv_default_loop();
    uv_pipe_init(loop, &pm_server, 0);

    signal(SIGINT, remove_sock);

    int r;
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
