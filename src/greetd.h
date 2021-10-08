#ifndef GREETD_H
#define GREETD_H

#include <json-c/json_object.h>

enum greetd_response_type {
	GREETD_RESPONSE_INVALID,
	GREETD_RESPONSE_SUCCESS,
	GREETD_RESPONSE_ERROR,
	GREETD_RESPONSE_AUTH_MESSAGE
};

enum greetd_auth_message_type {
	GREETD_AUTH_MESSAGE_INVALID,
	GREETD_AUTH_MESSAGE_VISIBLE,
	GREETD_AUTH_MESSAGE_INVISIBLE,
	GREETD_AUTH_MESSAGE_INFO,
	GREETD_AUTH_MESSAGE_ERROR
};

struct json_object *greetd_create_session(const char *username);
struct json_object *greetd_post_auth_message_response(const char *response);
struct json_object *greetd_start_session(const char *command);
struct json_object *greetd_cancel_session(void);

enum greetd_response_type greetd_parse_response_type(struct json_object *response);
enum greetd_auth_message_type greetd_parse_auth_message_type(struct json_object *response);

#endif /* GREETD_H */
