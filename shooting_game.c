#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

typedef struct _Element {
    char** shape;
    int width;
    int height;
    int x;
    int y;
} Element;

typedef struct _AttackElement {
    Element* element;
    int move_time;
} AttackElement;

#pragma pack(push, 1)
typedef struct _RankingFile {
    int num;
    char names[10][10];
    int scores[10];
} RankingFile;
#pragma pack(pop)

const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;

const char* SPACE_SHIP_SHAPE[] = {
    "  |  ",
    "/---\\" 
};
const char* METEOR_SHAPE[] = { 
    "/|||\\",
    "\\___/"
};
const char* BULLET_SHAPE[] = { "*" };
const char* GAME_OVER_SHAPE[] = {
    "\x1b[34m   ____    _    __  __ _____    _____     _______ ____  ",
    "       / ___|  / \\  |  \\/  | ____|  / _ \\ \\   / / ____|  _ \\ ",
    "      | |  _  / _ \\ | |\\/| |  _|   | | | \\ \\ / /|  _| | |_) |",
    "      | |_| |/ ___ \\| |  | | |___  | |_| |\\ V / | |___|  _ < ",
    "       \\____/_/   \\_\\_|  |_|_____|  \\___/  \\_/  |_____|_| \\_\\",
    "\x1b[33m                      PRESS ANY KEY TO CONTINUE\x1b[0m"
};
const char* TITLE_LOGO_SHAPE[] = {
    " ________  ___  ___  ________  ________  _________  ___  ________   ________     ",
    "|\\   ____\\|\\  \\|\\  \\|\\   __  \\|\\   __  \\|\\___   ___\\\\  \\|\\   ___  \\|\\   ____\\    ",
    "\\ \\  \\___|\\ \\  \\\\\\  \\ \\  \\|\\  \\ \\  \\|\\  \\|___ \\  \\_\\ \\  \\ \\  \\\\ \\  \\ \\  \\___|    ",
    " \\ \\_____  \\ \\   __  \\ \\  \\\\\\  \\ \\  \\\\\\  \\   \\ \\  \\ \\ \\  \\ \\  \\\\ \\  \\ \\  \\  ___  ",
    "  \\|____|\\  \\ \\  \\ \\  \\ \\  \\\\\\  \\ \\  \\\\\\  \\   \\ \\  \\ \\ \\  \\ \\  \\\\ \\  \\ \\  \\|\\  \\ ",
    "    ____\\_\\  \\ \\__\\ \\__\\ \\_______\\ \\_______\\   \\ \\__\\ \\ \\__\\ \\__\\\\ \\__\\ \\_______\\",
    "   |\\_________\\|__|\\|__|\\|_______|\\|_______|    \\|__|  \\|__|\\|__| \\|__|\\|_______|",
    "   \\|_________|                                                                  ",
    "                   ________  ________  _____ ______   _______                    ",
    "                  |\\   ____\\|\\   __  \\|\\   _ \\  _   \\|\\  ___ \\                   ",
    "                  \\ \\  \\___|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\   __/|                  ",
    "                   \\ \\  \\  __\\ \\   __  \\ \\  \\\\|__| \\  \\ \\  \\_|/__                ",
    "                    \\ \\  \\|\\  \\ \\  \\ \\  \\ \\  \\    \\ \\  \\ \\  \\_|\\ \\               ",
    "                     \\ \\_______\\ \\__\\ \\__\\ \\__\\    \\ \\__\\ \\_______\\              ",
    "                      \\|_______|\\|__|\\|__|\\|__|     \\|__|\\|_______|              ",
    "              1: View Ranking | 2: Change Name | Others: Play Game               "
};

const int SPACE_SHIP_WIDTH = 5;
const int SPACE_SHIP_HEIGHT = 2;

const int METEOR_WIDTH = 5;
const int METEOR_HEIGHT = 2;

const int BULLET_WIDTH = 1;
const int BULLET_HEIGHT = 1;

