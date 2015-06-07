/**
 *   author       :   丁雪峰
 *   time         :   2015-06-06 06:54:52
 *   email        :   fengidri@yeah.net
 *   version      :   1.0.1
 *   description  :
 */
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <webkit2/webkit2.h>
#include <JavaScriptCore/JSStringRef.h>
#include <JavaScriptCore/JSValueRef.h>
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
static void web_view_javascript_finished(GObject      *object,
                              GAsyncResult *result,
                              gpointer      user_data)
{
    WebKitJavascriptResult *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    GError                 *error = NULL;

    js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);
    if (!js_result) {
        g_warning ("Error running javascript: %s", error->message);
        g_error_free (error);
        return;
    }
    printf("###########\n");

    context = webkit_javascript_result_get_global_context(js_result);
    value = webkit_javascript_result_get_value(js_result);
    if (JSValueIsString(context, value)) {
        JSStringRef js_str_value;
        gchar      *str_value;
        gsize       str_length;
    printf("###########\n");

        js_str_value = JSValueToStringCopy(context, value, NULL);
    printf("2###########\n");
        str_length = JSStringGetMaximumUTF8CStringSize(js_str_value);
    printf("3###########\n");
        str_value = (gchar *)g_malloc(str_length);
        JSStringGetUTF8CString(js_str_value, str_value, str_length);
        JSStringRelease(js_str_value);
        g_print("$$$$$$$$$$$$$$$4Script result: %s\n", str_value);
        g_free (str_value);
    } else {
        g_warning("Error running javascript: unexpected return value");
    }
    webkit_javascript_result_unref(js_result);
}
static void web_view_load_changed(WebKitWebView  *web_view,
                                   WebKitLoadEvent load_event,
                                   gpointer        user_data)
{
    switch (load_event) {
    case WEBKIT_LOAD_STARTED:
        /* New load, we have now a provisional URI */
        printf("%s: load start\n", webkit_web_view_get_uri(web_view));
        /* Here we could start a spinner or update the
         * location bar with the provisional URI */
        break;
    case WEBKIT_LOAD_REDIRECTED:
        printf("%s: redirect\n", webkit_web_view_get_uri(web_view));
        break;
    case WEBKIT_LOAD_COMMITTED:
        /* The load is being performed. Current URI is
         * the final one and it won't change unless a new
         * load is requested or a navigation within the
         * same page is performed */
        printf("%s: commit\n", webkit_web_view_get_uri(web_view));
        break;
    case WEBKIT_LOAD_FINISHED:
        printf("%s: finished\n", webkit_web_view_get_uri(web_view));
        /* Load finished, we can now stop the spinner */
        webkit_web_view_get_snapshot(web_view,
                WEBKIT_SNAPSHOT_REGION_FULL_DOCUMENT,
                WEBKIT_SNAPSHOT_OPTIONS_NONE,
                NULL,
                get_snapshot_finish,
                web_view
                );
        const gchar *script = "JSON.stringify(window.performance.timing)";
        script = "console.log('@#@#@#----console-')";
        webkit_web_view_run_javascript(web_view, script, NULL, web_view_javascript_finished, NULL);
        break;
    }
}

