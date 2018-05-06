#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *window;

static char *font = "Sans";
static char *color = "#000";
static char *background;
static char *format = "%H:%M";
static int size = 20;
static char *outline = "#fff";
static int thickness = 0;
static GdkColor text_color;
static GdkColor outline_color;
static GdkColor background_color;
static int x, y = 0;
static int gravity = 0;

static GOptionEntry options[] =
{
    { "format", 'd', 0, G_OPTION_ARG_STRING, &format, "Sets date format", NULL },
    { "font", 'f', 0, G_OPTION_ARG_STRING, &font, "Sets font family", NULL },
    { "color", 'c', 0, G_OPTION_ARG_STRING, &color, "Sets font color", NULL },
    { "background", 'b', 0, G_OPTION_ARG_STRING, &background, "Sets background color", NULL },
    { "size", 's', 0, G_OPTION_ARG_INT, &size, "Sets font size", NULL },
    { "outline", 'o', 0, G_OPTION_ARG_STRING, &outline, "Sets outline color", NULL },
    { "thickness", 't', 0, G_OPTION_ARG_INT, &thickness, "Sets outline thickness", NULL },
    { "x", 'x', 0, G_OPTION_ARG_INT, &x, "Sets horizontal position on the screen", NULL },
    { "y", 'y', 0, G_OPTION_ARG_INT, &y, "Sets vertical position on the screen", NULL },
    { "gravity", 'g', 0, G_OPTION_ARG_INT, &gravity, "Sets the anchor point of the window", NULL },
    { NULL }
};

static void configure(GtkWindow *window, GdkEvent *event, gpointer data);
static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer user_data);
static gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
static void button_press(GtkWidget *widget, GdkEventButton *event);
static gboolean tick(gpointer user_data);
static void draw_background(cairo_t *cr);
static void draw_text(cairo_t *cr, gchar *text);

int main(int argc, char **argv)
{
    GError* err = NULL;

    /* Parse command line arguments. */
    if(G_UNLIKELY(!gtk_init_with_args(&argc, &argv, " ", options, NULL, &err)))
    {
        g_printf("%s\n", err->message);
        g_error_free(err);
        return 1;
    }
    
    gdk_color_parse(color, &text_color);
    gdk_color_parse(outline, &outline_color);
    if (background)
        gdk_color_parse(background, &background_color);
    
    /* For convenience make negative x, y values represent the opposite
     * side of the screen. */
    if (x < 0)
        x = gdk_screen_width() + x;
    if (y < 0)
        y = gdk_screen_height() + y;
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "Desktop Clock");
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
    gtk_window_set_keep_below(GTK_WINDOW(window), TRUE);
    gtk_widget_set_app_paintable(window, TRUE);

    /* When clicking program will display color selection dialog. It will
     * need an event box to be able to capture events. Left mouse button
     * shows "Change text fill color" dialog and right mouse button shows 
     * "Change text outline color" dialog (second one is optional!).
     */
    GtkWidget *event_box = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
    g_signal_connect(G_OBJECT(event_box), "button-press-event", G_CALLBACK(button_press), NULL);

    g_signal_connect(G_OBJECT(window), "expose-event", G_CALLBACK(expose), NULL);
    g_signal_connect(G_OBJECT(window), "screen-changed", G_CALLBACK(screen_changed), NULL);
    
    tick(window);

    screen_changed(window, NULL, NULL);

    /* Put everything together and show it! */
    gtk_container_add(GTK_CONTAINER(window), event_box);
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

/* Timer function will update our clock and redraw it every second. */
static gboolean tick(gpointer user_data)
{
    gtk_widget_queue_draw(user_data);
    g_timeout_add(1000, tick, user_data);
    
    return FALSE;
}

static void color_changed(GtkColorSelection *color_sel, GdkColor *color)
{
    gtk_color_selection_get_current_color(color_sel, color);
    gtk_widget_queue_draw(window);
}

