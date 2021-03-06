#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>

#define DELAY 25000
#define MAX_SCORE 9

int c;

struct screen
{
    int max_x; int max_y;
} Screen;

struct score
{
    int x; int y;
} Score;

struct paddle
{
    double x; double y;
    int w; int h;
    double vel;
};
typedef struct paddle Paddle;

struct ball
{
    double x; double y;
    double velx; double vely;
};
typedef struct ball Ball;

char *choices[] = {
    "Player vs Computer",
    "Exit"
};

int n_choices = sizeof(choices) / sizeof(char *);
int choice = -1;
void print_menu(int highlight);

Paddle *makePaddle(double x, double y, int width, int height, double vel);
void destroyPaddle(Paddle *paddle);
void movePaddleUp(Paddle *paddle);
void movePaddleDown(Paddle *paddle);
void printPaddle(Paddle *paddle);
void aiPaddle(Ball *b, Paddle *p);

Ball *makeBall();
void destroyBall(Ball *ball);
void moveBall(Ball *ball);
void ballCollisionLR(Ball *ball, Paddle *paddle);
void ballCollisionUD(Ball *ball);
void ballBounds(Ball *b);
void printBall(Ball *ball);

void playerControl(Paddle *p, char up, char down);

void game();
void roundReset(Ball *b);
void gameReset(Ball *b, Paddle *p1, Paddle *p2);

int main()
{
    int highlight = n_choices;

    initscr();
    getmaxyx(stdscr, Screen.max_y, Screen.max_x);
    start_color();
    raw();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    refresh();

    init_pair(0, 0, 0); // black background
    init_pair(1, 0, 1);
    init_pair(2, 4, 3);
    init_pair(3, 2, 7);
    init_pair(4, 0, 7);

    while (1)
    {
        attrset(COLOR_PAIR(4));
        print_menu(highlight);
        c = getch();
        switch (c)
        {
        case KEY_UP:
            if (highlight == 1 )
                highlight = n_choices;
            else
                --highlight;
            break;
        case KEY_DOWN:
            if (highlight == n_choices)
                highlight = 1;
            else
                ++highlight;
            break;
        case 10:
            choice = highlight;
            break;
        default:
            refresh();
            break;
        }

        if (choice == n_choices)
        {
            clear();
            game();
        }
        else if (choice == 1)
        {
            clear;
            endwin();
            return 0;
        }
    }

    refresh();
}

void print_menu(int highlight)
{
    int x, y, i;

    y = 10;
    x = 30;
    for (i = 0; i < n_choices; ++i)
    {
        if (highlight == i + 1)
        {
            attron(A_REVERSE);
            mvprintw(y, x, "%s", choices[i]);
            attroff(A_REVERSE);
        }
        else
            mvprintw(y, x, "%s", choices[i]);
        ++y;
    }
    refresh();
}

Paddle *makePaddle(double x, double y, int width, int height, double vel)
{
    Paddle *p = malloc(sizeof(Paddle));
    p->x = x; p->y = y;
    p->w = width; p->h = height;
    p->vel = vel;

    return p;
}

void destroyPaddle(Paddle *paddle)
{
    free(paddle);
    paddle = NULL;
}

void movePaddleUp(Paddle *paddle)
{
    if (paddle->y - paddle->vel < 0);
    else
    {
        paddle->y -= paddle->vel;
    }
}

void movePaddleDown(Paddle *paddle)
{
    if (paddle->y + paddle->h + paddle->vel > Screen.max_y);
    else
    {
        paddle->y += paddle->vel;
    }
}

void aiPaddle(Ball *b, Paddle *p)
{
    if ((rand() % 2) && p->y > b->y) movePaddleUp(p);
    else if ((rand() % 2) && p->y < b->y) movePaddleDown(p);
}

Ball *makeBall()
{
    Ball *p = malloc(sizeof(Ball));
    p->x = Screen.max_x / 2;
    p->y = Screen.max_y / 2;
    p->velx = 1.0;
    p->vely = 0.5;

    return p;
}

void destroyBall(Ball *ball)
{
    free(ball);
    ball = NULL;
}

