#include <stdio.h>
#include <ulfius.h>
#include <jansson.h>
#include "pib.h"
#include "cib.h"

#define PORT 45888

int
callback_get_pib (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];
    const char *uid = u_map_get(request->map_url, "uid");
    snprintf(msg, 256, "Request for pib uid %s", uid);
    ulfius_set_string_body_response(response, 200, msg);

    json_t *policy = get_pibnode_by_uid(uid);

    if (policy) {
        printf("found policy %s\n", json_dumps(policy, 0));
        //TODO: SET RESPONSE TO JSON POLICY
    }
    else {
        printf("found no policy file\n");
        //TODO: SET RESPONSE TO 404 NOT FOUND
    }

    return U_CALLBACK_CONTINUE;
}

int
callback_get_cib (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];
    const char *uid = u_map_get(request->map_url, "uid");

    json_t *cibnode = get_cibnode_by_uid(uid);

    if (cibnode) {
        printf("found cib node %s\n", json_dumps(cibnode, 0));
        ulfius_set_json_body_response(response, 200, cibnode);
        //TODO: SET RESPONSE TO JSON POLICY
    }
    else {
        printf("found no cib node file\n");
        snprintf(msg, 256, "%s not found", uid);
        ulfius_set_string_body_response(response, 404, msg);
        //TODO: SET RESPONSE TO 404 NOT FOUND
    }

    return U_CALLBACK_CONTINUE;
}

int
main() {
    struct _u_instance instance;

    pib_start();
    cib_start();

    // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return(1);
    }

    // Endpoint list declaration
    ulfius_add_endpoint_by_val(&instance, "GET", "/pib", "/:uid", 0, &callback_get_pib, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/cib", "/:uid", 0, &callback_get_cib, NULL);

    // Start the framework
    if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);

    getchar();

    } else {
    fprintf(stderr, "Error starting framework\n");
    }
    getchar();
    printf("End framework\n");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
}
