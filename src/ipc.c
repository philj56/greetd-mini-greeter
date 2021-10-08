#include "ipc.h"
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static int ipc_open(void);
static int ipc_send(int socket, struct json_object *request);
static struct json_object *ipc_receive(int socket);

struct json_object *ipc_submit(struct json_object *request)
{
	int sock = ipc_open();
	if (sock == -1) {
		return NULL;
	}

	if (ipc_send(sock, request) == -1) {
		close(sock);
		return NULL;
	}

	struct json_object *resp = ipc_receive(sock);
	close(sock);
	return resp;
}

int ipc_open(void)
{
	char *greetd_sock = getenv("GREETD_SOCK");

	if (greetd_sock == NULL) {
		fprintf(stderr, "GREETD_SOCK not set.\n");
		return -1;
	}

	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("Unable to create socket");
		return -1;
	}

	struct sockaddr_un remote = { .sun_family = AF_UNIX };
	strncpy(remote.sun_path, greetd_sock, sizeof(remote.sun_path));

	if (connect(sock, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
		perror("Unable to connect to greetd socket");
		close(sock);
		return -1;
	}
	return sock;
}

int ipc_send(int sock, struct json_object *request)
{
	const char *str = json_object_to_json_string(request);
	uint32_t len = strlen(str);
	
	if (send(sock, &len, sizeof(len), 0) == -1) {
		perror("Error sending request size");
		return -1;
	}

	if (send(sock, str, len, 0) == -1) {
		perror("Error sending request");
		return -1;
	}
	return 0;
}

struct json_object *ipc_receive(int sock)
{
	uint32_t len = 0;

	if (recv(sock, &len, sizeof(len), 0) != sizeof(len)) {
		perror("Error receiving response size");
		return NULL;
	}

	char *buf = malloc(len + 1);
	if (recv(sock, buf, len, 0) != len) {
		perror("Error receiving response");
		free(buf);
		return NULL;
	}

	buf[len] = '\0';

	enum json_tokener_error error;
	struct json_object *resp = json_tokener_parse_verbose(buf, &error);
	free(buf);

	if (resp == NULL) {
		fprintf(stderr, "Error parsing response: %s\n", json_tokener_error_desc(error));
	}
	return resp;

}
