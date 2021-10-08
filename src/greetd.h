#ifndef GREETD_H
#define GREETD_H

#include <json-c/json_object.h>

struct json_object *greetd_create_session(const char *username);
struct json_object *greetd_post_auth_message_response(const char *response);
struct json_object *greetd_start_session(const char *command);
struct json_object *greetd_cancel_session(void);

#endif /* GREETD_H */
