#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#include "ai.h"
#include "functions.h"

// Declaramos las ventanas y los campos de entrada
GtkWidget *main_window, *config_window, *about_window, *grid, *board_window;
GtkWidget *P1_window, *P2_window, *start_window;
GtkWidget *entry_player1, *entry_player2;

#define ROWS 15
#define COLS 11
void print_pitch();
static void on_button_clicked(GtkWidget *button, gpointer data);
extern int team1[5][2];
extern int team2[5][2];
extern int ball[2];
extern int goals[2];
extern int last_moved_player;
extern int bot_passes;

// coords son las coordenadas de la casilla clickeada. bot es 0 si no juega la compu, 1 si juega. click_box. chosen_player es el indice del jugador seleccionado. turn es 0 si empieza el jugador 1 y 1 si empieza el jugador 2.
int coords[2] = {-1, -1}, bot = 1, click_box, chosen_player, turn = 0, color_p1, color_p2, passes = 0;
char *colors[15] = {"red.png", "blue.png", "yellow.png", "magenta.png"}, *colors_gk[15] = {"red_gk.png", "blue_gk.png", "yellow_gk.png", "magenta_gk.png"};


void move_bot(){
	while(turn % 2){
		int aux = -1;
		if (ball[0] <= 4){
			if (possesion(turn % 2)){
				aux = pass();
				if (! aux) break;
			}
			else{
				pressure();
			}
		}
		else{
			if (possesion(turn % 2)){
				aux = pass();
				if (! aux) break;
			}
			else{
				pressure();
			}
		}
	}
	bot_passes = 0;
	print_pitch();
}

// Funcion cuando un jugador marca un gol
void goal_scored(int player){
	goals[player]++;
	reset_pitch();
	if (goals[player] == 2) printf("El jugador %d ha ganado", player + 1);
	print_pitch();
}

// Funcion para saber si hay un jugador rival adyacente a la pelota
int check_near(int team, int x, int y){
    int c1 = 0, c2 = 0; // c1 es el contador para jugadores del equipo 1, c2 para los del equipo 2
    for(int i = -1; i < 2; i++)
        for(int j = -1; j < 2; j++)
            for(int k = 0; k < 5; k++)
            	if(x + i == team1[k][0] && y + j == team1[k][1]) c1++;
            	else if(x + i == team2[k][0] && y + j == team2[k][1]) c2++;
    if(team){
        if((! team && c1 > 0) || (team && c2 > 0)) return 1;
    }
    else{
        if((!team && c2 > c1) || (team &&  c1 > c2)) return 1;
    }
    return 0;
}

// Función para verificar si una casilla contiene un jugador o la pelota
int check_square(int x, int y){
    for (int i = 1; i < 5; i++) {
        if (x == team1[i][0] && y == team1[i][1]) return i;
        if (x == team2[i][0] && y == team2[i][1]) return i + 5;
    }
    if (x == team1[0][0] && y == team1[0][1]) return 0;
    if (x == team2[0][0] && y == team2[0][1]) return 5;
    if (x == ball[0] && y == ball[1]) return -2;
    return -1;
}

// Función para aplicar CSS
void apply_css_to_window(GtkWidget *window, const char *css_file) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GdkScreen *screen = gdk_screen_get_default();

    if (gtk_css_provider_load_from_file(css_provider, g_file_new_for_path(css_file), NULL)) {
        gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    } else g_print("Error al cargar el archivo CSS.\n");

    g_object_unref(css_provider);
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
    g_object_unref(scaled_pixbuf); // Liberar el pixbuf escalado
    return image;
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

// Función para ver si una casilla esta en el rango de movimiento de la pelota
// Parametros: x e y son las coordenadas de la casilla adonde se quiere mover
int possible_move_ball(int x, int y){
    if (x < ball[0] - 4 || x > ball[0] + 4 || y < ball[1] - 4 || y > ball[1] + 4) // Verificar si la pelota esta en el rango de 4 antes o 4 más de su distancia
        return 0;
    else if ((x - ball[0]) != (y - ball[1]) && (x - ball[0]) * (-1) != (y - ball[1]) && (x - ball[0]) != 0 && (y - ball[1]) != 0)
    	return 0;
    return 1;
}

