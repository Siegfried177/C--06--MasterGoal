#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char pitch[15][11]; // Matriz que representa el campo de juego
int team1[5][2] = {{11, 5}, {10, 3}, {10, 7}, {8, 8}, {8, 2}}; // Matriz de posiciones del Equipo del Jugador 1
int team2[5][2] = {{3, 5}, {4, 3}, {4, 7}, {6, 8}, {6, 2}}; // Matriz de posiciones del Equipo del Jugador 2 / Computadora
int ball[2] = {7, 5}; // Posición de la pelota
int moves[9][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}; // Las 8 direcciones que se pueden mover la pelota o un jugador
int corn2[2][2] = {{13,0},{13,10}}; // Corners contrarios del Equipo1
int corn1[2][2] = {{1,0},{1,10}};// Corners contrarios del Equipo 2

// Funcion para comprobar si el input es un entero
// Parametros: num es el input, lim_inf es el limite inferior, lim_sup es el limite superior, aux es 1 si num no puede valer 4 el entero y 0 en caso contrario
int verify(int num, int lim_inf, int lim_sup, int aux){
    while (scanf("%d", &num) != 1 || getchar() != '\n'){ // Mientras no sea un entero el input o sea un salto de linea
        while (getchar() != '\n'); // Limpiar el buffer de entrada
    }
	while (num < lim_inf || num > lim_sup || aux ? num == 4 : 0){
        if(! aux) printf("Ingrese un numero entre %d y %d:\n", lim_inf, lim_sup);
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
                    i == team1[k][0] && j == team1[k][1] ? printf(" H "), flag = 1 : 0;
                    i == team2[k][0] && j == team2[k][1] ? printf(" V "), flag = 1 : 0;
                }
                else if(! team){ // Se imprimen los numeros para los jugadores del jugador 1
                    i == team1[k][0] && j == team1[k][1] ? printf(" %d ", k + 1), flag = 1 : 0;
                    i == team2[k][0] && j == team2[k][1] ? printf(" V "), flag = 1 : 0;
                }
                else if(team == 1){ // Se imprimen los numeros para los jugadores del jugador 2
                    i == team1[k][0] && j == team1[k][1] ? printf(" H "), flag = 1 : 0;
                    i == team2[k][0] && j == team2[k][1] ? printf(" %d ", k + 1), flag = 1 : 0;
                }
                k == 4 && ! flag ? printf(" %c ", pitch[i][j]) : 0;
            }
        }
    }
    puts("");
}

// Función para verificar si una jugada es valida
// Parametros: x e y son las coordenadas de la pelota / el jugador, obj es 0 si es un palabra y 1 si es la pelota
// Retorno: 1 si es una jugada valida, 0 en caso contrario
int verify_move(int x, int y, int obj){
    if(y > 10 || y < 0) return 0;
    if(obj == 0){ // Si es un jugador
        for (int i = 0; i < 5; i++)
            if((x == team1[i][0] && y == team1[i][1]) || (x == team2[i][0] && y == team2[i][1]) || x < 1 || x > 13) return 0;
    }
    else // Si es la pelota
        if(x < 0 || x > 14) return 0;
    return 1;
}

// Función que muestra las posibles posiciones para mover al jugador o la pelota
// Parametros: x e y son las coordenadas de la pelota / el jugador, obj es 0 si es un jugador y 1 si es la pelota
void print_moves(int x, int y, int obj){
    int count = 0, lim_sup = obj ? 5 : 3;
    for(int i = obj ? -4 : -2; i < lim_sup; i++, puts("")){
        for(int j = obj ? -4 : -2; j < lim_sup; j++){
            if ((i > -2 && j > -2) && (i < 2 && j < 2)){
                printf(" %d ", count++);
                continue;
            }
            if(i == j || i == 0 || j == 0 || i*(-1) == j)
                if(verify_move(x + i, y + j, 0)) printf(" . ");
                else printf(" - ");
            else printf("   ");
        }
    }
}

// Función para verificar si el equipo en turno tiene la posesión del balón
// Parametros: team es 0 si es el equipo 1 y 1 si es el equipo 2, x e y son las coordenadas de la pelota
// Retorno: 1 si es que se tiene mayoría de jugadores adyacentes a la pelota, 0 en caso contrario
int possesion(int team, int x, int y){
    int c = 0, d = 0;
    for(int k = 0; k < 9; k++)
        for(int i = 0; i < 5; i++){
            if(team1[i][0] == (x + moves[k][0]) && team1[i][1] == (y + moves[k][1])) c++;
            if(team2[i][0] == (x + moves[k][0]) && team2[i][1] == (y + moves[k][1])) d++;
        }
    if(! team && c > d) return 1;
    if(team && d > c) return 1;
    return 0;
}

