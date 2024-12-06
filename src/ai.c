#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

extern int team1[5][2];
extern int team2[5][2];
extern int ball[2];
extern int turn;

void print_pitch();
void goal_scored(int player);
int verify_move(int x, int y, int obj);
int possible_move_ball(int x, int y);
int possible_move(int x, int y, int ply_x, int ply_y);
void move(int *x, int *y, int move_x, int move_y);
int possesion(int team);

int last_moved_player = -1;
int player_away = -1;
int bot_passes = 0;
int last_ball_pos[2] = {-1, -1};
int last_controller = -1; // El jugador que dio el ultimo pase de los bots

// Funcion para devolver el valor absoluto de un numero
int abs(int num){
	if (num < 0) return num * -1;
	return num;
}

// Funcion para ver si un disparo pasa por los brazos del portero
int is_gk(int x_0, int x_f, int y_0, int y_f){
	int distance_x = x_f - x_0, distance_y = y_f - y_0;
	int slope = distance_y != 0 ? distance_x / distance_y : distance_x;
	for (int i = 1; i <= distance_x; i++){
		if (ball[0] + i == team2[0][0] && (((ball[1] + (i * slope)) == team1[0][1] + 1 || (ball[1] + (i * slope)) == team1[0][1] - 1))){
			return 1;
		}
	}
	return 0;
}

// Funcion para saber cual jugador esta al lado de la pelota
int player_next_to_ball(){
	for (int i = 0; i < 5; i++){
		if (ball[0] - team2[i][0] < 2 && ball[0] - team2[i][0] > -2 ){ // Si la distancia en x es 1 o 0
			if (ball[1] - team2[i][1] < 2 && ball[1] - team2[i][1] > -2 ) // Si la distancia en y es 1 o 0
				return i; // Indice del jugador al lado de la pelota
		}
	}
	return -1;
}

// Funcion para saber si una casilla esta al lado de la pelota
int check_adj(int x, int y){
	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++)
			if ((ball[0] + i == x) && (ball[1] + j == y)) return 1;
	return 0;
}

// Funcion para mover a los jugadores al lado de la pelota o que se acerquen en caso contrario
int pressure(){
	for (int i = 4; i > -1; i--){
		for (int x = -2; x < 3; x++){
			for (int y = -2; y < 3; y++){
				if (x == 0 || y == 0 || x == y || x == - y){
					if (check_adj(team2[i][0] + x, team2[i][1] + y) && ! check_adj(team2[i][0], team2[i][1]) && verify_move(team2[i][0] + x, team2[i][1] + y, 0)){
						move(&team2[i][0], &team2[i][1], x, y);
						print_pitch();
						if (! possesion(turn % 2)) turn++;
						return 1;
					}
				}
			}
		}
	}

	for (int i = 4; i > -1; i--){
		if (i == player_next_to_ball()) continue;
		for (int x = -2; x < 3; x++){
			for (int y = -2; y < 3; y++){
				if ((x == 0 || y == 0 || x == y || x == - y) && (x + y > 3 || x + y < -3)){
					int distance_x = abs(ball[0] - team2[i][0]), distance_y = abs(ball[1] - team2[i][1]);
					if ((distance_x >= abs(distance_x - x)) && (distance_y >= abs(distance_y - y)) && (verify_move(team2[i][0] + x, team2[i][1] + y, 0))){
						move(&team2[i][0], &team2[i][1], x, y);
						print_pitch();
						if (! possesion(turn % 2)) turn++;
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

// Funcion para dar pases o disparar para marcar gol
int pass(){
	for (int j = 3; j < 8; j++){
		if (possible_move_ball(15, j) && ! is_gk(ball[0], 15, ball[1], j)){
			move(&ball[0], &ball[1], 15 - ball[0], j - ball[1]);
			goal_scored(turn % 2);
			turn++;
			return 1;
		}
	}

	int aux = player_next_to_ball();
	for (int i = 4; i > -1; i--){
		if (i == last_controller || i == aux) continue;
		for (int x = 1; x > -2; x--){
			for (int y = 1; y > -2; y--){
				if (possible_move_ball(team2[i][0] + x, team2[i][1] + y) && verify_move(team2[i][0] + x, team2[i][1] + y, 1)){
					if ((x == y || x == -y || x == 0 || y == 0) && (x != 0 || y != 0)){
						if (team2[i][0] >= team2[aux][0]){
							last_controller = aux;
							move(&ball[0], &ball[1], team2[i][0] + x - ball[0], team2[i][1] + y - ball[1]);
							if (ball[0] == 13 && (ball[1] == 0 || ball[1] == 10 || (ball[1] > 1 && ball[1] < 9))){
								turn++;
								turn++;
							}
							bot_passes++;
							if (bot_passes == 4) turn++;
							print_pitch();
							return 1;
						}
					}
				}
			}
		}
	}

	if (bot_passes == 0){
		for (int i = 4; i > -1; i--){
			if (i == aux) continue;
			for (int x = 1; x > -2; x--){
				for (int y = 1; y > -2; y--){
					if (possible_move_ball(team2[i][0] + x, team2[i][1] + y) && verify_move(team2[i][0] + x, team2[i][1] + y, 1)){
						if ((x == y || x == -y || x == 0 || y == 0) && (x != 0 || y != 0)){
							last_controller = aux;
							move(&ball[0], &ball[1], team2[i][0] + x - ball[0], team2[i][1] + y - ball[1]);
							if (ball[0] == 13 && (ball[1] == 0 || ball[1] == 10 || (ball[1] > 1 && ball[1] < 9))){
								turn++;
								turn++;
							}
							bot_passes++;
							if (bot_passes == 4) turn++;
							print_pitch();
							return 1;
						}
					}
				}
			}
		}
	}

	turn++;
	return 0;
}