const int GAME_OVER_WIDTH = 61;
const int GAME_OVER_HEIGHT = 6;

const int TITLE_LOGO_WIDTH = 81;
const int TITLE_LOGO_HEIGHT = 16;

void gotoxy(int x, int y)
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int random_range(int start, int end)
{
    return rand() % (end + 1 - start) + start;
}

void pull_attack_element_list(AttackElement** list, int length, int idx)
{
    AttackElement* temp = list[idx];
    for (int i = idx + 1; i < length; i++)
    {
        list[i - 1] = list[i];
    }
    list[length - 1] = temp;
}

char** create_screen(int width, int height)
{
    char** screen = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++)
    {
        screen[i] = (char*)malloc(width * sizeof(char));
    }
    for (int i = 0; i < height - 1; i++)
    {
        for (int j = 0; j < width - 1; j++)
        {
            screen[i][j] = ' ';
        }
    }
    return screen;
}

AttackElement** create_attack_element_list(int length)
{
    AttackElement** elements = (AttackElement**)malloc(length * sizeof(AttackElement*));
    for (int i = 0; i < length; i++)
    {
        elements[i] = (AttackElement*)malloc(sizeof(AttackElement));
    }
    return elements;
}

void free_attack_element_list(AttackElement** elements, int length, int num)
{
    for (int i = 0; i < length; i++)
    {
        if (i < num) free(elements[i]->element);
        free(elements[i]);
    }
    free(elements);
}

Element* create_element(char** shape, int width, int height, int x, int y)
{
    Element* element = (Element*)malloc(sizeof(Element));
    element->shape = shape;
    element->width = width;
    element->height = height;
    element->x = x;
    element->y = y;
    return element;
}

void write_element(char** screen, Element* element)
{
    int x = element->x;
    int y = element->y;
    for (int i = 0; i < element->height; i++)
    {
        for (int j = 0; j < element->width; j++)
        {
            screen[i + y][j + x] = (element->shape)[i][j];
        }
    }
}

void remove_element(char** screen, Element* element)
{
    int x = element->x;
    int y = element->y;
    for (int i = 0; i < element->height; i++)
    {
        for (int j = 0; j < element->width; j++)
        {
            screen[i + y][j + x] = ' ';
        }
    }
}

Element* load_space_ship(char** screen)
{
    int x = SCREEN_WIDTH / 2;
    int y = SCREEN_HEIGHT * 0.9;
    Element* spaceship = create_element(SPACE_SHIP_SHAPE, SPACE_SHIP_WIDTH, SPACE_SHIP_HEIGHT, x - 1, y);
    write_element(screen, spaceship);
    return spaceship;
}

bool check_overflow(Element* element, int move_x, int move_y)
{
    int width = element->width;
    int height = element->height;
    int x = element->x + move_x;
    int y = element->y + move_y;
    if ((x < 0 || x + width >= SCREEN_WIDTH) || (y < 0 || y + height >= SCREEN_HEIGHT))
    {
        return false;
    }
    return true;
}

bool check_meteor_collision(AttackElement** meteors, int current_meteors, int start_x)
{
    for (int i = 0; i < current_meteors; i++)
    {
        int x = meteors[i]->element->x;
        if ((x <= start_x && start_x <= x + (METEOR_WIDTH - 1)) || (x <= start_x + (METEOR_WIDTH - 1) && start_x + (METEOR_WIDTH - 1) <= x + (METEOR_WIDTH - 1)))
        {
            return false;
        }
    }
    return true;
}

void create_meteors(char** screen, AttackElement** meteors, int* current_meteors)
{
    while (*current_meteors < 7)
    {
        int start_x = random_range(0, SCREEN_WIDTH - (METEOR_WIDTH + 1));
        while (!check_meteor_collision(meteors, *current_meteors, start_x))
        {
            start_x = random_range(0, SCREEN_WIDTH - (METEOR_WIDTH + 1));
        }
        Element* element = create_element(METEOR_SHAPE, METEOR_WIDTH, METEOR_HEIGHT, start_x, 0);
        meteors[*current_meteors]->element = element;
        meteors[*current_meteors]->move_time = GetTickCount64();
        write_element(screen, element);
        (*current_meteors)++;
    }
}

