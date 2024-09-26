#include <stdio.h>
#include <stdlib.h>

char pitch[15][11]; // Matriz que representa el campo de juego
int team_1[5][2] = {{11, 5}, {10, 3}, {10, 7}, {8, 8}, {8, 2}}; // Matriz de posiciones del Equipo del Jugador 1
int team_2[5][2] = {{3, 5}, {4, 3}, {4, 7}, {6, 8}, {6, 2}}; // Matriz de posiciones del Equipo del Jugador 2 / Computadora

int verify(int num, int lim_inf, int lim_sup){
	while (scanf("%d", &num) != 1 || getchar() != '\n') // Mientras no sea un entero el input o sea un salto de linea
        while (getchar() != '\n'); // Limpiar el buffer de entrada
	while ((lim_inf == -1 ? 0 : num < lim_inf) || (lim_sup == -1 ? 0 : num > lim_sup)){
		if (lim_inf != -1 && lim_sup == -1) printf("Ingrese un numero a partir de %d:\n", lim_inf);
		if (lim_inf == -1 && lim_sup != -1) printf("Ingrese un numero hasta %d:\n", lim_sup);
		if (lim_inf != -1 && lim_sup != -1) printf("Ingrese un numero entre %d y %d:\n", lim_inf, lim_sup);
        scanf("%d", &num);
        while (getchar() != '\n'); // Limpiar el buffer de entrada
    }
	return num;
}

// Funcion para imprimir el campo de juego
// Parametros: team (0 para mostrar los simbolos, 1 para mostrar los numeros de los jugadores del team 1 y 2 para los del team 2)
void print_pitch(int team, int *ball){
    for(int i = 0; i < 15; i++, puts("")){
        for(int j = 0; j < 11; j++){
            if(i == ball[0] && j == ball[1]) {
                printf(" O ");
                continue;
            }
            for(int k = 0, flag = 0; k < 5; k++){
                if(! team){ // Se imprimen los simbolos para los jugadores de cada equipo
                    i == team_1[k][0] && j == team_1[k][1] ? printf(" X "), flag = 1 : 0;
                    i == team_2[k][0] && j == team_2[k][1] ? printf(" H "), flag = 1 : 0;
                }
                if(team == 1){ // Se imprimen los numeros para los jugadores del jugador 1
                    i == team_1[k][0] && j == team_1[k][1] ? printf(" %d ", k), flag = 1 : 0;
                    i == team_2[k][0] && j == team_2[k][1] ? printf(" H "), flag = 1 : 0;
                }
                else if(team == 2){ // Se imprimen los numeros para los jugadores del jugador 2
                    i == team_2[k][0] && j == team_2[k][1] ? printf(" %d ", k), flag = 1 : 0;
                    i == team_1[k][0] && j == team_1[k][1] ? printf(" X "), flag = 1 : 0;
                }
                k == 4 && !flag ? printf(" %c ", pitch[i][j]) : 0;
            }
        }
    }
}

// Función para verificar si una jugada es valida
int verify_move(int x, int y, int obj){
    if (x > 14 || x < 0 || y > 10 || y < 0) return 0;
    if (obj == 0)
        for (int i = 0; i < 5; i++)
            if((x == team_1[i][0] && y == team_1[i][1]) || (x == team_2[i][0] && y == team_2[i][1])) return 0;
    return 1;
}

//
void print_moves(int chosen_plyr, int **team){
    int moves[9][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    int c = 0;
    for(int i = 0; i < 3; i++, puts(""))
        for(int j = 0; j < 3; j++){
            if(moves[c][0] != 0 && moves[c][1] != 0){
                if (verify_move(moves[c][0] + team[chosen_plyr][0], moves[c][1] + team[chosen_plyr][1]))
                    printf(" - ");
                else
                    printf(" %d ", c++);
            }
        printf("%d", c);
        }
}

//
void move_player(){

}

//
void move_ball(){

}

int main(){
    int c = 0, chosen_plyr, ball[2] = {6,5};
    for(int i = 0; i < 15; i++)
        for (int j = 0; j < 11; j++)
            if((i != 0 && i != 14))
                pitch[i][j] = '.';
            else if((i == 0 || i == 14) && (j > 2 && j < 8))
                pitch[i][j] = '.';
            else
                pitch[i][j] = '|';

    while(c < 2){
        puts("Que jugador desea mover? (0 - 4)");
        print_pitch(1, ball);
        chosen_plyr = verify(chosen_plyr, 0, 4);
        print_moves(chosen_plyr, team_1);
        c++;
    }
    return 0;
}