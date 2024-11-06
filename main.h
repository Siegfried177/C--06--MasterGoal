int verify(int num, int lim_inf, int lim_sup, int aux);

int verify_move(int x, int y, int obj);

void move(int *x, int *y, int move_x, int move_y);

int is_box(int x, int y, int aux);

int defend_box(int x, int y, int sp, int team);

int possesion(int team);

int specbox(int team);

int check_adj(int team, int chosen_dir, int spaces, int aux);

void reset_pitch();