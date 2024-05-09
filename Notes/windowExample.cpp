#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <iostream>
#include <ctime>
using namespace std;

int main()
{
    initscr();	//--> Ekranı oluşturuyor
    start_color();	//--> Renk kullanmayı açar
    keypad(stdscr, true);	//--> Özel karakter girebilmeyi sağlar
    nodelay(stdscr, true);	//--> input için beklemez
    curs_set(0);	//--> Cursor u siler
    cbreak();	//--> Karakter girildiği an işlenmesi için
    noecho();	//--> echo olmaması için
    int x =10, y = 5;
    init_pair(1, COLOR_GREEN, COLOR_BLACK);		//--> 1 nolu renk (yeşil yazı - siyah bg)
    attron(COLOR_PAIR(1));		//--> rengi kullanır
    char text[50];		
    sprintf(text,"< or A: moves the car to the left");
    mvprintw(y, x, text);		//--> Verilen konuma metni yazdırır
    y+= 2;
    sprintf(text,"> or D: moves the car to the right");
    mvprintw(y, x, text);
    attroff(COLOR_PAIR(1));		//--> Rengi devre dışı bırakır
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
            mvaddch(i, j, '#');		//--> Verilen konuma add char işlemini yapar
        }
    }
	
	x = 15, y = 25;
	mvhline(y, x, '#', w);		//--> Belirlenen konumdan yatay çizgi yapar ("w" kaç charlık çizgi olacağı)
	mvhline(y + h - 1, x, '#', w);
	mvvline(y, x, '#', h);			//--> Belirlenen konumdan dikey çizgi yapar
	mvvline(y, x + w - 1, '#', h);
	
	refresh();
    usleep(3000000);		//--> milisaniye 3.000.000 ms => 3 saniye
	attroff(COLOR_PAIR(2));
    clear();
    usleep(1000000);
    endwin();
    return 0;
}



