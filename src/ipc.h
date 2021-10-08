#ifndef IPC_H
#define IPC_H

#include <json-c/json_object.h>

struct json_object *ipc_submit(struct json_object *request);

#endif /* IPC_H */
