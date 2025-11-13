#include "ncurses/curses.h"
#include <cstdlib>
#include <ctime>

// Game terminal sederhana menggunakan ncurses
// - '@' = pemain
// - 'E' = musuh (bergerak acak)
// - '#' = dinding
// WASD/arrows untuk bergerak, q untuk keluar

const int MAP_W = 40;
const int MAP_H = 20;                   

char map[MAP_H][MAP_W+1];
int px, py; // player
int ex, ey; // musuh

void init_map() {
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            if (y == 0 || y == MAP_H-1 || x == 0 || x == MAP_W-1) map[y][x] = '#';
            else map[y][x] = ' ';
        }
        map[y][MAP_W] = '\0';
    }
    // tambahkan beberapa dinding acak
    for (int i = 0; i < 80; ++i) {
        int rx = rand() % (MAP_W-2) + 1;
        int ry = rand() % (MAP_H-2) + 1;
        map[ry][rx] = '#';
    }
}

void draw() {
    clear();
    for (int y = 0; y < MAP_H; ++y) {
        mvprintw(y, 0, "%s", map[y]);
    }
    mvaddch(py, px, '@' | A_BOLD | COLOR_PAIR(2));
    mvaddch(ey, ex, 'E' | COLOR_PAIR(3));
    mvprintw(MAP_H+1, 0, "WASD/Arrows to move, q to quit");
    refresh();
}

bool walkable(int x, int y) {
    return x >= 1 && x < MAP_W-1 && y >= 1 && y < MAP_H-1 && map[y][x] != '#';
}

int main() {
    srand(time(nullptr));
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
    }

    init_map();

    // tempatkan player dan musuh di posisi kosong
    do { px = rand() % (MAP_W-2) + 1; py = rand() % (MAP_H-2) + 1; } while (!walkable(px,py));
    do { ex = rand() % (MAP_W-2) + 1; ey = rand() % (MAP_H-2) + 1; } while (!walkable(ex,ey) || (ex==px && ey==py));

    bool running = true;
    draw();
    while (running) {
        int ch = getch();
        int nx = px, ny = py;
        switch (ch) {
            case 'q': case 'Q': running = false; break;
            case KEY_UP: case 'w': case 'W': ny = py-1; break;
            case KEY_DOWN: case 's': case 'S': ny = py+1; break;
            case KEY_LEFT: case 'a': case 'A': nx = px-1; break;
            case KEY_RIGHT: case 'd': case 'D': nx = px+1; break;
            default: break;
        }
        if (walkable(nx,ny)) { px = nx; py = ny; }

        // musuh bergerak acak sederhana
        int dx = (rand()%3)-1; // -1,0,1
        int dy = (rand()%3)-1;
        int enx = ex + dx;
        int eny = ey + dy;
        if (walkable(enx,eny) && !(enx==px && eny==py)) { ex = enx; ey = eny; }

        // jika pemain menyentuh musuh, game over
        if (ex == px && ey == py) {
            mvprintw(MAP_H+2, 0, "Kamu Berhasil Menangkap Musuh! Tekan sembarang tombol untuk keluar...");
            getch();
            break;
        }

        draw();
    }

    endwin();
    return 0;
}