void move_element(char** screen, Element* element, int key)
{
    int x = 0, y = 0;
    switch (key)
    {
    case 1: // left
        x = -1; break;
    case 2: // right
        x = 1; break;
    case 3: //up
        y = -1; break;
    case 4: // down
        y = 1; break;
    }
    remove_element(screen, element);
    if (!check_overflow(element, x, y)) return;
    element->x += x; element->y += y;
    write_element(screen, element);
}

void drop_meteors(char** screen, AttackElement** meteors, int* current_meteors, int* score)
{
    for (int i = 0; i < *current_meteors; i++)
    {
        int current_time = GetTickCount64();
        if (current_time - meteors[i]->move_time >= 500)
        {
            if (meteors[i]->element->y == (int)SCREEN_HEIGHT * 0.9) //game over
            {
                remove_element(screen, meteors[i]->element);
                free(meteors[i]->element);
                pull_attack_element_list(meteors, *current_meteors, i);
                (*current_meteors)--;
                (*score)--;
            }
            move_element(screen, meteors[i]->element, 4);
            meteors[i]->move_time = current_time;
        }
    }
}

void shoot(char** screen, AttackElement** bullets, Element* space_ship, int *current_bullets)
{
    Element* element = create_element(BULLET_SHAPE, BULLET_WIDTH, BULLET_HEIGHT, space_ship->x + (SPACE_SHIP_WIDTH - 1) /2, space_ship->y - 1);
    bullets[*current_bullets]->element = element;
    bullets[*current_bullets]->move_time = GetTickCount64();
    write_element(screen, element);
    (*current_bullets)++;
}

void lift_bullets(char** screen, AttackElement** bullets, int* current_bullets)
{
    for (int i = 0; i < *current_bullets; i++)
    {
        
        int current_time = GetTickCount64();
        if (current_time - bullets[i]->move_time >= 15)
        {
            if (bullets[i]->element->y == 0)
            {
                remove_element(screen, bullets[i]->element);
                free(bullets[i]->element);
                pull_attack_element_list(bullets, *current_bullets, i);
                (*current_bullets)--;
            }
            move_element(screen, bullets[i]->element, 3);
            bullets[i]->move_time = current_time;
        }
    }
}

void check_meteor_bullet_collid(char** screen, AttackElement** meteors, AttackElement** bullets, int *current_meteors, int *current_bullets, int *score)
{
    for (int i = 0; i < *current_meteors; i++)
    {
        AttackElement* meteor = meteors[i];
        int meteor_x = meteor->element->x;
        int meteor_y = meteor->element->y;
        for (int j = 0; j < *current_bullets; j++)
        {
            AttackElement* bullet = bullets[j];
            int bullet_x = bullet->element->x;
            int bullet_y = bullet->element->y;
            if (meteor_y == bullet_y && (meteor_x <= bullet_x && bullet_x <= meteor_x + (METEOR_WIDTH - 1)))
            {
                remove_element(screen, meteor->element);
                remove_element(screen, bullet->element);
                free(bullet->element);
                free(meteor->element);
                pull_attack_element_list(meteors, *current_meteors, i);
                pull_attack_element_list(bullets, *current_bullets, j);
                (*current_meteors)--;
                (*current_bullets)--;
                (*score)++;
                break;
            }
        }
    }
}

