/**
 *   author       :   丁雪峰
 *   time         :   2015-06-05 22:47:00
 *   email        :   fengidri@yeah.net
 *   version      :   1.0.1
 *   description  :
 */
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#include <webkit2/webkit2.h>


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

int main(int argc, char *argv[])
{
  //GtkBuilder *builder;
  //GtkWidget *scrolledWindow;

  //gtk_init (&argc, &argv);

  ///* Construct a GtkBuilder instance and load our UI description */
  //builder = gtk_builder_new ();
  //gtk_builder_add_from_file (builder, "builder.ui", NULL);

  // Create a browser instance

  /* Connect signal handlers to the constructed widgets. */
  //main_window = gtk_builder_get_object (builder, "main_window");
  //g_signal_connect (main_window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  //scrolledWindow = GTK_WIDGET(gtk_builder_get_object (builder, "scrolledWindow"));
  //gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(webView));

  //// Set up callbacks so that if either the main window or the browser instance is
  //// closed, the program will exit
  //g_signal_connect(main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
  //g_signal_connect(webView, "close-web-view", G_CALLBACK(closeWebViewCb), main_window);

  // Load a web page into the browser instance
  gtk_init(0, NULL);
  GtkWidget *main_window = gtk_offscreen_window_new();
  WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new());

  gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(webView));
  webkit_web_view_load_uri(webView, "http://www.baidu.com/");



    gtk_widget_grab_focus(GTK_WIDGET(webView));
  // webView in not shown yet
  gtk_widget_show_all(GTK_WIDGET(main_window));

  webkit_web_view_get_snapshot(webView,
          WEBKIT_SNAPSHOT_REGION_FULL_DOCUMENT,
          WEBKIT_SNAPSHOT_OPTIONS_NONE,
          NULL,
          get_snapshot_finish,
          webView
          );
  gtk_main();
  return 0;
}