void on_button_clicked(GtkWidget *button, gpointer data){
    int x, y;
    const gchar *id = g_object_get_data(G_OBJECT(button), "id"); // Tomamos el ID del boton clickeado
    sscanf(id, "btn_%d_%d", &x, &y); // Recuperamos las coordenadas del boton clickeado
    // Si no hay ningun jugador ni pelota seleccionado
    if (coords[0] == -1 && coords[1] == -1){
        click_box = check_square(x, y);
        if (click_box != -1){
        	if (click_box == -2 && possesion(turn % 2))
        		move(&coords[0], &coords[1], ball[0] - coords[0], ball[1] - coords[1]);
        	else if (((click_box < 5 && turn % 2 == 0) || (click_box > 4 && turn % 2 == 1)) && click_box != -2)
                move(&coords[0], &coords[1], x - coords[0], y - coords[1]);
        }
    }
    // Si se selecciono la pelota previamente
    else if (coords[0] == ball[0] && coords[1] == ball[1]){
        if (verify_move(x, y, 1) && possible_move_ball(x, y)){
            if (check_near(turn % 2, x, y))
            	g_print("No podes dar un pase al rival\n");
            else{
            	move(&ball[0], &ball[1], x - ball[0], y - ball[1]);
            	print_pitch();
            	last_moved_player = -1;
            	passes++;
				if (ball[0] == 13 && (ball[1] == 0 || ball[1] == 10 || (ball[1] > 1 && ball[1] < 9))){
					turn++;
					turn++;
					passes = 0;
				}
				else if (passes == 4 || ! possesion(turn % 2)){
            		turn++;
            		passes = 0;
            	}
            }
        }
    	move(&coords[0], &coords[1], -1 - coords[0], -1 - coords[1]);
    	if (ball[0] == 0) goal_scored(0);
    	if (ball[0] == 15) goal_scored(1);

    	if (bot && turn % 2) move_bot();

    }
    // Si se seleccionó un jugador previamente
    else {
    	if (! passes){
            if(is_box(x, y, turn % 2 + 1) == 3) puts("No podes ir a tu propio corner");
            else if (verify_move(x, y, 0) && possible_move(x, y, team1[click_box][0], team1[click_box][1]) && ! (turn % 2)){
                move(&team1[click_box][0], &team1[click_box][1], x - team1[click_box][0], y - team1[click_box][1]);
                if (! possesion(turn % 2)) turn++;

                print_pitch();
            }
            else if (verify_move(x, y, 0) && possible_move(x, y, team2[click_box - 5][0], team2[click_box - 5][1]) && turn % 2){
                move(&team2[click_box - 5][0], &team2[click_box - 5][1], x - team2[click_box - 5][0], y - team2[click_box - 5][1]);
                if (! possesion(turn % 2)) turn++;
                print_pitch();
            }
            move(&coords[0], &coords[1], -1 - coords[0], -1 - coords[1]);

            if (bot && turn % 2) move_bot();
    	}
    }
}

