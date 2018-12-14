#include <stdio.h>
#include <ulfius.h>
#include <jansson.h>
#include "pib.h"
#include "cib.h"

#define PORT 45888
struct _u_instance instance;

int
callback_get_pib (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];

    json_t *pib = get_pib_list();

    if (pib) {
        ulfius_set_json_body_response(response, 200, pib);
    }
    else {
        snprintf(msg, 256, "pib not found");
        ulfius_set_string_body_response(response, 404, msg);
    }

    return U_CALLBACK_CONTINUE;
}

int
callback_get_cib (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];

    json_t *cib = get_cib_list();

    if (cib) {
        ulfius_set_json_body_response(response, 200, cib);
    }
    else {
        snprintf(msg, 256, "cib not found");
        ulfius_set_string_body_response(response, 404, msg);
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
        ulfius_set_json_body_response(response, 200, policy);
    }
    else {
        snprintf(msg, 256, "%s not found", uid);
        ulfius_set_string_body_response(response, 404, msg);
    }

    return U_CALLBACK_CONTINUE;
}

int
callback_get_cib_node (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];
    const char *uid = u_map_get(request->map_url, "uid");

    json_t *cibnode = get_cibnode_by_uid(uid);

    if (cibnode) {
        ulfius_set_json_body_response(response, 200, cibnode);
    }
    else {
        snprintf(msg, 256, "%s not found", uid);
        ulfius_set_string_body_response(response, 404, msg);
    }

    return U_CALLBACK_CONTINUE;
}

int
callback_put_pib_node (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char msg[256];
    json_error_t error;
    const char *uid = u_map_get(request->map_url, "uid");
    json_t *json_request = ulfius_get_json_body_request(request, &error);
    char *path = new_string("%s/%s/%s/%s", get_home_dir(), ".neat", policy_dir, uid);
    if(json_request)
    {
        if(!json_object_get(json_request, "uid") || !json_object_get(json_request, "properties")){
            snprintf(msg, 256, "PIB JSON object missing mandatory field");
            ulfius_set_string_body_response(response, 400, msg);
        } else {
            add_pib_node(json_request);
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
    char *path = new_string("%s%s%s", cib_dir, uid, ".cib");
    if(json_request)
    {
        if(!json_object_get(json_request, "uid") || !json_object_get(json_request, "properties")){
            snprintf(msg, 256, "CIB JSON object missing mandatory field");
            ulfius_set_string_body_response(response, 400, msg);
        } else {
            add_cib_node(json_request);
        }
    } else {
        snprintf(msg, 256, "JSON not found in body");
        ulfius_set_string_body_response(response, 400, msg);
    }
    free(path);
    return U_CALLBACK_CONTINUE;
}

int
rest_start(int argc, char **argv){

    // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
        write_log(__FILE__, __func__, LOG_ERROR, "REST API failure, ulfius_init_instance, abort\n");
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
        write_log(__FILE__, __func__, LOG_EVENT, "Starting REST-API on port %d", instance.port);
        getchar();
    } else {
        write_log(__FILE__, __func__, LOG_EVENT, "Failed to start REST-API");
    }
    return 0;
}

void
rest_close() {
    write_log(__FILE__, __func__, LOG_EVENT,"Closing  REST-API..");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);
}