struct ship_node{
    int size; 
    int lives; 
    char direction;
    int row; 
    int column; 
};

char **init_board();
void free_board(char **board);

int generate_board(char *player, char **board, struct ship_node *ships);
int check_board(int r, int r_inc, int c, int c_inc, char **board);
void new_ship(struct ship_node *ships, char *line, int i);

int launch_missile(char *coord, char **board, char **sboard, struct ship_node *ships);
void check_downed_ship(int row, int column, int i, char **board, char **sboard, struct ship_node *ships);
int check_win(char **board, struct ship_node *ships);

void display_board(char **board);

char *two_to_one(char **board);
void one_to_two(char *arr, char **board);
void end_prog(char *arr, char **board, char **board1, char **board2);