void moveBall(Ball *ball)
{
    ball->x += ball->velx;
    ball->y += ball->vely;
}

void ballCollisionLR(Ball *ball, Paddle *paddle)
{
    if (ball->x >= paddle->x && ball->x <= paddle->x + paddle->w)
        if (ball->y >= paddle->y && ball->y <= paddle->y + paddle->h)
            ball->velx = -ball->velx;
}

void ballBounds(Ball *b)
{
    if (b->x < 0)
    {
        Score.y++;
        //usleep(DELAY * 10);
        roundReset(b);
    }
    else if (b->x > Screen.max_x)
    {
        Score.x++;
        //usleep(DELAY * 10);
        roundReset(b);
    }
}

void ballCollisionUD(Ball *ball)
{
    if (ball->y + ball->vely < 0) ball->vely = -ball->vely;
    else if (ball->y + ball->vely > Screen.max_y) ball->vely = -ball->vely;
}

void printBall(Ball *ball)
{
    mvprintw(ball->y, ball->x, " ");
}


void printPaddle(Paddle *paddle)
{
    int w, h;
    for(w = 0; w < paddle->w; ++w)
        for(h = 0; h < paddle->h; ++h)
            mvprintw(paddle->y + h, paddle->x + w, " ");
}

void playerControl(Paddle *p, char up, char down)
{
    c = wgetch(stdscr);
    if (c == up) movePaddleUp(p);
    else if (c == down) movePaddleDown(p);
    else if (c == 27) endwin();
}

void roundReset(Ball *b)
{
    b->x = Screen.max_x / 2; b->y = Screen.max_y / 2;
    srand(time(NULL));
    if (rand() % 2) b->velx = (-1) * b->velx;
    if (rand() % 2) b->vely = (-1) * b->vely;
}

void gameReset(Ball *b, Paddle *p1, Paddle *p2)
{
    roundReset(b);
    p1->x = 1; p2->x = Screen.max_x - p2->w - 1;
    p1->y = Screen.max_y / 2; p2->y = Screen.max_y / 2;
    Score.x = Score.y = 0;
}

void printScore()
{
    int half = (Screen.max_x - 4) / 2;
    int i = 0;
    for (i = 0; i < Screen.max_y; i++)
    {
        mvprintw(i, half, "|");
    }
    attron(A_BOLD);
    mvprintw(1, (half - 12) / 2, "Player 1: %d", Score.x);
    mvprintw(1, half + 2 + (half - 12) / 2, "Player 2: %d", Score.y);
    attroff(A_BOLD);
}

void printWinner()
{
    if (Score.x == MAX_SCORE) mvprintw(Screen.max_y / 2, Screen.max_x / 2 - 7, "Player 1 Wins");
    else if (Score.y == MAX_SCORE) mvprintw(Screen.max_y / 2, Screen.max_x / 2 - 7, "Player 2 Wins");
}

void game()
{
    Ball *b = makeBall(Screen.max_x / 2, Screen.max_y / 2, -1, 1);
    Paddle *p1 = makePaddle(0, Screen.max_y / 2, 1, 4, 2);
    Paddle *p2 = makePaddle(Screen.max_x, Screen.max_y / 2, 1, 4, 2);
    timeout(0);

    gameReset(b, p1, p2);

    while (Score.x < MAX_SCORE && Score.y < MAX_SCORE)
    {
        clear();

        ballCollisionLR(b, p1);
        ballCollisionLR(b, p2);
        ballCollisionUD(b);
        ballBounds(b);
        moveBall(b);

        usleep(DELAY);

        aiPaddle(b, p1);
        playerControl(p2, 'k', 'm');

        attrset(COLOR_PAIR(0));
        printScore();
        attrset(COLOR_PAIR(1));
        printBall(b);
        attrset(COLOR_PAIR(2));
        printPaddle(p1);
        attrset(COLOR_PAIR(3));
        printPaddle(p2);
        refresh();
    }
    destroyBall(b);
    destroyPaddle(p1);
    destroyPaddle(p2);
    choice = -1;
    clear();
    printWinner();
    refresh();
    usleep(3000000);

    clear();

}