Element* show_banner(char** screen, char** shape, int width, int height)
{
    Element* element = create_element(shape, width, height, (SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
    write_element(screen, element);
    return element;
}

void printxy(char** screen, char* string, int x, int y)
{
    int idx = 0;
    while (string[idx] != NULL)
    {
        screen[y][x + idx] = string[idx];
        idx++;
    }
}

void show_score(char** screen, int score)
{
    char score_string[20];
    sprintf(score_string, "SCORE: %d", score);
    printxy(screen, score_string, 0, 1);
}

void show_life(char** screen, int life)
{
    char life_string[10];
    sprintf(life_string, "LIFE: %d", life);
    printxy(screen, life_string, 0, 2);
}

void show_name(char** screen, char* name)
{
    char name_string[20];
    sprintf(name_string, "NAME: %s", name);
    printxy(screen, name_string, 0, 0);
}

int get_key_input() 
{
    if (_kbhit())
    {
        fflush(stdin);
        int key = _getch();
        if (key == 224)
        {
            key = _getch();
            switch (key)
            {
            case 75: return 1; //left
            case 77: return 2; //right
            /*
            case 72: return 3; //up
            case 80: return 4; //down
            */
            }
        }
        else if (key == 32) return 5; //space bar
        else if (key == 113) return 6; //q key
    }
    return -1;
}

void update_screen(char** screen, char* buffer)
{
    gotoxy(0, 0);
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            buffer[i * SCREEN_WIDTH + j + 1] = screen[i][j];
        }
        buffer[i * SCREEN_WIDTH] = '\n';
    }
    buffer[SCREEN_WIDTH * SCREEN_HEIGHT] = '\0';
    printf("%s", buffer + 1);
}

void reset_screen(char** screen)
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            screen[i][j] = ' ';
        }
    }
}

void set_name(char** screen, char* buffer, char* name)
{
    printxy(screen, "SET YOUR NAME: ", (SCREEN_WIDTH - 25) / 2, SCREEN_HEIGHT * 0.3);
    update_screen(screen, buffer);
    gotoxy((SCREEN_WIDTH - 25) / 2 + 15, SCREEN_HEIGHT * 0.3);
    scanf("%s", name);
}

bool file_exists(char* file_name) 
{
    struct stat buffer;
    return stat(file_name, &buffer) == 0;
}

void save_ranking_file(char* file_name, RankingFile* ranking_file)
{
    FILE* fp = fopen(file_name, "w");
    fwrite(ranking_file, sizeof(RankingFile), 1, fp);
    fclose(fp);
}

RankingFile* open_ranking_file(char* file_name)
{
    RankingFile* ranking_file = (RankingFile*)malloc(sizeof(RankingFile));
    memset(ranking_file, 0, sizeof(RankingFile));
    FILE* fp = fopen(file_name, "r");
    fread(ranking_file, sizeof(RankingFile), 1, fp);
    fclose(fp);
    return ranking_file;
}

void show_ranking(char** screen, RankingFile* ranking_file)
{
    printxy(screen, "RANKING", (SCREEN_WIDTH - 7) / 2, 3);
    char ranking_string[30];
    int y = 5;
    if (ranking_file->num <= 0)
    {
        printxy(screen, "NO RANKING EXISTS", (SCREEN_WIDTH - 17) / 2, 5);
        return;
    }
    for (int i = 0; i < ranking_file->num; i++)
    {
        sprintf(ranking_string, "%d: %s - %d", i + 1, ranking_file->names[i], ranking_file->scores[i]);
        printxy(screen, ranking_string, (SCREEN_WIDTH - strlen(ranking_string)) / 2, y);
        printxy(screen, " ", 0, y);
        y += 2;
    }
}