// Fucnion que imprime el estado del tablero
void print_pitch(){
    gtk_container_foreach(GTK_CONTAINER(grid), (GtkCallback)gtk_widget_destroy, NULL); // Eliminamos los widgets para volver a mostrarlos
    for (int i = 0; i < ROWS; i++)
        for (int j = 0, flag = 0; j < COLS; j++){
        	if (i == 0 && (j == 0 || j == 8)){ // Generar los marcadores
                GtkWidget *overlay = gtk_overlay_new(); // Se crea el overlay que tendrá las imagenes y el boton para cada casilla
                gtk_widget_set_size_request(overlay, 60, 60);
                GtkWidget *label = gtk_label_new("HOLA"); // Se crea un botón
                gchar id[10];

                g_snprintf(id, sizeof(id), "label_%d", j);
                g_object_set_data(G_OBJECT(label), "id", g_strdup(id)); // Se le asigna el ID
                gtk_widget_set_hexpand(label, TRUE);
                gtk_widget_set_vexpand(label, TRUE);

                gtk_overlay_add_overlay(GTK_OVERLAY(overlay), label); // Se añade al overlay
                gtk_grid_attach(GTK_GRID(grid), overlay, j, i, 3, 1); // Se añade al grid
        	}
        	else if (i != 0 || (i == 0 && (j > 2 && j < 8))){ // Generar los botones
                GtkWidget *overlay = gtk_overlay_new(); // Se crea el overlay que tendrá las imagenes y el boton para cada casilla
                flag = check_square(i, j);

                gtk_widget_set_size_request(overlay, 60, 60);

                if (flag != -1){ // Si hay un jugador en la casilla i, j
                    GtkWidget *image = load_scaled_image(flag == -2 ? "ball.png" : flag == 0 ? colors_gk[color_p1] : flag < 5 ? colors[color_p1] : flag == 5 ? colors_gk[color_p2] : colors[color_p2], 60, 60);
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
        }
    gtk_widget_show_all(grid);
}


// Verifica si los campos de entrada de nombre de Jugador 1 está llena
gboolean check_entry1_filled(){
	const gchar *name1 = gtk_entry_get_text(GTK_ENTRY(entry_player1));

	    if (g_strcmp0(name1, "") == 0) {
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(P1_window),
				GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK, "Debe llenar nombre de jugador.");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			return FALSE;
	}
	return TRUE;
}

// Verifica si los campos de entrada de nombre de Jugador 2 está llena
gboolean check_entry2_filled(){
	const gchar *name2 = gtk_entry_get_text(GTK_ENTRY(entry_player2));
	    if (g_strcmp0(name2, "") == 0) {
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(P1_window),
				GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK, "Debe llenar nombre de jugador.");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			return FALSE;
	}
	return TRUE;
}

//Funcion para comparar si los jugadores tienen colores iguales
gboolean check_colors(){
	    if (color_p1 == color_p2) {
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(P1_window),
				GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK, "Debe escoger un color distinto al del Jugador 1");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			return FALSE;
	}
	return TRUE;
}

// Función para manejar la selección del color amarillo para el jugador 1
void on_color_yellow_p1(GtkWidget *widget, gpointer data) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        color_p1 = 0;
    }
}

// Función para manejar la selección del color celeste para el jugador 1
void on_color_green_p1(GtkWidget *widget, gpointer data) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        color_p1 = 1;
    }
}

// Función para manejar la selección del color rojo para el jugador 1
void on_color_red_p1(GtkWidget *widget, gpointer data) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        color_p1 = 2;
    }
}

// Función para manejar la selección del color azul para el jugador 1
void on_color_blue_p1(GtkWidget *widget, gpointer data) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        color_p1 = 3;
    }
}

// Función para manejar la selección del color amarillo para el jugador 2
void on_color_yellow_p2(GtkWidget *widget, gpointer data) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        color_p2 = 0;
    }
}

// Función para manejar la selección del color celeste para el jugador 2
void on_color_green_p2(GtkWidget *widget, gpointer data) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        color_p2 = 1;
    }
}

// Función para manejar la selección del color rojo para el jugador 2
void on_color_red_p2(GtkWidget *widget, gpointer data) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        color_p2 = 2;
    }
}

// Función para manejar la selección del color azul para el jugador 2
void on_color_blue_p2(GtkWidget *widget, gpointer data) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        color_p2 = 3;
    }
}


// 						Funciones para manejar los botones

// Funcion que permite abrir le ventana de Configuracion de juego al presionar el boton siguiente
void on_btn_play_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(main_window);
    gtk_widget_show_all(config_window);
}

// Funcion que permite abrir le ventana "Creditos" de juego al presionar el boton siguiente
void on_btn_about_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(main_window);
    gtk_widget_show_all(about_window);
}

// Funcion para salir del programa
void on_btn_exit_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    GtkWidget *parent_window = GTK_WIDGET(user_data);
    dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "¿Está seguro de que quiere salir del programa?");

    gint response = gtk_dialog_run(GTK_DIALOG(dialog)); // Ejecutar el diálogo y obtener la respuesta del usuario

    if (response == GTK_RESPONSE_YES) {
        gtk_widget_destroy(dialog);
        gtk_main_quit(); // Cerrar el programa si el usuario elige "Sí"
    }
    else
        gtk_widget_destroy(dialog); // Cerrar el diálogo si elige "No"
}

