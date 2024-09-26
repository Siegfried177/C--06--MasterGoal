#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char pitch[15][11]; // Matriz que representa el campo de juego
int team_1[5][2] = {{11, 5}, {10, 3}, {10, 7}, {8, 8}, {8, 2}}; // Matriz de posiciones del Equipo del Jugador 1
int team_2[5][2] = {{3, 5}, {4, 3}, {4, 7}, {6, 8}, {6, 2}}; // Matriz de posiciones del Equipo del Jugador 2 / Computadora
int ball[2] = {7, 5}; // Posición de la pelota

int verify(int num, int lim_inf, int lim_sup){
    while (scanf("%d", &num) != 1 || getchar() != '\n'){ // Mientras no sea un entero el input o sea un salto de linea
        while (getchar() != '\n'); // Limpiar el buffer de entrada
    }
	while (num < lim_inf || num > lim_sup){
		if (lim_inf != -1 && lim_sup != -1) printf("Ingrese un numero entre %d y %d:\n", lim_inf, lim_sup);
        scanf("%d", &num);
        while (getchar() != '\n'); // Limpiar el buffer de entrada
    }
	return num;
}

// Funcion para imprimir el campo de juego
// Parametros: team (0 para mostrar los simbolos, 1 para mostrar los numeros de los jugadores del team 1 y 2 para los del team 2)
void print_pitch(int team){
    for(int i = 0; i < 15; i++, puts("")){
        for(int j = 0; j < 11; j++){
            if(i == ball[0] && j == ball[1]) {
                printf(" O ");
                continue;
            }
            for(int k = 0, flag = 0; k < 5; k++){
                if(team == 2){ // Se imprimen los simbolos para los jugadores de cada equipo. H para los jugadores del equipo 1 y V para los oponentes
                    i == team_1[k][0] && j == team_1[k][1] ? printf(" H "), flag = 1 : 0;
                    i == team_2[k][0] && j == team_2[k][1] ? printf(" V "), flag = 1 : 0;
                }
                if(! team){ // Se imprimen los numeros para los jugadores del jugador 1
                    i == team_1[k][0] && j == team_1[k][1] ? printf(" %d ", k), flag = 1 : 0;
                    i == team_2[k][0] && j == team_2[k][1] ? printf(" V "), flag = 1 : 0;
                }
                else if(team == 1){ // Se imprimen los numeros para los jugadores del jugador 2
                    i == team_1[k][0] && j == team_1[k][1] ? printf(" H "), flag = 1 : 0;
                    i == team_2[k][0] && j == team_2[k][1] ? printf(" %d ", k), flag = 1 : 0;
                }
                k == 4 && !flag ? printf(" %c ", pitch[i][j]) : 0;
            }
        }
    }
    puts("");
}

// Función para verificar si una jugada es valida
int verify_move(int x, int y, int obj){
    if(y > 10 || y < 0) return 0;
    if(obj == 0){ // Si es un jugador
        for (int i = 0; i < 5; i++)
            if((x == team_1[i][0] && y == team_1[i][1]) || (x == team_2[i][0] && y == team_2[i][1]) || x < 1 || x > 13) return 0;
    }
    else // Si es la pelota
        if(x < 0 || x > 14) return 0;
    return 1;
}

// Función que muestra las posibles posiciones a mover al jugador o la pelota
void print_moves(int x, int y, int obj){
    int count = 0;
    if(! obj){ // Si es un jugador
        for(int i = -2; i < 3; i++, puts("")){
            for(int j = -2; j < 3; j++){
                if ((i > -2 && j > -2) && (i < 2 && j < 2)){
                    printf(" %d ", count++);
                    continue;
                }
                if(! (i % 2) && ! (j % 2))
                    if(verify_move(x + i, y + j, 0)) printf(" . ");
                    else printf(" - ");
                else printf("   ");
            }
        }
    }
    else{ // Si es la pelota

    }
}

// Función para mover ya sea la pelota o un jugador
void move(int *x, int *y, int move_x, int move_y){
    *x += move_x;
    *y += move_y;
}

int main(){
    int c = 0, chosen_p, chosen_d, turn, sp; 
    int moves[9][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    srand(time(0));
    for(int i = 0; i < 15; i++)
        for (int j = 0; j < 11; j++)
            if ((i != 0 && i != 14) || ((i == 0 || i == 14) && (j > 2 && j < 8)))
                pitch[i][j] = '.';
            else
                pitch[i][j] = '|';

    turn = rand() % 2 ? 1 : 0; // Si turn es 0 empieza el jugador 1, si es 1 empieza el jugador 2
    printf("Empieza el Jugador %d\n", turn % 2 + 1);
    while(c < 10){
        printf("\n%d ---- %d\n", turn, turn % 2);
        printf("Turno del Jugador %d\n", turn % 2 + 1);
        puts("Que jugador desea mover? (0 - 4)\n");
        print_pitch(turn % 2);
        chosen_p = verify(chosen_p, 0, 4); // Se ingresa el jugador a mover
        
        printf("A que direccion quiere mover al jugador %d? (0 - 8)\n\n", chosen_p);
        print_moves(! (turn % 2) ? team_1[chosen_p][0] : team_2[chosen_p][0], ! (turn % 2) ? team_1[chosen_p][1] : team_2[chosen_p][1], 0); // Se muestran las direcciones
        chosen_d = verify(chosen_d, 0, 8);
        
        puts("Cuantas casillas quiere mover al jugador en esa direccion? (1 - 2)");
        sp = verify(sp, 1, 2);
        move(! (turn % 2) ? &team_1[chosen_p][0] : &team_2[chosen_p][0], ! (turn % 2) ? &team_1[chosen_p][1] : &team_2[chosen_p][1], moves[chosen_d][0] * sp, moves[chosen_d][1] * sp);
        turn++;
        c++;
    }
    return 0;
}