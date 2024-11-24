#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern int team2[5][2];
extern int ball[2];
extern int turn;

void print_pitch();
int verify_move(int x, int y, int obj);
int possible_move_ball(int x, int y);
int possible_move(int x, int y, int ply_x, int ply_y);
void move(int *x, int *y, int move_x, int move_y);

int last_moved_player = -1;

int defend(){
	for (int i = 1; i < 3; i++){ // Iterar sobre los defensas
		if (i == last_moved_player){
			printf("YYYYYYYYYYY\n");
			continue;
		}
		int distance_x = ball[0] - team2[i][0], distance_y = ball[1] - team2[i][1];
		if ((distance_x > 2 || distance_x < -2) || (distance_y > 2 || distance_y < -2)){
			printf("WQQQQQQQ\n");
			continue;
		}
		else{
			printf("%d\n", i);
			for (int l = -2; l < 3; l++)
				for (int m = -2; m < 3; m++){
					//printf("%d %d", team2[i][0] + l, team2[i][1] + m);
					if (l == m || l == -m || l == 1 || l == -1 || m == 1 || m == -1){
						int movement_x = team2[i][0] + l - ball[0], movement_y = team2[i][1] + m - ball[1];
						if (movement_x < 2 && movement_y < 2 && movement_x > -2 && movement_y > -2){
							if (possible_move(team2[i][0] + l, team2[i][1] + m, team2[i][0], team2[i][1]) && verify_move(team2[i][0] + l, team2[i][1] + m, 0)){
								move(&team2[i][0], &team2[i][1], l, m);
								last_moved_player = i;
								turn++;
								print_pitch();
								return 0;
							}
						}
					}
				}
		}
	}

	int distance_x = ball[0] - team2[0][0], distance_y = ball[1] - team2[0][1];
	if ((distance_x > 2 || distance_x < -2) || (distance_y > 2 || distance_y < -2)) return 0;
	if (distance_x < 2 && distance_x > -2 && distance_y < 2 && distance_y > -2){
		printf("BBBBBBBBBBBBBBBBBBB\n");
		return 0;
	}
	else
		for (int l = -2; l < 3; l++)
			for (int m = -2; m < 3; m++)
				if (l == m || l == -m || l == 1 || l == -1 || m == 1 || m == -1){
					int movement_x = team2[0][0] + l - ball[0], movement_y = team2[0][1] + m - ball[1];
					if (movement_x < 2 && movement_y < 2 && movement_x > -2 && movement_y > -2){
						if (possible_move(team2[0][0] + l, team2[0][0] + m,team2[0][0], team2[0][1])){
							move(&team2[0][0], &team2[0][1], l, m);
							turn++;
							print_pitch();
							return 0;
						}
					}
				}
}