void on_btn_back_about_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(about_window);
    gtk_widget_show_all(main_window);
}

void on_btn_back_config_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(config_window);
    gtk_widget_show_all(main_window);
}

// Funcion para asignar el modo de juego Humano vs Humano
void on_btn_pvp_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(config_window);
    gtk_widget_show_all(P1_window);
}

void on_btn_play_bot_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(config_window);
    gtk_widget_show_all(P1_window);
}

void on_btn_bot_bot_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(config_window);
    gtk_widget_show_all(P1_window);
}

void on_btn_sig_P1_clicked(GtkWidget *widget, gpointer data) {
    if (check_entry1_filled()) {
        gtk_widget_hide(P1_window);
        gtk_widget_show_all(P2_window);
    }
}

void on_btn_back_P1_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(P1_window);
    gtk_widget_show_all(config_window);
}

void on_btn_sig_P2_clicked(GtkWidget *widget, gpointer data) {
	if (check_entry2_filled()) {
		if(check_colors()){
	        gtk_widget_hide(P2_window);
	        gtk_widget_show_all(start_window);
		}
	}
}

void on_btn_back_P2_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(P2_window);
    gtk_widget_show_all(P1_window);

}

void on_btn_sig_start_clicked(GtkWidget *widget, gpointer data){
	gtk_widget_hide(start_window);
	gtk_widget_show_all(board_window);

 }

void on_btn_back_start_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(start_window);
    gtk_widget_show_all(P2_window);
}

void on_btn_start_p1_clicked(GtkToggleButton *toggle_button, gpointer user_data) {
	if (gtk_toggle_button_get_active(toggle_button)) {
	        turn = 0;
	        g_print("\tTUrno es %d", turn);
	}
}

void on_btn_start_p2_clicked(GtkToggleButton *toggle_button, gpointer user_data) {
    if (gtk_toggle_button_get_active(toggle_button)) {
        turn = 1;
        g_print("\tTUrno es %d", turn);
    }
}

void on_btn_start_random_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if (gtk_toggle_button_get_active(togglebutton)) {
        srand(time(NULL)); // Inicializar el generador de números aleatorios
        turn = rand() % 2; // Generar un número aleatorio entre 0 y 1
    }
}

void setup_start_buttons(GtkBuilder *builder) {
    // Obtener los radio buttons usando sus ID en el archivo .glade
    GtkWidget *btn_start_p1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_start_p1"));
    GtkWidget *btn_start_p2 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_start_p2"));
    GtkWidget *btn_start_random = GTK_WIDGET(gtk_builder_get_object(builder, "btn_start_random"));

    // Conectar cada radio button con su respectiva función
    g_signal_connect(btn_start_p1, "toggled", G_CALLBACK(on_btn_start_p1_clicked), NULL);
    g_signal_connect(btn_start_p2, "toggled", G_CALLBACK(on_btn_start_p2_clicked), NULL);
    g_signal_connect(btn_start_random, "toggled", G_CALLBACK(on_btn_start_random_toggled), NULL);
}

 // Función que configura los botones y conecta las señales
 void setup_color_buttons(GtkBuilder *builder) {
     // Obtener los objetos de los radio buttons usando sus ID en el archivo .glade
     GtkWidget *btn_yellow_p1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_yellow_p1"));
     GtkWidget *btn_green_p1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_green_p1"));
     GtkWidget *btn_red_p1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_red_p1"));
     GtkWidget *btn_blue_p1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_blue_p1"));

     GtkWidget *btn_yellow_p2 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_yellow_p2"));
     GtkWidget *btn_green_p2 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_green_p2"));
     GtkWidget *btn_red_p2 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_red_p2"));
     GtkWidget *btn_blue_p2 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_blue_p2"));

     // Conectar cada radio button con su respectiva funcion
     g_signal_connect(btn_yellow_p1, "toggled", G_CALLBACK(on_color_yellow_p1), NULL);
     g_signal_connect(btn_green_p1, "toggled", G_CALLBACK(on_color_green_p1), NULL);
     g_signal_connect(btn_red_p1, "toggled", G_CALLBACK(on_color_red_p1), NULL);
     g_signal_connect(btn_blue_p1, "toggled", G_CALLBACK(on_color_blue_p1), NULL);

     g_signal_connect(btn_yellow_p2, "toggled", G_CALLBACK(on_color_yellow_p2), NULL);
     g_signal_connect(btn_green_p2, "toggled", G_CALLBACK(on_color_green_p2), NULL);
     g_signal_connect(btn_red_p2, "toggled", G_CALLBACK(on_color_red_p2), NULL);
     g_signal_connect(btn_blue_p2, "toggled", G_CALLBACK(on_color_blue_p2), NULL);
 }

