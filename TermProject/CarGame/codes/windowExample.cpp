#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <iostream>
#include <ctime>
using namespace std;

int main()
{
    initscr();
    start_color();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    curs_set(0);
    cbreak();
    noecho();
    int x =10, y = 5;
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    char text[50];
    sprintf(text,"< or A: moves the car to the left");
    mvprintw(y, x, text);
    y+= 2;
    sprintf(text,"> or D: moves the car to the right");
    mvprintw(y, x, text);
    attroff(COLOR_PAIR(1));
    refresh();
    sleep(5);
    clear();
	refresh();
    x = 10, y = 5;
    int h = 5, w = 10;
    init_pair(2, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(2));
    for (int i = x; i <x + h; ++i) {
        for (int j = y; j <y + w; ++j) {
            mvaddch(i, j, '#');
        }
    }
	
	x = 15, y = 25;
	mvhline(y, x, '#', w);
	mvhline(y + h - 1, x, '#', w);
	mvvline(y, x, '#', h);
	mvvline(y, x + w - 1, '#', h);
	
	refresh();
    usleep(3000000);
	attroff(COLOR_PAIR(2));
    clear();
    usleep(1000000);
    endwin();
    return 0;
}



