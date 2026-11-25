#include <stdio.h>

#define ROWS 9
#define COLS 12
#define ENEMY_COUNT 10

const char map[ROWS][COLS + 1] = {
    "############", "#..........#", "#..#.......#",
    "#..........#", "#......#...#", "#..........#",
    "#...#......#", "#.........X#", "############"
};

struct Player {
    int row, col, hp, attack, defense;
};

struct Enemy {
    char symbol;
    char *name;
    int row, col, hp, attack, defense;
    int defeated;
};

struct Enemy enemies[ENEMY_COUNT] = {
    {'R', "Rat",      1, 3, 12,  5, 0, 0},
    {'R', "Rat",      2, 5, 12,  5, 0, 0},
    {'G', "Goblin",   1, 7, 20,  7, 1, 0},
    {'G', "Goblin",   3, 3, 20,  7, 1, 0},
    {'W', "Wolf",     3, 8, 25,  8, 2, 0},
    {'W', "Wolf",     5, 2, 25,  8, 2, 0},
    {'S', "Skeleton", 4, 5, 30,  9, 3, 0},
    {'S', "Skeleton", 6, 7, 30,  9, 3, 0},
    {'O', "Orc",      5, 9, 35, 11, 4, 0},
    {'O', "Orc",      7, 8, 35, 11, 4, 0}
};

int enemyAt(int row, int col) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (!enemies[i].defeated &&
            enemies[i].row == row && enemies[i].col == col) return i;
    }
    return -1;
}

void drawMap(struct Player player) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int enemy = enemyAt(row, col);
            if (row == player.row && col == player.col) putchar('P');
            else if (enemy != -1) putchar(enemies[enemy].symbol);
            else putchar(map[row][col]);
        }
        putchar('\n');
    }
}

void fight(struct Player *player, struct Enemy *enemy) {
    printf("Fight: %s!\n", enemy->name);
    while (player->hp > 0 && enemy->hp > 0) {
        int damage = player->attack - enemy->defense;
        if (damage < 1) damage = 1;
        enemy->hp -= damage;
        printf("You hit for %d damage.\n", damage);
        if (enemy->hp <= 0) break;

        damage = enemy->attack - player->defense;
        if (damage < 1) damage = 1;
        player->hp -= damage;
        printf("%s hits you for %d damage.\n", enemy->name, damage);
    }
    if (player->hp > 0) {
        enemy->defeated = 1;
        printf("You defeated the %s!\n", enemy->name);
    }
}

void movePlayer(struct Player *player, char command) {
    int row = player->row;
    int col = player->col;
    if (command == 'w') row--;
    if (command == 's') row++;
    if (command == 'a') col--;
    if (command == 'd') col++;
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS ||
        map[row][col] == '#') return;

    int enemy = enemyAt(row, col);
    if (enemy != -1) fight(player, &enemies[enemy]);
    if (player->hp > 0) {
        player->row = row;
        player->col = col;
    }
}

int main(void) {
    struct Player player = {1, 1, 100, 12, 3};
    char input[32];
    while (player.hp > 0) {
        drawMap(player);
        printf("HP: %d | Move (w/a/s/d) or quit (q): ", player.hp);
        if (!fgets(input, sizeof(input), stdin) || input[0] == 'q') break;
        movePlayer(&player, input[0]);
        if (map[player.row][player.col] == 'X') {
            puts("You escaped! You win!");
            return 0;
        }
    }
    if (player.hp <= 0) puts("Game over!");
    return 0;
}
