#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char pitch[15][11]; // Matriz que representa el campo de juego
int team1[5][2] = {{11, 5}, {10, 3}, {10, 7}, {8, 8}, {8, 2}}; // Matriz de posiciones del Equipo del Jugador 1
int team2[5][2] = {{3, 5}, {4, 3}, {4, 7}, {6, 8}, {6, 2}}; // Matriz de posiciones del Equipo del Jugador 2 / Computadora
int ball[2] = {7, 5};
int moves[9][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}; // Las 8 direcciones que se pueden mover la pelota o un jugador

// Funcion para comprobar si el input es un entero
// Parametros: num es el input, lim_inf es el limite inferior, lim_sup es el limite superior, aux es 1 si num no puede valer 4 el entero y 0 en caso contrario
// Retorno: El numero ingresado y verificado
int verify(int num, int lim_inf, int lim_sup, int aux){
   while (scanf("%d", &num) != 1 || getchar() != '\n'){ // Mientras no sea un entero el input o sea un salto de linea
       while (getchar() != '\n');} // Limpiar el buffer de entrada
    while (num < lim_inf || num > lim_sup){
        if(! aux) printf("Ingrese un numero entre %d y %d:\n", lim_inf, lim_sup);
        scanf("%d", &num);
       while (getchar() != '\n'); // Limpiar el buffer de entrada
    }
    return num;
}

// Función para verificar si una jugada es valida
// Parametros: x e y son las coordenadas de la pelota / el jugador, obj es 0 si es un palabra y 1 si es la pelota
// Retorno: 1 si es una jugada valida, 0 en caso contrario
int verify_move(int x, int y, int obj){
    if(y > 10 || y < 0) return 0;
   for (int i = 0; i < 5; i++) // Verifica si ya hay un jugador en la casilla
        if ((x == team1[i][0] && y == team1[i][1]) || (x == team2[i][0] && y == team2[i][1])) return 0;
   if(! obj){ // Si es un jugador
        if((x == ball[0] && y == ball[1]) || (x < 1 || x > 13)) return 0;
    }
    else if(obj == 1 && (x < 0 || x > 14)) return 0;// Si es la pelota y está fuera de las filas del campo: 0 retorna
   else if(obj == 1 && ((x == 0 || x == 14) && (y < 3 || y > 7))) return 0; // Si es la pelota y está en las ultimas filas pero no dentro de la porteria: 0 retorna
    return 1;
}

// Función para mover ya sea la pelota o un jugador
// Parametros: x es la posicion en x, y es la posicion en y, move_x es la cantidad de casillas a mover en x, move_y es la cantidad de casillas a mover en y
void move(int *x, int *y, int move_x, int move_y){
   *x += move_x;
   *y += move_y;
}

// Función que verifica si una casilla se encuentra en un area o corner
// Parametros: x e y son las coordenadas de la casilla, aux  es 1 para el equipo 1 y 2 para el equipo 2
// Retorno: 1 si la casilla se encuentra en el area chica, 2 si la casilla se encuentra en el area grande, 3 si esta en tu propio corner 0 en casos contrarios
int is_box(int x, int y, int aux){
        if (aux == 1 && (y == 0 || y == 10) && x == 13) return 3;
        else if (aux == 2 && (y == 0 || y == 10) && x == 1) return 3;
        else if ((x == 13 || x == 12 || x == 1 || x == 2) && (y > 1 && y < 8)) return 1;
        else if ((x == 11 || x == 10 || x == 3 || x == 4) && (y > 0 && y < 9)) return 2;
    return 0;
}