void ready_to_show(WebKitWebView *webView)
{
    printf("---------------ready-to-show\n");
    //webkit_web_view_get_snapshot(webView,
    //        WEBKIT_SNAPSHOT_REGION_FULL_DOCUMENT,
    //        WEBKIT_SNAPSHOT_OPTIONS_NONE,
    //        NULL,
    //        get_snapshot_finish,
    //        webView
    //        );
        const gchar *script = "JSON.stringify(window.performance.timing)";
        webkit_web_view_run_javascript(webView, script, NULL, web_view_javascript_finished, NULL);

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

static void download_proc(WebKitDownload *download,
               guint64         data_length,
               gpointer        user_data)
{
    const gchar * url;
    printf("dwonload proc\n");
    url = webkit_uri_request_get_uri(webkit_download_get_request(download));
    printf("%s: %"G_GUINT64_FORMAT"\n", url, data_length);

}
static void download_finished(WebKitDownload *download,
               gpointer        user_data)
{
    const gchar * url;
    url = webkit_uri_request_get_uri(webkit_download_get_request(download));
    const guint64 size = webkit_download_get_received_data_length(download);
    printf("!!!!!!!!!!!!!!!%s: %"G_GUINT64_FORMAT"\n", url, size);

}
static gboolean download_dest(WebKitDownload *download,
        gchar          *suggested_filename,
        gpointer        user_data)
{
    webkit_download_set_destination(download,
            g_build_filename("file:///tmp/cache", suggested_filename, NULL));
    printf("destination:%s\n", webkit_download_get_destination(download));
    return true;

}

void resource_started(WebKitWebView   *web_view,
               WebKitWebResource *resource,
               WebKitURIRequest  *request,
               gpointer           user_data)
{
    const gchar * url;
    WebKitDownload *download;
    url = webkit_web_resource_get_uri(resource);
    printf("@@@Start resource:%s\n", url);
    //printf("@@@Start resource\n");

    download = webkit_web_view_download_uri(web_view, url);
    g_signal_connect(download, "received-data", G_CALLBACK(download_proc), NULL);
    g_signal_connect(download, "finished", G_CALLBACK(download_finished), NULL);
    g_signal_connect(download, "decide-destination", G_CALLBACK(download_dest), NULL);



}
int main(int argn, char **argv)
{

	gtk_init(0, NULL);
    //GtkWidget * window = gtk_offscreen_window_new();
    GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
    GdkScreen* screen = gtk_window_get_screen(GTK_WINDOW(window));
    GdkVisual* rgba_visual = gdk_screen_get_rgba_visual(screen);
    WebKitSettings * setting;
    WebKitWebView *webView;
    if (rgba_visual) {
        gtk_widget_set_visual(window, rgba_visual);
    }
    gtk_widget_set_app_paintable(window, TRUE);

	WebKitWebContext* context = webkit_web_context_get_default();
	//webkit_web_context_set_process_model(context, WEBKIT_PROCESS_MODEL_MULTIPLE_SECONDARY_PROCESSES);
	//webkit_web_context_set_process_model(context, WEBKIT_PROCESS_MODEL_SHARED_SECONDARY_PROCESS);
	//webkit_web_context_set_cache_model(context, WEBKIT_CACHE_MODEL_WEB_BROWSER);
    webkit_web_context_set_disk_cache_directory(context, "/tmp/cache");
    webkit_web_context_set_process_model(context, WEBKIT_PROCESS_MODEL_MULTIPLE_SECONDARY_PROCESSES);
	webkit_web_context_set_cache_model(context, WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER);
	webkit_web_context_set_tls_errors_policy(context, WEBKIT_TLS_ERRORS_POLICY_IGNORE);

    webkit_web_context_clear_cache(context);

	webView = WEBKIT_WEB_VIEW(
            webkit_web_view_new_with_user_content_manager(webkit_user_content_manager_new()));
    //webView =WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(context));
    setting = webkit_web_view_get_settings(webView);
    webkit_settings_set_enable_page_cache(setting, false);
    webkit_settings_set_enable_javascript(setting, true);
    webkit_settings_set_enable_offline_web_application_cache(setting, false);
	//webView = WEBKIT_WEB_VIEW(webkit_web_view_new_with_settings(setting));
    g_object_set(setting,
	//	"enable-private-browsing", NanBooleanOptionValue(opts, H("private"), false),
		"enable-plugins", FALSE,
		"print-backgrounds", TRUE,
		"enable-javascript", TRUE,
		"enable-html5-database", FALSE,
		"enable-html5-local-storage", FALSE,
		"enable-java", FALSE,
		"enable-page-cache", FALSE,
		"enable-write-console-messages-to-stdout", TRUE,
		"enable-offline-web-application-cache", FALSE,
	//	"auto-load-images", NanBooleanOptionValue(opts, H("images"), true),
		"zoom-text-only", FALSE,
		"media-playback-requires-user-gesture", FALSE, // effectively disables media playback ?
	//	"user-agent", ua,
        NULL
	);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));
	g_signal_connect(webView, "ready-to-show", G_CALLBACK(ready_to_show), NULL);
	g_signal_connect(webView, "load-changed", G_CALLBACK(web_view_load_changed), NULL);
	g_signal_connect(webView, "web-view-ready", G_CALLBACK(ready_to_show), NULL);

    g_signal_connect(webView, "resource-load-started", G_CALLBACK(resource_started), window);

    g_signal_connect(window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
    g_signal_connect(webView, "close-web-view", G_CALLBACK(closeWebViewCb), window);

    webkit_web_view_load_uri(webView, argv[1]);
	gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
