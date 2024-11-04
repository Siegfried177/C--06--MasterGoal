#include <gtk/gtk.h>

#define ROWS 15
#define COLS 11
int team1[5][2] = {{11, 5}, {10, 3}, {10, 7}, {8, 8}, {8, 2}};
int team2[5][2] = {{3, 5}, {4, 3}, {4, 7}, {6, 8}, {6, 2}};

// Función para verificar si una casilla contiene un jugador
// Parametros: x e y son las coordenadas de la casilla
// Retorno: 1 si la casilla contiene un jugador, 0 en caso contrario
int check_player(int x, int y) {
    for (int i = 0; i < 5; i++){
        if (x == team1[i][0] && y == team1[i][1]) 
            return 1;
        if (x == team2[i][0] && y == team2[i][1]) 
            return 2;
    }
    return 0;
}

// Función para aplicar CSS
void apply_css(GtkWidget *widget, const char *css_file_path){
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_css_provider_load_from_path(provider, css_file_path, NULL);
    g_object_unref(provider); // Liberar el proveedor después de usarlo
}

// Función para cargar y escalar una imagen
GtkWidget *load_scaled_image(const char *filename, int width, int height) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    if (pixbuf == NULL) {
        g_warning("Error cargando la imagen: %s", filename);
        return NULL;
    }
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
    GtkWidget *image = gtk_image_new_from_pixbuf(scaled_pixbuf);

    g_object_unref(pixbuf); // Liberar el pixbuf original
    g_object_unref(scaled_pixbuf);  // Liberar el pixbuf escalado
    return image;
}

int main(int argc, char *argv[]){
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "MasterGoal");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 800);
    gtk_widget_set_name(window, "main-window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *hbox_centered = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(hbox_centered), TRUE); 
    gtk_box_pack_start(GTK_BOX(vbox), hbox_centered, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(grid, TRUE);
    gtk_box_pack_start(GTK_BOX(hbox_centered), grid, FALSE, FALSE, 0);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            GtkWidget *overlay = gtk_overlay_new();
            gtk_widget_set_size_request(overlay, 60, 60);

            if (check_player(i, j)) {
                GtkWidget *image = load_scaled_image(check_player(i, j) == 1 ? "red.png" : "blue.png", 60, 60);
                if (image != NULL) {
                    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), image);
                    gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
                    gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
                }
            }

            GtkWidget *button = gtk_button_new();
            gchar id[10];
            g_snprintf(id, sizeof(id), "btn_%d_%d", i, j);
            g_object_set_data(G_OBJECT(button), "my_id", id);
            gtk_widget_set_size_request(button, 60, 60);
            gtk_widget_set_hexpand(button, FALSE);
            gtk_widget_set_vexpand(button, TRUE);

            gtk_overlay_add_overlay(GTK_OVERLAY(overlay), button);
            gtk_grid_attach(GTK_GRID(grid), overlay, j, i, 1, 1); 
        }
    }

    apply_css(window, "styles.css");
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}