// Función para saber si el balon pasa por encima del portero en area grande/ jugador en area chica.
// Parametros: x e y son las coordenadas de la casilla, team es 0 si es para team1, team es 1 para team2, sp es la cantidad de casillas a mover.
// Retorno: 1 si es que la casilla se encuentra arriba de un jugador 2, 0 si no arriba de ningun jugador.
int defend_box(int x, int y, int sp,int team){
        for (int j = 0; j <5; j++){
            for(int i = 1; i<= sp;i++){
                if(team){
                if((is_box(team1[j][0],team1[j][1],2)) == 1){
                   if ((ball[0] + (x * i) == team1[j][0]) && (ball[1] + (y * i) == team1[j][1])){
                        return 1;
                        }
                    }
                if((is_box(team1[0][0],team1[0][1],2)) == 2){
                    if ((ball[0] + (x * i) == team1[0][0]) && (ball[1] + (y * i) == team1[1][1]))
                    return 1;
                }
                }
                else{
                if((is_box(team2[j][0],team2[j][1],1)) == 1){
                   if ((ball[0] + (x * i) == team2[j][0]) && (ball[1] + (y * i) == team2[j][1])){
                        return 1;
                        }
                    }
                if((is_box(team2[0][0],team2[0][1],2)) == 2){
                    if ((ball[0] + (x * i) == team2[0][0]) && (ball[1] + (y * i) == team2[0][1])){
                    return 1;}
                }
            }
        }
        }
    return 0;
}

// Funcion para imprimir el campo de juego
void print_pitch(){
    for(int i = 0; i < 15; i++, puts(""))
        for(int j = 0; j < 11; j++){
            for(int k = 0, flag = 0; k < 5; k++){
               i == ball[0] && j == ball[1] ? printf(" O "), flag = 2: 0; // Imprimir la pelota
                i == team1[k][0] && j == team1[k][1] ? printf("\033[31m %d \033[0m", k + 1), flag = 1 : 0;
                i == team2[k][0] && j == team2[k][1] ? printf("\033[34m %d \033[0m", k + 1), flag = 1 : 0;
                if(k == 4 && ! flag) printf(" %c ", pitch[i][j]);
                if(flag == 2) break;
            }
        }
    puts("");
}

// Función que muestra las posibles posiciones para mover al jugador o la pelota
// Parametros: x e y son las coordenadas de la pelota / el jugador, obj es 0 si es un jugador y 1 si es la pelota
void print_moves(int x, int y, int obj){
    int count = 0, lim_sup = obj ? 5 : 3;
    for(int i = obj ? -4 : -2; i < lim_sup; i++, puts("")){
        for(int j = obj ? -4 : -2; j < lim_sup; j++){
            if ((i > -2 && j > -2) && (i < 2 && j < 2)){
                if(count != 4){
                    printf(" %d ", count++);
                    continue;
                }
                else count++;
            }
            if(i == j || i == 0 || j == 0 || i*(-1) == j){
                if(verify_move(x + i, y + j, obj)) printf(" . ");
                else printf(" - ");
            }
            else printf("   ");
        }
    }
}

// Función para verificar si el equipo en turno tiene la posesión del balón.
// Parametros: team es 0 si es el equipo 1 y 1 si es el equipo 2
// Retorno: 1 si es que se tiene mayoría de jugadores adyacentes a la pelota, 0 en caso contrario
int possesion(int team){
    int c = 0, d = 0;
    for(int k = 0; k < 9; k++)
        for(int i = 0; i < 5; i++){
            if(team1[i][0] == (ball[0] + moves[k][0]) && team1[i][1] == (ball[1] + moves[k][1])) c++;
            if(team2[i][0] == (ball[0] + moves[k][0]) && team2[i][1] == (ball[1] + moves[k][1])) d++;
        }
    if(is_box(team1[0][0], team1[0][1], 1))
        if (ball[0] == team1[0][0] && (ball[1] == team1[0][1] - 1 || ball[1] == team1[0][1] + 1)) c = 100;
    if(is_box(team2[0][0], team2[0][1], 2))
        if(ball[0] == team2[0][0] && (ball[1] == team2[0][1] - 1 || ball[1] == team2[0][1] + 1)) d = 100;
   if(! team && c > d) return 1; // Si es turno del equipo 1 y tienen la posesión
   if(team && d > c) return 1; // Si es turno del equipo 2 y tienen la posesión
    return 0;
}

