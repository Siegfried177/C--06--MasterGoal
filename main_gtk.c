#include <gtk/gtk.h>

#define ROWS 15
#define COLS 11

int team1[5][2] = {{11, 5}, {10, 3}, {10, 7}, {8, 8}, {8, 2}};
int team2[5][2] = {{3, 5}, {4, 3}, {4, 7}, {6, 8}, {6, 2}};
int ball[2] = {7, 5}, coords[2] = {-1, -1}, click_ply, turn;
GtkWidget *grid;
void print_pitch();
static void on_button_clicked(GtkWidget *button, gpointer data);

// Función para verificar si una casilla contiene un jugador
// Parametros: x es la posicion en x, y es la posicion en y
// Retorno: -1 si no hay un jugador en esa casilla, 
//         i que es el indice del jugador si es del team 1 y i + 5 si es del team 2, 0 si es el portero del team 1 y 5 si es el portero del team 2
int check_player(int x, int y){
    for (int i = 1; i < 5; i++){
        if (x == team1[i][0] && y == team1[i][1]) return i;
        if (x == team2[i][0] && y == team2[i][1]) return i + 5;
    }
    if (x == team1[0][0] && y == team1[0][1]) return 0;
    if (x == team2[0][0] && y == team2[0][1]) return 5;
    return -1;
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
GtkWidget *load_scaled_image(const char *filename, int width, int height){
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    if (pixbuf == NULL){
        g_warning("Error cargando la imagen: %s", filename);
        return NULL;
    }
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
    GtkWidget *image = gtk_image_new_from_pixbuf(scaled_pixbuf);

    g_object_unref(pixbuf);
    g_object_unref(scaled_pixbuf); 
    return image;
}

int verify_move(int x, int y, int obj){
    if (y > 10 || y < 0) return 0;
    for (int i = 0; i < 5; i++) // Verifica si ya hay un jugador en la casilla
        if ((x == team1[i][0] && y == team1[i][1]) || (x == team2[i][0] && y == team2[i][1])) return 0;
    if (! obj){ // Si es un jugador
        if ((x == ball[0] && y == ball[1]) || (x < 1 || x > 13)) return 0;
    }
    else if (obj == 1 && (x < 0 || x > 14)) return 0; // Si es la pelota y está fuera de las filas del campo: retorna 0
    else if (obj == 1 && ((x == 0 || x == 14) && (y < 3 || y > 7))) return 0; // Si es la pelota y está en las ultimas filas pero no dentro de la porteria: retorna 0
    return 1;
}

// Función para ver si una casilla esta en el rango de movimiento de un jugador
// Parametros: x e y son las coordenadas de la casilla adonde se quiere mover, ply_x e ply_y son las coordenadas del jugador
int possible_move(int x, int y, int ply_x, int ply_y){
    if (x < ply_x - 2 || x > ply_x + 2 || y < ply_y - 2 || y > ply_y + 2) // Verificar si están en el rango de 2 antes o 2 más del jugador
        return 0;
    if ((x == ply_x - 2 && (y == ply_y + 1 || y == ply_y - 1)) || (x == ply_x + 2 && (y == ply_y + 1 || y == ply_y - 1))) // Verificar para las filas que están 2 antes o 2 después
        return 0;
    if ((y == ply_y - 2 && (x == ply_x + 1 || x == ply_x - 1)) || (y == ply_y + 2 && (x == ply_x + 1 || x == ply_x - 1))) // Verificar para las columnas que están 2 antes o 2 después
        return 0;
    return 1;
}

void move(int *x, int *y, int move_x, int move_y){
    *x += move_x;
    *y += move_y;
}

// Callback al clickear un botón
void on_button_clicked(GtkWidget *button, gpointer data){
    int x, y;
    const gchar *id = g_object_get_data(G_OBJECT(button), "id"); // Tomamos el ID del boton clickeado
    sscanf(id, "btn_%d_%d", &x, &y); // Recuperamos las coordenadas del boton clickeado
    if (coords[0] == -1 && coords[1] == -1){ // Si no hay ningun jugador seleccionado
        click_ply = check_player(x, y);
        if (click_ply != -1){
            if ((click_ply < 5 && turn % 2 == 0) || (click_ply > 4 && turn % 2 == 1))
                move(&coords[0], &coords[1], x - coords[0], y - coords[1]);
        }
    }
    else{ // Si se seleccionó un jugador previamente
        if (verify_move(x, y, 0) && possible_move(x, y, team1[click_ply][0], team1[click_ply][1]) && ! (turn % 2)){
            move(&team1[click_ply][0], &team1[click_ply][1], x - team1[click_ply][0], y - team1[click_ply][1]);
            turn++;
            print_pitch();
        }
        else if (verify_move(x, y, 0) && possible_move(x, y, team2[click_ply - 5][0], team2[click_ply - 5][1]) && turn % 2){
            move(&team2[click_ply - 5][0], &team2[click_ply - 5][1], x - team2[click_ply - 5][0], y - team2[click_ply - 5][1]);
            turn++;
            print_pitch();
        }
        move(&coords[0], &coords[1], -1 - coords[0], -1 - coords[1]);
    }
}

// Función para generar y mostrar el campo de juego
void print_pitch(){
    gtk_container_foreach(GTK_CONTAINER(grid), (GtkCallback)gtk_widget_destroy, NULL); // Eliminamos los widgets para volver a mostrarlos
    for (int i = 0; i < ROWS; i++)
        for (int j = 0, flag = 0; j < COLS; j++){
            GtkWidget *overlay = gtk_overlay_new(); // Se crea el overlay que tendrá las imagenes y el boton para cada casilla
            flag = check_player(i, j);

            gtk_widget_set_size_request(overlay, 60, 60);
            if (i == ball[0] && j == ball[1]) flag = -2;

            if (flag != -1){ // Si hay un jugador en la casilla i, j
                GtkWidget *image = load_scaled_image(flag == -2 ? "ball.png" : flag == 0 ? "red_gk.png": flag < 5 ? "red.png" : flag == 5 ? "blue_gk.png" : "blue.png", 60, 60);
                if (image != NULL){
                    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), image);
                    gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
                    gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
                }
            }

            GtkWidget *button = gtk_button_new(); // Se crea un botón
            gchar id[10];
            g_snprintf(id, sizeof(id), "btn_%d_%d", i, j);
            g_object_set_data(G_OBJECT(button), "id", g_strdup(id)); // Se le asigna el ID
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
            gtk_widget_set_hexpand(button, TRUE);
            gtk_widget_set_vexpand(button, TRUE);

            gtk_overlay_add_overlay(GTK_OVERLAY(overlay), button); // Se añade al overlay
            gtk_grid_attach(GTK_GRID(grid), overlay, j, i, 1, 1); // Se añade al grid
        }
    gtk_widget_show_all(grid);
}

int main(int argc, char *argv[]){
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "MasterGoal");
    gtk_widget_set_name(window, "main-window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *hbox_centered = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(hbox_centered), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_centered, TRUE, TRUE, 0);

    grid = gtk_grid_new();
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(grid, TRUE);
    gtk_box_pack_start(GTK_BOX(hbox_centered), grid, TRUE, TRUE, 0);

    print_pitch();

    apply_css(window, "styles.css");
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