void get_callbacks(GtkBuilder *builder){
	 // Obtener las ventanas
	     main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
	     config_window = GTK_WIDGET(gtk_builder_get_object(builder, "config_window"));
	     about_window = GTK_WIDGET(gtk_builder_get_object(builder, "about_window"));
	     P1_window = GTK_WIDGET(gtk_builder_get_object(builder, "P1_window"));

	     P2_window = GTK_WIDGET(gtk_builder_get_object(builder, "P2_window"));
	     start_window = GTK_WIDGET(gtk_builder_get_object(builder, "start_window"));

	     // Obtener los campos de nombre de jugador
	     entry_player1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry_player1"));
	     entry_player2 = GTK_WIDGET(gtk_builder_get_object(builder, "entry_player2"));

	     // Conectar señales de botones manualmente
	     g_signal_connect(gtk_builder_get_object(builder, "btn_play"), "clicked", G_CALLBACK(on_btn_play_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_about"), "clicked", G_CALLBACK(on_btn_about_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_exit"), "clicked", G_CALLBACK(on_btn_exit_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_back_about"), "clicked", G_CALLBACK(on_btn_back_about_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_pvp"), "clicked", G_CALLBACK(on_btn_pvp_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_play_bot"), "clicked", G_CALLBACK(on_btn_play_bot_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_bot_bot"), "clicked", G_CALLBACK(on_btn_bot_bot_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_back_config"), "clicked", G_CALLBACK(on_btn_back_config_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_sig_p1"), "clicked", G_CALLBACK(on_btn_sig_P1_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_back_p1"), "clicked", G_CALLBACK(on_btn_back_P1_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_sig_p2"), "clicked", G_CALLBACK(on_btn_sig_P2_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_back_p2"), "clicked", G_CALLBACK(on_btn_back_P2_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_back_start"), "clicked", G_CALLBACK(on_btn_back_start_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_start_p1"), "clicked", G_CALLBACK(on_btn_start_p1_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_start_p2"), "clicked", G_CALLBACK(on_btn_start_p1_clicked), NULL);
	     g_signal_connect(gtk_builder_get_object(builder, "btn_sig_start"), "clicked", G_CALLBACK(on_btn_sig_start_clicked), NULL);
 }

// Funcion principal
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    board_window =  gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(board_window), "MasterGoal");
        gtk_widget_set_name(board_window, "board_window");
        g_signal_connect(board_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_container_add(GTK_CONTAINER(board_window), vbox);

        GtkWidget *hbox_centered = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_box_set_homogeneous(GTK_BOX(hbox_centered), TRUE);
        gtk_box_pack_start(GTK_BOX(vbox), hbox_centered, TRUE, TRUE, 0);

        grid = gtk_grid_new();
        gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
        gtk_widget_set_vexpand(grid, TRUE);
        gtk_box_pack_start(GTK_BOX(hbox_centered), grid, TRUE, TRUE, 0);

        apply_css_to_window(board_window, "board.css");
        apply_css_to_window(board_window, "styles.css");
        gtk_window_maximize(GTK_WINDOW(board_window));

    // Cargamos el archivo Glade
    GtkBuilder *builder = gtk_builder_new_from_file("TPF.glade");

    get_callbacks(builder);
    setup_color_buttons(builder);
    setup_start_buttons(builder);
    g_object_unref(builder);
    print_pitch();
    gtk_widget_show_all(main_window);
    gtk_main();

    return 0;
}