// Función para verificar si el balon se encuentra en una casilla especial
// Parametros: team es 0 si es el equipo 1 y 1 si es el equipo 2
// Retorno: 1 si es que la pelota se encuentra en una casilla especial, 0 en caso contrario
int specbox(int team){
   if(!team){ // Si es turno del equipo 1 se verifica si la pelota está en un corner del equipo 2
        if(((ball[0] == 1) && ((ball[1] == 0 || ball[1] == 10) || (ball[1] > 2 && ball[1] < 8))))
            return 1;
    }
   else // Si es turno del equipo 2 se verifica si la pelota está en un corner del equipo 1
        if(((ball[0] == 13) && ((ball[1] == 0 || ball[1] == 10) || (ball[1] > 2 && ball[1] < 8))))
            return 1;
    return 0;
}

int check_adj(int team, int chosen_dir, int spaces, int aux){
    int c1 = 0, c2 = 0; // c1 es el contador para jugadores del equipo 1, c2 para los del equipo 2
    for(int i = -1; i < 2; i++)
        for(int j = -1; j < 2; j++)
            for(int k = 0; k < 5; k++)
               if((ball[0] + (moves[chosen_dir][0] * spaces) + i == team1[k][0]) && (ball[1] + (moves[chosen_dir][1] * spaces) + j == team1[k][1]))
                    c1++;
               else if((ball[0] + (moves[chosen_dir][0] * spaces) + i == team2[k][0]) && (ball[1] + (moves[chosen_dir][1] * spaces) + j == team2[k][1]))
                    c2++;
    if(! aux){
        if(! team && c1 > 0) return 1;
        if(team && c2 > 0) return 1;
    }
    else{
        if(!team && c2 > c1) return 1;
        if(team &&  c1 > c2) return 1;
    }
    return 0;
}

// Función que reinicia el campo de juego a su estado inicial
void reset_pitch(){
    int temp_team1[5][2] = {{11, 5}, {10, 3}, {10, 7}, {8, 8}, {8, 2}};
    int temp_team2[5][2] = {{3, 5}, {4, 3}, {4, 7}, {6, 8}, {6, 2}};
   for (int i = 0; i < 5; i++) { // Restaurar la posicion inicial de los jugadores
        move(&team1[i][0], &team1[i][1], temp_team1[i][0] - team1[i][0], temp_team1[i][1] - team1[i][1]);
        move(&team2[i][0], &team2[i][1], temp_team2[i][0] - team2[i][0], temp_team2[i][1] - team2[i][1]);
    }
   ball[0] = 7; // Restaurar la posicion inicial de la pelota en x
   ball[1] = 5; // Restaurar la posicion inicial de la pelota en y
}