// Función para mover ya sea la pelota o un jugador
// Parametros: x es la posicion en x, y es la posicion en y, move_x es la cantidad de casillas a mover en x, move_y es la cantidad de casillas a mover en y
void move(int *x, int *y, int move_x, int move_y){
    *x += move_x;
    *y += move_y;
}
//Funciòn para verfificar si el balǹ se encuentra en una casilla especial
int specbox(int team){
   if(!team){ // Si el equipo es 0 (equipo 1)
       // Verifica si la pelota está en una de las esquinas del equipo 2 (corn2)
       if(((ball[0] == corn1[0][0] || ball[0] == corn1[1][0]) && (ball[1] == corn1[0][1] || ball[1] == corn1[1][1])) ||
          ((ball[0] == 1) && (ball[1] > 2 && ball[1] < 8))){
           return 1;
       }
   } else { // Si el equipo es 1 (equipo 2)
       // Verifica si la pelota está en una de las esquinas del equipo 1 (corn1)
       if(((ball[0] == corn2[0][0] || ball[0] == corn2[1][0]) && (ball[1] == corn2[0][1] || ball[1] == corn2[1][1])) ||
          ((ball[0] == 13) && (ball[1] > 2 && ball[1] < 8))){
           return 1;
       }
   }
   return 0;
}
//Función que reinicia el campo de juego a su estado inicial
void reset_pitch(){
	ball[0]= 7;
	ball[1] = 5;
	int temp_team1[5][2] = {{11, 5}, {10, 3}, {10, 7}, {8, 8}, {8, 2}};
	    for (int i = 0; i < 5; i++) {
	        team1[i][0] = temp_team1[i][0];
	        team1[i][1] = temp_team1[i][1];
	    }
	    // Restaurar las posiciones iniciales de los jugadores del equipo 2
	    int temp_team2[5][2] = {{3, 5}, {4, 3}, {4, 7}, {6, 8}, {6, 2}};
	    for (int i = 0; i < 5; i++) {
	        team2[i][0] = temp_team2[i][0];
	        team2[i][1] = temp_team2[i][1];
	    }
}

int main(){
    // chosen_p guarda el indice del jugador a mover, chosen_d guarda la dirección hacia donde mover la pelota o del jugador, sp es la cantidad de casillas que se mueve la pelota o el jugador
    int chosen_p, chosen_d, turn, sp, passes,goal; // passes es la cantidad de pases en ese turno
    goal = 0;
    turn = 3;
    srand(time(0)); // Aseguro que el rand sea siempre diferente
     for(int i = 0; i < 15; i++)
       for (int j = 0; j < 11; j++){
           if ((i != 0 && i != 14) || ((i == 0 || i == 14) && (j > 2 && j < 8)))
               pitch[i][j] = '.';
           else
               pitch[i][j] = ' ';
           if((i == 13 || i == 1) && (j == 0 || j == 10))
               pitch[i][j] = '#';
            if((i== 13 || i == 1) && (j>2 && j<8))
                pitch[i][j] = '#';
        }

    turn = rand() % 2 ? 1 : 0; // Si turn es 0 empieza el jugador 1, si es 1 empieza el jugador 2
    printf("Empieza el Jugador %d\n\n", turn % 2 + 1);
    while(turn < 10 || goal <2){
        printf("Turno del Jugador %d\n", turn % 2 + 1);

        puts("Que jugador desea mover? (1 - 5)\n");
        print_pitch(turn % 2);
        chosen_p = verify(chosen_p, 1, 5, 0) - 1; // Se ingresa el jugador a mover
        printf("En que direccion quiere mover al jugador %d? (0 - 8)\n\n", chosen_p + 1);
        print_moves(! (turn % 2) ? team1[chosen_p][0] : team2[chosen_p][0], ! (turn % 2) ? team1[chosen_p][1] : team2[chosen_p][1], 0); // Se muestran las direcciones
        chosen_d = verify(chosen_d, 0, 8, 1); // Se ingresa la dirección del jugador
        puts("Cuantas casillas quiere mover al jugador en esa direccion? (1 - 2)");
        sp = verify(sp, 1, 2, 0); // Ingresar cuantas casillas va a moverse el jugador
        move(!(turn % 2) ? &team1[chosen_p][0] : &team2[chosen_p][0], !(turn % 2) ? &team1[chosen_p][1] : &team2[chosen_p][1], moves[chosen_d][0] * sp, moves[chosen_d][1] * sp); // Mover el jugador
        print_pitch(2);

        passes = 0; // Reinicio a 0 la cantidad de pases cada turno
        while(possesion(turn % 2, ball[0], ball[1]) && passes < 4){
            printf("En que direccion quiere mover a la pelota %d? (0 - 8)\n\n", chosen_p);
            print_moves(ball[0], ball[1], 1); // Se muestran las direcciones
            chosen_d = verify(chosen_d, 0, 8, 1); // Se ingresa la dirección de la pelota
            puts("Cuantas casillas quiere mover la pelota en esa direccion? (1 - 4)");
            sp = verify(sp, 1, 4, 0); // Ingresar cuantas casillas va a moverse la pelota
            move(&ball[0], &ball[1], moves[chosen_d][0] * sp, moves[chosen_d][1] * sp); // Mover la pelota
            if(possesion(turn % 2, ball[0], ball[1])) passes++; // Si la pelota llega a un jugador de tu equipo, aumenta el contador de pases
            if(ball[0] <= 0 || ball[0] >= 14){ // Si la pelota está en una portería
               puts("\nGolazo!!!!");
               reset_pitch();
               goal++;
           }
           if(specbox(turn % 2)){turn++;}
           print_pitch(2);
        }
    

        turn++;
    }
    return 0;
}