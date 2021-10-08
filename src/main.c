#include "greetd.h"
#include <json-c/json_object.h>
#include <stdio.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

static void initialise(GtkApplication *app, gpointer data);
static void handle_options(GtkApplication *app, GVariantDict *options, gpointer data);
static void on_submit_password(GtkPasswordEntry *entry, gpointer data);
static void set_label_to_field(struct json_object *object, const char *field);
static void create_session(void);
static void restart_session(void);

static struct {
	GtkApplication *app;
	GtkLabel *label;
	GtkPasswordEntry *entry;
	struct json_object *response;
	const char *user;
	const char *command;
} greeter;

static GOptionEntry entries[2] = {
	{"user", 'u', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &greeter.user, "The user to login as", ""},
	{"command", 'c', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &greeter.command, "The command to run", ""}
};

int main(int argc, char **argv)
{
	GtkApplication *app = gtk_application_new("com.philj56.greetd-mini-greeter", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(initialise), NULL);
	g_signal_connect(app, "handle-local-options", G_CALLBACK(handle_options), NULL);

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

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_window_set_application(GTK_WINDOW(window), app);

	greeter.label = GTK_LABEL(gtk_builder_get_object(builder, "message"));

	greeter.entry = GTK_PASSWORD_ENTRY(gtk_builder_get_object(builder, "entry"));

	GdkDisplay *display = gtk_widget_get_display(window);
	GtkCssProvider *css = gtk_css_provider_new();
	gtk_css_provider_load_from_path(css, GTK_CSS_PATH);
	gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	g_signal_connect(G_OBJECT(greeter.entry), "activate", G_CALLBACK(on_submit_password), app);

	gtk_widget_show(window);
	gtk_window_fullscreen(GTK_WINDOW(window));
	create_session();
}

void handle_options(GtkApplication *app, GVariantDict *options, gpointer data)
{
	GVariant *user = g_variant_dict_lookup_value(options, "user", G_VARIANT_TYPE_STRING);
	if (user == NULL) {
		fprintf(stderr, "A user must be specified with -u / --user\n");
		greeter.user = "nobody";
	} else {
		greeter.user = g_variant_dup_string(user, NULL);
	}
	GVariant *command = g_variant_dict_lookup_value(options, "command", G_VARIANT_TYPE_STRING);
	if (user == NULL) {
		fprintf(stderr, "A command must be specified with -c / --command\n");
		greeter.command = "false";
	} else {
		greeter.command = g_variant_dup_string(command, NULL);
	}
}

void on_submit_password(GtkPasswordEntry *entry, gpointer data)
{
	json_object_put(greeter.response);
	greeter.response = greetd_post_auth_message_response(gtk_editable_get_text(GTK_EDITABLE(entry)));
	switch (greetd_parse_response_type(greeter.response)) {
		case GREETD_RESPONSE_SUCCESS:
			json_object_put(greeter.response);
			greetd_start_session(greeter.command);
			g_application_quit(G_APPLICATION(greeter.app));
			return;
		case GREETD_RESPONSE_ERROR:
			set_label_to_field(greeter.response, "description");
			restart_session();
			return;
		case GREETD_RESPONSE_AUTH_MESSAGE:
			set_label_to_field(greeter.response, "auth_message");
			break;
		default:
			restart_session();
			return;
	}
}

void set_label_to_field(struct json_object *object, const char *field)
{
	gtk_label_set_text(greeter.label,json_object_get_string(json_object_object_get(object,field)));
}

void create_session()
{
	greeter.response = greetd_create_session(greeter.user);
	switch (greetd_parse_response_type(greeter.response)) {
		case GREETD_RESPONSE_SUCCESS:
			g_application_quit(G_APPLICATION(greeter.app));
			return;
		case GREETD_RESPONSE_ERROR:
			greetd_cancel_session();
			g_application_quit(G_APPLICATION(greeter.app));
			return;
		case GREETD_RESPONSE_AUTH_MESSAGE:
			set_label_to_field(greeter.response, "auth_message");
			if (greetd_parse_auth_message_type(greeter.response) == GREETD_AUTH_MESSAGE_VISIBLE) {
				gtk_password_entry_set_show_peek_icon(greeter.entry, true);
			} else {
				gtk_password_entry_set_show_peek_icon(greeter.entry, false);
			}
			break;
		default:
			greetd_cancel_session();
			g_application_quit(G_APPLICATION(greeter.app));
			return;
	}
}

void restart_session()
{
	greetd_cancel_session();
	greetd_create_session(greeter.user);
}