int main(){
    // chsn_p guarda el indice del jugador a mover, chsn_d guarda la dirección hacia donde mover la pelota o del jugador, sp es la cantidad de casillas que se mueve la pelota o el jugador
    int chsn_p, chsn_d, turn, sp, passes, goal[2] = {0, 0}, players[2] = {0, 0}, option = 0; // passes es la cantidad de pases en ese turno
    srand(time(0)); // Aseguro que el rand sea siempre diferente

    for(int i = 0; i < 15; i++)
        for (int j = 0; j < 11; j++){
            if ((i != 0 && i != 14) || ((i == 0 || i == 14) && (j > 2 && j < 8))) pitch[i][j] = '.';
            else pitch[i][j] = ' ';
            if((i == 13 || i == 1) && ((j == 0 || j == 10) || (j > 2 && j < 8))) // Casillas Especiales
                pitch[i][j] = '#';
        }
    puts("Bienvenido a MasterGOAL.\nVa a jugar contra otra persona o contra la computadora?\n1 -- Persona\n2 -- Computadora");
    option = verify(option, 1, 2, 0);
    players[1] = option == 1 ? 0 : 1; // El segundo jugador será 0 si es una persona, 1 si es la computadora
    turn = rand() % 2 ? 1 : 0; // Si turn es 0 empieza el jugador 1, si es 1 empieza el jugador 2
    printf("\tEmpieza el Jugador %s\n", turn % 2 ? "\033[34mAzul\033[0m" : "\033[31mRojo\033[0m");

    while(turn < 10 || goal[0] != 2 || goal[1] != 2){
        printf("\nJ1: %d\tJ2: %d\nTurno del Jugador %s\n",goal[0], goal[1], turn % 2 ? "\033[34mAzul\033[0m" : "\033[31mRojo\033[0m");
        if(! players[turn % 2]){ // Si el jugador en turno es una persona
            while(1){
                puts("Que jugador desea mover? (1 - 5)\n");
                print_pitch();
               chsn_p = verify(chsn_p, 1, 5, 0) - 1; // Se ingresa el jugador a mover
                printf("En que direccion quiere mover al jugador %d? (0 - 8)\n\n", chsn_p + 1);
               print_moves(!(turn % 2) ? team1[chsn_p][0] : team2[chsn_p][0], !(turn % 2) ? team1[chsn_p][1] : team2[chsn_p][1], 0); // Se muestran las direcciones
               chsn_d = verify(chsn_d, 0, 8, 1); // Se ingresa la dirección del jugador
                puts("Cuantas casillas quiere mover al jugador en esa direccion? (1 - 2)");
               sp = verify(sp, 1, 2, 0); // Ingresar cuantas casillas va a moverse el jugador
               if(is_box((turn % 2 ? team2[chsn_p][0] : team1[chsn_p][0]) + moves[chsn_d][0] * sp, (turn % 2 ? team2[chsn_p][1] : team1[chsn_p][1]) + moves[chsn_d][1] * sp, turn % 2 + 1) == 3){
                    puts("No podes ir a tu propio corner");
                    continue;
                }
               if(verify_move((turn % 2 ? team2[chsn_p][0] : team1[chsn_p][0]) + moves[chsn_d][0] * sp, (turn % 2 ? team2[chsn_p][1] : team1[chsn_p][1]) + moves[chsn_d][1] * sp, 0))
                    break;
                else puts("\n\tMovimiento invalido");
            }
        move(!(turn % 2) ? &team1[chsn_p][0] : &team2[chsn_p][0], !(turn % 2) ? &team1[chsn_p][1] : &team2[chsn_p][1], moves[chsn_d][0] * sp, moves[chsn_d][1] * sp); // Mover el jugador
        }
        
        else{ // Si el jugador en turno es un bot
            while(1){
                chsn_p = rand() % 5;
                chsn_d = rand() % 9;
                sp = (rand() % 2) + 1;
               if(is_box((turn % 2 ? team2[chsn_p][0] : team1[chsn_p][0]) + moves[chsn_d][0] * sp, (turn % 2 ? team2[chsn_p][1] : team1[chsn_p][1]) + moves[chsn_d][1] * sp, turn % 2 + 1) == 3)
                    continue;
               if(verify_move((turn % 2 ? team2[chsn_p][0] : team1[chsn_p][0]) + moves[chsn_d][0] * sp, (turn % 2 ? team2[chsn_p][1] : team1[chsn_p][1]) + moves[chsn_d][1] * sp, 0))
                    break;
            }
        move(!(turn % 2) ? &team1[chsn_p][0] : &team2[chsn_p][0], !(turn % 2) ? &team1[chsn_p][1] : &team2[chsn_p][1], moves[chsn_d][0] * sp, moves[chsn_d][1] * sp); // Mover el jugador
        }
        print_pitch();
        passes = 0; // Reinicio a 0 la cantidad de pases cada turno

        if(! players[turn % 2]){ // Si el jugador en turno es una persona
            while(possesion(turn % 2) && passes < 4){
                while(1){
                    puts("En que direccion quiere mover a la pelota? (0 - 8)\n");
                   print_moves(ball[0], ball[1], 1); // Se muestran las direcciones
                   chsn_d = verify(chsn_d, 0, 8, 1); // Se ingresa la dirección de la pelota
                    puts("Cuantas casillas quiere mover la pelota en esa direccion? (1 - 4)");
                    sp = verify(sp, 1, 4, 0); // Ingresar cuantas casillas va a moverse la pelota
                    if(check_adj(turn % 2, chsn_d, sp, 1)){
						puts("\n\tMovimiento invalido. No podes dar un pase al rival");
						print_pitch();
						continue;
					}
					if(passes == 3 && check_adj(turn % 2, chsn_d, sp, 0)){
						puts("\n\tMovimiento invalido. No podes dar un 4to pase a un compañero");
						print_pitch();
						continue;
					}
					if(defend_box(moves[chsn_d][0], moves[chsn_d][1], sp, (turn % 2))) {
						puts("\n\tMovimiento inválido. No puedes tirar arriba de este jugador .");
						print_pitch(); // Mostrar el campo después del movimiento inválido
						continue;      // Saltar el turno actual y pasar al siguiente
					}
                    if(verify_move(ball[0] + moves[chsn_d][0] * sp, ball[1] + moves[chsn_d][1] * sp, 1)) break;
                    else puts("\n\tMovimiento invalido");
                    print_pitch();
                }
                    move(&ball[0], &ball[1], moves[chsn_d][0] * sp, moves[chsn_d][1] * sp); // Mover la pelota}
               if(possesion(turn % 2))print_pitch(); passes++; // Si la pelota llega a un jugador de tu equipo, aumenta el contador de pases
               if(ball[0] <= 0 || ball[0] >= 14){ // Si la pelota está en una portería
                    puts("\n\tGolazo!!!!");
                    reset_pitch();
                    (turn % 2) ? goal[1]++: goal[0]++;
                }
                if(specbox(turn % 2)){
                    puts("\n\tTurno Extra por casilla especial");
                    turn++;
                }
            }
        }

       else{ // Si el jugador en turno es un bot
            while(possesion(turn % 2) && passes < 4){
                while(1){
                   chsn_d = rand() % 9; // Se ingresa la dirección de la pelota
                   sp = rand() % 5; // Ingresar cuantas casillas va a moverse la pelota
                    if(passes == 3 && check_adj(turn % 2, chsn_d, sp, 0))
                        continue;
                    if(defend_box(moves[chsn_d][0], moves[chsn_d][1], sp, (turn % 2))) {
						puts("\n\tMovimiento inválido. No puedes tirar por arriba de este jugador.");
						print_pitch(); // Mostrar el campo después del movimiento inválido
						continue;      // Saltar el turno actual y pasar al siguiente
					}
                    if(check_adj(turn % 2, chsn_d, sp, 1))
                        continue;
                   if(verify_move(ball[0] + moves[chsn_d][0] * sp, ball[1] + moves[chsn_d][1] * sp, 1)) break;
                    
                    if(passes == 3 && check_adj(turn % 2, chsn_d, sp, 0))
                        continue;
                }
               move(&ball[0], &ball[1], moves[chsn_d][0] * sp, moves[chsn_d][1] * sp); // Mover la pelota
               if(possesion(turn % 2)) passes++; // Si la pelota llega a un jugador de tu equipo, aumenta el contador de pases
               if(ball[0] == 0 || ball[0] == 14){ // Si la pelota está en una portería
                    puts("\tGolazo!!!!");
                    if(ball[0] == 0) goal[0]++;
                    else goal[1]++;
                    reset_pitch();
                }
                if(specbox(turn % 2)){
                    puts("\n\tTurno Extra por casilla especial");
                    turn++;
                }
            }
        }
        turn++;
    }
    printf("\nEl Jugador %s ha ganado\n", goal[1] == 2 ? "\033[34mAzul\033[0m" : "\033[31mRojo\033[0m");
    return 0;
}