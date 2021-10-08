#include "greetd.h"
#include <json-c/json_object.h>
#include <stdio.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

static void initialise(GtkApplication *app, gpointer data);
static void on_submit_password(GtkPasswordEntry *entry, gpointer data);

int main(int argc, char **argv)
{
	GtkApplication *app = gtk_application_new("com.philj56.greetd-mini-greeter", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(initialise), NULL);

	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}

void initialise(GtkApplication *app, gpointer data)
{
	GError *error = NULL;
	GtkBuilder *builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, "./greeter.ui", &error) == 0) {
		fprintf(stderr, "Error loading UI description: %s\n", error->message);
		g_clear_error(&error);
		exit(EXIT_FAILURE);
	}

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_window_set_application(GTK_WINDOW(window), app);

	GtkPasswordEntry *entry = GTK_PASSWORD_ENTRY(gtk_builder_get_object(builder, "entry"));

	GdkDisplay *display = gtk_widget_get_display(window);
	GtkCssProvider *css = gtk_css_provider_new();
	gtk_css_provider_load_from_path(css, "./greeter.css");
	gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(on_submit_password), NULL);

	gtk_widget_show(window);
}

void on_submit_password(GtkPasswordEntry *entry, gpointer data)
{
	greetd_create_session("phil");
}
