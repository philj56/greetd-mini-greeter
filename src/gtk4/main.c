#include "../greetd.h"
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <json-c/json_object.h>
#include <stdio.h>

static void initialise(GtkApplication *app, gpointer data);
static void on_submit_answer(GtkPasswordEntry *entry, gpointer data);
static void on_confirm(GtkButton *button, gpointer data);
static void set_label_to_field(struct json_object *object, const char *field);
static void set_error_to_field(struct json_object *object, const char *field);
static void create_session(void);
static void post_auth_message_response(const char *response);
static void start_session(void);
static void cancel_session(void);
static void restart_session(void);

static struct {
	GtkApplication *app;
	GtkLabel *label;
	GtkLabel *error;
	GtkPasswordEntry *password;
	GtkEntry *plaintext;
	GtkButton *ok;
	const char *user;
	const char *command;
	const char *css_path;
} greeter = {
	.user = "nobody",
	.command = "false",
	.css_path = GTK_CSS_PATH,
};

static GOptionEntry entries[3] = {
	{"user", 'u', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &greeter.user, "The user to login as", ""},
	{"command", 'c', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &greeter.command, "The command to run", ""},
	{"style", 's', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &greeter.css_path, "The css file to use for styling", ""}
};

int main(int argc, char **argv)
{
	GtkApplication *app = gtk_application_new("com.philj56.greetd-mini-greeter", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(initialise), NULL);

	g_application_add_main_option_entries(G_APPLICATION(app), entries);

	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}

void initialise(GtkApplication *app, gpointer data)
{
	greeter.app = app;

	GError *error = NULL;
	GtkBuilder *builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, GTK_UI_PATH, &error) == 0) {
		fprintf(stderr, "Error loading UI description: %s\n", error->message);
		g_clear_error(&error);
		exit(EXIT_FAILURE);
	}

	GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
	gtk_window_set_application(window, app);

	greeter.label = GTK_LABEL(gtk_builder_get_object(builder, "message"));
	greeter.error = GTK_LABEL(gtk_builder_get_object(builder, "error"));
	greeter.password = GTK_PASSWORD_ENTRY(gtk_builder_get_object(builder, "password"));
	greeter.plaintext = GTK_ENTRY(gtk_builder_get_object(builder, "plaintext"));
	greeter.ok = GTK_BUTTON(gtk_builder_get_object(builder, "ok"));

	GdkDisplay *display = gtk_widget_get_display(GTK_WIDGET(window));
	GtkCssProvider *css = gtk_css_provider_new();
	gtk_css_provider_load_from_path(css, greeter.css_path);
	gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	g_signal_connect(G_OBJECT(greeter.password), "activate", G_CALLBACK(on_submit_answer), app);
	g_signal_connect(G_OBJECT(greeter.plaintext), "activate", G_CALLBACK(on_submit_answer), app);
	g_signal_connect(G_OBJECT(greeter.ok), "clicked", G_CALLBACK(on_confirm), app);

	gtk_widget_show(GTK_WIDGET(window));
	create_session();
}

void on_submit_answer(GtkPasswordEntry *entry, gpointer data)
{
	post_auth_message_response(gtk_editable_get_text(GTK_EDITABLE(entry)));
}

void on_confirm(GtkButton *button, gpointer data)
{
	gtk_widget_hide(GTK_WIDGET(button));
	gtk_widget_hide(GTK_WIDGET(greeter.error));
	restart_session();
}

void set_label_to_field(struct json_object *object, const char *field)
{
	gtk_label_set_text(greeter.label, json_object_get_string(json_object_object_get(object,field)));
}

void set_error_to_field(struct json_object *object, const char *field)
{
	gtk_label_set_text(greeter.error, json_object_get_string(json_object_object_get(object,field)));
}

void handle_response(struct json_object *response, enum greetd_request_type request)
{
	if (response == NULL) {
		return;
	}
	enum greetd_response_type type = greetd_parse_response_type(response);

	switch (type) {
		case GREETD_RESPONSE_SUCCESS:
			switch (request) {
				case GREETD_REQUEST_CREATE_SESSION:
				case GREETD_REQUEST_POST_AUTH_MESSAGE_RESPONSE:
					start_session();
					break;
				case GREETD_REQUEST_START_SESSION:
					g_application_quit(G_APPLICATION(greeter.app));
					break;
				case GREETD_REQUEST_CANCEL_SESSION:
					break;
			}
			break;
		case GREETD_RESPONSE_ERROR:
			switch (request) {
				case GREETD_REQUEST_POST_AUTH_MESSAGE_RESPONSE:
				case GREETD_REQUEST_START_SESSION:
					set_error_to_field(response, "description");
					gtk_widget_show(GTK_WIDGET(greeter.error));
					gtk_widget_show(GTK_WIDGET(greeter.ok));
					gtk_widget_hide(GTK_WIDGET(greeter.plaintext));
					gtk_widget_hide(GTK_WIDGET(greeter.password));
					break;
				case GREETD_REQUEST_CREATE_SESSION:
					g_application_quit(G_APPLICATION(greeter.app));
					break;
				case GREETD_REQUEST_CANCEL_SESSION:
					break;
			}
			break;
		case GREETD_RESPONSE_AUTH_MESSAGE:
			switch (greetd_parse_auth_message_type(response)) {
				case GREETD_AUTH_MESSAGE_VISIBLE:
					set_label_to_field(response, "auth_message");
					gtk_widget_hide(GTK_WIDGET(greeter.password));
					gtk_widget_show(GTK_WIDGET(greeter.plaintext));
					gtk_editable_set_text(GTK_EDITABLE(greeter.plaintext), "");
					gtk_widget_grab_focus(GTK_WIDGET(greeter.plaintext));
					break;
				case GREETD_AUTH_MESSAGE_SECRET:
					set_label_to_field(response, "auth_message");
					gtk_widget_hide(GTK_WIDGET(greeter.plaintext));
					gtk_widget_show(GTK_WIDGET(greeter.password));
					gtk_editable_set_text(GTK_EDITABLE(greeter.password), "");
					gtk_widget_grab_focus(GTK_WIDGET(greeter.password));
					break;
				case GREETD_AUTH_MESSAGE_INFO:
				case GREETD_AUTH_MESSAGE_ERROR:
					set_error_to_field(response, "auth_message");
					gtk_widget_show(GTK_WIDGET(greeter.error));
					gtk_widget_show(GTK_WIDGET(greeter.ok));
					gtk_widget_hide(GTK_WIDGET(greeter.plaintext));
					gtk_widget_hide(GTK_WIDGET(greeter.password));
					break;
				case GREETD_AUTH_MESSAGE_INVALID:
					break;
			}
			break;
		case GREETD_RESPONSE_INVALID:
			break;
	}
	json_object_put(response);
}

void create_session()
{
	handle_response(greetd_create_session(greeter.user), GREETD_REQUEST_CREATE_SESSION);
}

void start_session()
{
	handle_response(greetd_start_session(greeter.command), GREETD_REQUEST_START_SESSION);
}

void post_auth_message_response(const char *response)
{
	handle_response(greetd_post_auth_message_response(response), GREETD_REQUEST_POST_AUTH_MESSAGE_RESPONSE);
}

void cancel_session()
{
	handle_response(greetd_cancel_session(), GREETD_REQUEST_CANCEL_SESSION);
}

void restart_session()
{
	cancel_session();
	create_session();
}