void update_ranking(RankingFile* ranking_file, char* name, int score)
{
    bool is_changed = false;
    for (int i = 0; i < ranking_file->num; i++)
    {
        if (score <= ranking_file->scores[i] && !strcmp(ranking_file->names[i], name))
        {
            is_changed = true;
            break;
        }
        else if (score > ranking_file->scores[i] && strcmp(ranking_file->names[i], name))
        {
            for (int j = ranking_file->num; j >= i + 1; j--)
            {
                ranking_file->scores[j] = ranking_file->scores[j - 1];
                strcpy(ranking_file->names[j], ranking_file->names[j - 1]);
            }
            ranking_file->scores[i] = score;
            strcpy(ranking_file->names[i], name);
            is_changed = true;
            ranking_file->num++;
            break;
        }
        else if (score > ranking_file->scores[i] && !strcmp(ranking_file->names[i], name))
        {
            ranking_file->scores[i] = score;
            is_changed = true;
            break;
        }
        if (is_changed) break;
    }
    if (!is_changed && ranking_file->num < 10)
    {
        ranking_file->scores[ranking_file->num] = score;
        strcpy(ranking_file->names[ranking_file->num], name);
        ranking_file->num++;
    }
    if (ranking_file->num > 10) ranking_file->num = 10;
}

void hide_cursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void show_cursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

int main()
{
    srand(time(NULL));
    hide_cursor();
    char** screen = create_screen(SCREEN_WIDTH + 1, SCREEN_HEIGHT + 1);
    char* buffer = (char*)malloc((SCREEN_WIDTH * SCREEN_HEIGHT + 1) * sizeof(char));
    Element* title_logo = show_banner(screen, TITLE_LOGO_SHAPE, TITLE_LOGO_WIDTH, TITLE_LOGO_HEIGHT);
    RankingFile* ranking_file = NULL;
    char* file_name = "ranking.bin";
    char name[10] = "Guest";
    if (!file_exists(file_name))
    {
        ranking_file = (RankingFile*)malloc(sizeof(RankingFile));
        memset(ranking_file, 0, sizeof(RankingFile));
    }
    else ranking_file = open_ranking_file(file_name);
    while (1)
    {
        while (1)
        {
            write_element(screen, title_logo);
            update_screen(screen, buffer);
            fflush(stdin);
            int option = _getch();
            reset_screen(screen);
            if (option == 49 || option == 97) // option 1 - view ranking
            {
                show_ranking(screen, ranking_file);
                update_screen(screen, buffer);
                _getch();
            }
            else if (option == 50 || option == 98) // option 2 - set nickname
            {
                show_cursor();
                set_name(screen, buffer, name);
                hide_cursor();
            }
            else break;
            reset_screen(screen);
        }
        Element* space_ship = load_space_ship(screen);
        AttackElement** meteors = create_attack_element_list(50);
        AttackElement** bullets = create_attack_element_list(500);
        int current_meteors = 0;
        int current_bullets = 0;
        int score = 0;
        int life = 3;
        bool exit_game = false;
        update_screen(screen, buffer);
        while (1)
        {
            int key = get_key_input();
            move_element(screen, space_ship, key);
            move_element(screen, space_ship, key);
            if (key == 5)
                shoot(screen, bullets, space_ship, &current_bullets);
            else if (key == 6)
                exit_game = true;
            create_meteors(screen, meteors, &current_meteors);
            drop_meteors(screen, meteors, &current_meteors, &life);
            lift_bullets(screen, bullets, &current_bullets);
            check_meteor_bullet_collid(screen, meteors, bullets, &current_meteors, &current_bullets, &score);
            if (life <= 0 || exit_game)
            {
                reset_screen(screen);
                show_score(screen, score);
                show_life(screen, life);
                show_name(screen, name);
                update_screen(screen, buffer);
                Element* game_over_element = show_banner(screen, GAME_OVER_SHAPE, GAME_OVER_WIDTH, GAME_OVER_HEIGHT);
                update_screen(screen, buffer);
                fflush(stdin);
                _getch();
                free(game_over_element);
                break;
            }
            show_score(screen, score);
            show_life(screen, life);
            show_name(screen, name);
            update_screen(screen, buffer);
        }
        free(space_ship);
        free_attack_element_list(meteors, 50, current_meteors);
        free_attack_element_list(bullets, 100, current_bullets);
        reset_screen(screen);
        update_ranking(ranking_file, name, score);
        save_ranking_file(file_name, ranking_file);
    }
    return 0;
}
