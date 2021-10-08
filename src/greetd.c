#include "ipc.h"
#include <json-c/json_object.h>

struct json_object *greetd_create_session(const char *username)
{
	struct json_object *request = json_object_new_object();
	
	struct json_object *type = json_object_new_string("create_session");
	json_object_object_add(request, "type", type);

	struct json_object *name = json_object_new_string(username);
	json_object_object_add(request, "username", name);

	struct json_object *response = ipc_submit(request);
	json_object_put(request);
	return response;
}

struct json_object *greetd_post_auth_message_response(const char *response)
{
	struct json_object *request = json_object_new_object();
	
	struct json_object *type = json_object_new_string("post_auth_message_response");
	json_object_object_add(request, "type", type);

	if (response != NULL) {
		struct json_object *resp = json_object_new_string(response);
		json_object_object_add(request, "response", resp);
	}

	struct json_object *resp = ipc_submit(request);
	json_object_put(request);
	return resp;
}

struct json_object *greetd_start_session(const char *command)
{
	struct json_object *request = json_object_new_object();
	
	struct json_object *type = json_object_new_string("start_session");
	json_object_object_add(request, "type", type);

	struct json_object *arr = json_object_new_array_ext(1);
	struct json_object *cmd = json_object_new_string(command);
	json_object_array_add(arr, cmd);
	json_object_object_add(request, "cmd", cmd);

	struct json_object *resp = ipc_submit(request);
	json_object_put(request);
	return resp;
}

struct json_object *greetd_cancel_session(void)
{
	struct json_object *request = json_object_new_object();
	
	struct json_object *type = json_object_new_string("cancel_session");
	json_object_object_add(request, "type", type);

	struct json_object *resp = ipc_submit(request);
	json_object_put(request);
	return resp;
}
