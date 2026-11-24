#include <stdio.h>

#define ROWS 9
#define COLS 12

const char map[ROWS][COLS + 1] = {
    "############",
    "#..........#",
    "#..#.......#",
    "#..........#",
    "#......#...#",
    "#..........#",
    "#...#......#",
    "#.........X#",
    "############"
};

struct Player {
    int row;
    int col;
};

void drawMap(struct Player player) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (row == player.row && col == player.col) {
                putchar('P');
            } else {
                putchar(map[row][col]);
            }
        }
        putchar('\n');
    }
}

void movePlayer(struct Player *player, char command) {
    int row = player->row;
    int col = player->col;

    if (command == 'w') row--;
    if (command == 's') row++;
    if (command == 'a') col--;
    if (command == 'd') col++;

    if (row >= 0 && row < ROWS && col >= 0 && col < COLS &&
        map[row][col] != '#') {
        player->row = row;
        player->col = col;
    }
}

int main(void) {
    struct Player player = {1, 1};
    char input[32];

    while (1) {
        drawMap(player);
        if (map[player.row][player.col] == 'X') {
            puts("You escaped! You win!");
            break;
        }
        printf("Move (w/a/s/d) or quit (q): ");
        if (fgets(input, sizeof(input), stdin) == NULL || input[0] == 'q') {
            break;
        }
        movePlayer(&player, input[0]);
    }
    return 0;
}