static void button_press(GtkWidget *widget, GdkEventButton *event)
{
    GdkColor *color_ptr;
    GdkColor tmp;

    switch (event->button) {
        case 1:
            color_ptr = &text_color;
            break;
        case 3:
            if (thickness == 0)
                return;
            color_ptr = &outline_color;
            break;
        default:
            return;
    }
    
    tmp = *color_ptr;

    GtkWidget* dialog = gtk_color_selection_dialog_new("Select color");
    GtkWidget* color_sel = GTK_COLOR_SELECTION_DIALOG(dialog)->colorsel;

    gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(color_sel), color_ptr);
    gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
    gtk_signal_connect(GTK_OBJECT(color_sel), "color_changed",
        (GtkSignalFunc)color_changed, color_ptr);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_CANCEL)
        *color_ptr = tmp;

    gtk_widget_queue_draw(window);
    gtk_widget_destroy(dialog);
}

static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
    /* To check if the display supports alpha channels, get the colormap */
    GdkScreen *screen = gtk_widget_get_screen(widget);
    GdkColormap *colormap = gdk_screen_get_rgba_colormap(screen);

    if (!colormap)
    {
        colormap = gdk_screen_get_rgb_colormap(screen);
    }

    /* Now we have a colormap appropriate for the screen, use it */
    gtk_widget_set_colormap(widget, colormap);
}

static void set_window_position(GtkWindow *window, int width, int height)
{
    gtk_window_set_gravity(window, (GdkGravity)gravity);
    
    switch ((GdkGravity)gravity) {
        default:
        case GDK_GRAVITY_NORTH_WEST:
            gtk_window_move(window, x, y);
            break;
        case GDK_GRAVITY_NORTH:
            gtk_window_move(window, x - width / 2, y);
            break;
        case GDK_GRAVITY_NORTH_EAST:
            gtk_window_move(window, x - width, y);
            break;
        case GDK_GRAVITY_CENTER:
            gtk_window_move(window, x - width, y - height / 2);
            break;
        case GDK_GRAVITY_SOUTH_WEST:
            gtk_window_move(window, x, y - height);
            break;
        case GDK_GRAVITY_SOUTH:
            gtk_window_move(window, x - width / 2, y - height);
            break;
        case GDK_GRAVITY_SOUTH_EAST:
            gtk_window_move(window, x - width, y - height);
            break;
    }
} 

static void set_source_color(cairo_t *cr, GdkColor *color)
{
    cairo_set_source_rgb(cr, 
        (float)color->red / (float)G_MAXUINT16,
        (float)color->green / (float)G_MAXUINT16, 
        (float)color->blue / (float)G_MAXUINT16);
        
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
}

/* This is called when we need to draw the clock */
static gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer userdata)
{
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(window));

    GDateTime *time = g_date_time_new_now_local();
    gchar *text = g_date_time_format(time, format);

    draw_background(cr);
    draw_text(cr, text);

    cairo_destroy (cr);
    g_free(text);
    
    return FALSE;
}

static void draw_background(cairo_t *cr)
{
    if (background)
    {
        set_source_color(cr, &background_color);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    }
    else {
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
        cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    }

    cairo_paint(cr);
}

static void draw_text(cairo_t *cr, gchar *text)
{
    cairo_set_font_size(cr, size);
    cairo_select_font_face(cr, font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);  
    
    int width = extents.width + thickness * 2;
    int height = extents.height + thickness * 2;

    gtk_window_resize(GTK_WINDOW(window), width, height);
    set_window_position(GTK_WINDOW(window), width, height);

    cairo_move_to(cr, width / 2 - (width / 2 + extents.x_bearing - thickness),
        height / 2 -(height / 2 + extents.y_bearing - thickness));

    set_source_color(cr, &text_color);
    cairo_show_text(cr, text);

    if (thickness == 0)
        return;
        
    set_source_color(cr, &outline_color);
    cairo_move_to(cr, width / 2 - (width / 2 + extents.x_bearing - thickness),
        height / 2 -(height / 2 + extents.y_bearing - thickness));
    cairo_text_path(cr, text);
    cairo_set_line_width(cr, thickness);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke(cr);
}
