extern int team1[5][2];
extern int team2[5][2];
extern int ball[2];
extern int goals[2];

int verify_move(int x, int y, int obj);

void move(int *x, int *y, int move_x, int move_y);

int is_box(int x, int y, int aux);

int defend_box(int x, int y, int sp, int team);

int possesion(int team);

int specbox(int team);

void reset_pitch();
