#include <stdio.h>
#include <ulfius.h>
#include <jansson.h>
#include "pib.h"
#include "cib.h"

#define PORT 45888

int
callback_get_pib (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];

    json_t *pib = get_pib_list();

    if (pib) {
        printf("found cib \n%s\n", json_dumps(pib, 2));
        ulfius_set_json_body_response(response, 200, pib);
        //TODO: SET RESPONSE TO JSON POLICY
    }
    else {
        printf("found no pib file\n");
        snprintf(msg, 256, "pib not found");
        ulfius_set_string_body_response(response, 404, msg);
        //TODO: SET RESPONSE TO 404 NOT FOUND
    }

    return U_CALLBACK_CONTINUE;
}

int
callback_get_cib (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];

    json_t *cib = get_cib_list();

    if (cib) {
        printf("found cib \n%s\n", json_dumps(cib, 2));
        ulfius_set_json_body_response(response, 200, cib);
        //TODO: SET RESPONSE TO JSON POLICY
    }
    else {
        printf("found no cib file\n");
        snprintf(msg, 256, "cib not found");
        ulfius_set_string_body_response(response, 404, msg);
        //TODO: SET RESPONSE TO 404 NOT FOUND
    }

    return U_CALLBACK_CONTINUE;
}

int
callback_get_pib_node (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];
    const char *uid = u_map_get(request->map_url, "uid");
    snprintf(msg, 256, "Request for pib uid %s", uid);
    ulfius_set_string_body_response(response, 200, msg);

    json_t *policy = get_pibnode_by_uid(uid);

    if (policy) {
        printf("found policy %s\n", json_dumps(policy, 0));
        ulfius_set_json_body_response(response, 200, policy);
        //TODO: SET RESPONSE TO JSON POLICY
    }
    else {
        printf("found no policy\n");
        snprintf(msg, 256, "%s not found", uid);
        ulfius_set_string_body_response(response, 404, msg);
        //TODO: SET RESPONSE TO 404 NOT FOUND
    }

    return U_CALLBACK_CONTINUE;
}

int
callback_get_cib_node (const struct _u_request * request, struct _u_response * response, void * user_data) {
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
callback_put_pib_node (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];
    json_error_t error;
    const char *uid = u_map_get(request->map_url, "uid");
    json_t *json_request = ulfius_get_json_body_request(request, &error);
    char *path = new_string("%s/%s/%s/%s", get_home_dir(), ".neat", POLICY_DIR, uid);
    if(json_request)
    {
        printf("%s\n", json_dumps(json_request, 2));
        if(!json_object_get(json_request, "uid") || !json_object_get(json_request, "properties")){
            snprintf(msg, 256, "PIB JSON object missing mandatory field");
            ulfius_set_string_body_response(response, 400, msg);
        } else {
            node_t *node = node_init(path);
            node->json = json_request;
            add_pib_node(node, path);
        }

    } else {
        snprintf(msg, 256, "JSON not found in body");
        ulfius_set_string_body_response(response, 400, msg);
    }
    free(path);
    return U_CALLBACK_CONTINUE;
}

int
callback_put_cib_node (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];
    json_error_t error;
    const char *uid = u_map_get(request->map_url, "uid");
    json_t *json_request = ulfius_get_json_body_request(request, &error);
    char *path = new_string("%s/%s/%s/%s", get_home_dir(), ".neat", CIB_DIR, uid);
    if(json_request)
    {
        printf("%s\n", json_dumps(json_request, 2));
        if(!json_object_get(json_request, "uid") || !json_object_get(json_request, "properties")){
            snprintf(msg, 256, "CIB JSON object missing mandatory field");
            ulfius_set_string_body_response(response, 400, msg);
        } else {
            node_t *node = node_init(path);
            add_cib_node(node);
        }
        //TODO: put data into node and call add_node(cib, node)
    } else {
        snprintf(msg, 256, "JSON not found in body");
        ulfius_set_string_body_response(response, 400, msg);
    }
    free(path);
    return U_CALLBACK_CONTINUE;
}

int
rest_start(int argc, char **argv){
    printf("-- Start REST API --\n\n");
    struct _u_instance instance;

    // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
        fprintf(stderr, "Error ulfius_init_instance, abort\n");
        return(1);
    }

    // Endpoint list declaration
    ulfius_add_endpoint_by_val(&instance, "GET", "/pib", "", 0, &callback_get_pib, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/cib", "", 0, &callback_get_cib, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/pib", "/:uid", 0, &callback_get_pib_node, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/cib", "/:uid", 0, &callback_get_cib_node, NULL);
    ulfius_add_endpoint_by_val(&instance, "PUT", "/pib", "/:uid", 0, &callback_put_pib_node, NULL);
    ulfius_add_endpoint_by_val(&instance, "PUT", "/cib", "/:uid", 0, &callback_put_cib_node, NULL);

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
