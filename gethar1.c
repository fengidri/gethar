/**
 *   author       :   丁雪峰
 *   time         :   2015-06-06 00:24:14
 *   email        :   fengidri@yeah.net
 *   version      :   1.0.1
 *   description  :
 */
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>


static void destroyWindowCb(GtkWidget* widget, GtkWidget* window);
static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window);
void get_snapshot_finish(GObject *object,
                                     GAsyncResult *result,
                                     gpointer data)
{
    printf("get_snapshot_finish\n");
    WebKitWebView * webview = (WebKitWebView*)data;
    printf("1get_snapshot_finish\n");
    GError* error = NULL;
	cairo_surface_t* surface = webkit_web_view_get_snapshot_finish(webview, result, &error);
    printf("2get_snapshot_finish:%x\n", surface);
	cairo_surface_write_to_png(surface, "/tmp/webkitgtk_test.png");
    printf("3get_snapshot_finish\n");
}

int main(int argc, char* argv[])
{
    // Initialize GTK+
    gtk_init(&argc, &argv);

    // Create an 800x600 window that will contain the browser instance
    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 900, 600);

    WebKitWebContext* context = webkit_web_context_get_default();
    // Create a browser instance
    WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(webkit_user_content_manager_new()));

    // Create a scrollable area, and put the browser instance into it
    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(webView));

    // Set up callbacks so that if either the main window or the browser instance is
    // closed, the program will exit
    g_signal_connect(main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
    g_signal_connect(webView, "close-web-view", G_CALLBACK(closeWebViewCb), main_window);

    // Put the scrollable area into the main window
    gtk_container_add(GTK_CONTAINER(main_window), scrolledWindow);

    // Load a web page into the browser instance
    webkit_web_view_load_uri(webView, "http://www.qq.com/");

    // Make sure that when the browser area becomes visible, it will get mouse
    // and keyboard events
    gtk_widget_grab_focus(GTK_WIDGET(webView));

    // Make sure the main window and all its contents are visible
    gtk_widget_show_all(main_window);

  webkit_web_view_get_snapshot(webView,
          WEBKIT_SNAPSHOT_REGION_FULL_DOCUMENT,
          WEBKIT_SNAPSHOT_OPTIONS_NONE,
          NULL,
          get_snapshot_finish,
          webView
          );
    // Run the main GTK+ event loop
    gtk_main();

    return 0;
}


static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
    gtk_main_quit();
}

static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window)
{
    gtk_widget_destroy(window);
    return TRUE;
}
