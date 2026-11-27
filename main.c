#include <stdio.h>

#define ROWS 9
#define COLS 12
#define ENEMY_COUNT 10
#define SAVE_FILE "save.txt"

/* # = wall, . = floor, X = exit. */
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
    int row, col;
    int hp, level, xp;
};

struct Player player = {1, 1, 100, 1, 0};

struct Enemy {
    char symbol;
    const char *name;
    int row, col;
    int hp, attack, defense;
    int defeated;
};

/* Two enemies of each type = 10 battles, enough XP for level 5. */
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
            enemies[i].row == row && enemies[i].col == col) {
            return i;
        }
    }
    return -1;
}

int defeatedCount(void) {
    int count = 0;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (enemies[i].defeated) count++;
    }
    return count;
}

int orcDefeated(void) {
    return enemies[8].defeated || enemies[9].defeated;
}

int questsComplete(void) {
    return defeatedCount() >= 3 && player.level >= 3 && orcDefeated();
}

void drawMap(void) {
    putchar('\n');
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int e = enemyAt(row, col);
            if (row == player.row && col == player.col) putchar('P');
            else if (e != -1) putchar(enemies[e].symbol);
            else putchar(map[row][col]);
        }
        putchar('\n');
    }
    printf("HP: %d | Level: %d/5 | XP: %d\n", player.hp, player.level, player.xp);
    puts("Quests:");
    printf("1. Defeat 3 enemies: %d/3\n",
           defeatedCount() < 3 ? defeatedCount() : 3);
    printf("2. Reach level 3: %s\n", player.level >= 3 ? "DONE" : "TODO");
    printf("3. Defeat an Orc: %s\n", orcDefeated() ? "DONE" : "TODO");
    puts("P=player, R/G/W/S/O=enemies, X=exit, #=wall");
}

void addExperience(void) {
    player.xp += 25;
    if (player.level < 5 && player.xp >= player.level * 50) {
        player.level++;
        player.hp = 100 + (player.level - 1) * 20;
        printf("LEVEL UP! You reached level %d. HP restored!\n", player.level);
    }
}

void fight(struct Enemy *enemy) {
    printf("You meet a %s!\n", enemy->name);
    while (player.hp > 0 && enemy->hp > 0) {
        int attack = 12 + (player.level - 1) * 2;
        int defense = 3 + (player.level - 1);
        int damage = attack - enemy->defense;
        if (damage < 1) damage = 1;
        enemy->hp -= damage;
        printf("You deal %d damage.\n", damage);
        if (enemy->hp <= 0) break;
        damage = enemy->attack - defense;
        if (damage < 1) damage = 1;
        player.hp -= damage;
        printf("%s deals %d damage.\n", enemy->name, damage);
    }
    if (player.hp > 0) {
        enemy->defeated = 1;
        printf("You defeated the %s! +25 XP\n", enemy->name);
        addExperience();
    }
}

void movePlayer(char command) {
    int row = player.row;
    int col = player.col;
    if (command == 'w') row--;
    else if (command == 's') row++;
    else if (command == 'a') col--;
    else if (command == 'd') col++;
    else return;

    if (row < 0 || row >= ROWS || col < 0 || col >= COLS ||
        map[row][col] == '#') {
        puts("There is a wall!");
        return;
    }
    if (map[row][col] == 'X' && !questsComplete()) {
        puts("Finish all 3 quests before escaping!");
        return;
    }

    int e = enemyAt(row, col);
    if (e != -1) fight(&enemies[e]);
    if (player.hp > 0) {
        player.row = row;
        player.col = col;
    }
}

int saveGame(void) {
    FILE *file = fopen(SAVE_FILE, "w");
    if (file == NULL) return 0;

    if (fprintf(file, "%d %d %d %d %d\n",
                player.row, player.col, player.hp,
                player.level, player.xp) < 0) {
        fclose(file);
        return 0;
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (fprintf(file, "%d ", enemies[i].defeated) < 0) {
            fclose(file);
            return 0;
        }
    }
    return fclose(file) == 0;
}

int loadGame(void) {
    FILE *file = fopen(SAVE_FILE, "r");
    if (file == NULL) return 0;

    struct Player loaded;
    int flags[ENEMY_COUNT];
    int ok = fscanf(file, "%d %d %d %d %d",
                    &loaded.row, &loaded.col, &loaded.hp,
                    &loaded.level, &loaded.xp) == 5;

    for (int i = 0; i < ENEMY_COUNT && ok; i++) {
        int numbersRead = fscanf(file, "%d", &flags[i]);
        if (numbersRead != 1) {
            ok = 0;
            break;
        }

        if (flags[i] != 0 && flags[i] != 1) {
            ok = 0;
            break;
        }
    }

    if (fclose(file) != 0) ok = 0;
    if (!ok) return 0;

    /* Keep the level, XP and defeated enemies consistent. */
    int defeated = 0;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        defeated += flags[i];
    }

    int expectedXp = defeated * 25;
    int expectedLevel = expectedXp / 50 + 1;
    if (expectedLevel > 5) expectedLevel = 5;
    if (loaded.xp != expectedXp || loaded.level != expectedLevel) return 0;

    player = loaded;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        enemies[i].defeated = flags[i];
    }

    return 1;
}

int main(void) {
    char input[32];
    puts("DUNGEON ESCAPE");
    puts("Move with w/a/s/d + Enter. v=save, l=load, q=quit.");

    while (player.hp > 0) {
        drawMap();
        printf("Command: ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        char command = input[0];
        if (command == 'q') break;
        if (command == 'v') puts(saveGame() ? "Saved!" : "Save failed!");
        else if (command == 'l') puts(loadGame() ? "Loaded!" : "Load failed!");
        else movePlayer(command);

        if (map[player.row][player.col] == 'X') {
            drawMap();
            puts("YOU WIN! Dungeon escaped!");
            return 0;
        }
    }
    if (player.hp <= 0) puts("GAME OVER!");
    return 0;
}
