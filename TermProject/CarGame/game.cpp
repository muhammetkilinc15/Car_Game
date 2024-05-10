#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>

// Ekranın boyutları
#define wHeight 40 // Yolun yüksekliği
#define wWidth 100 // Yolun genişliği

// Orta çizgi ve bitiş koordinatları
#define lineX 45 // Ortadaki çizginin x koordinatı
#define lineLEN 10 // Orta çizginin başlangıc ve sonundan uzaklığı
#define EXITY 35 // Yolun bitiş koordinatı

// Klavye girdileri
#define leftKeyArrow 260 // Sol ok tuşunun ASCII kodu
#define RightKeyArrow 261 // Sağ ok tuşunun ASCII kodu
#define leftKeyA 97 // A'nın ASCII kodu
#define RightKeyD 100 // D'nin ASCII kodu
#define ESC 27 // ESC tuşunun ASCII kodu
#define ENTER 10 // ENTER tuşunun ASCII kodu
#define KEYPUP 259 // Yukarı ok tuşunun ASCII kodu
#define KEYDOWN 258 // Aşağı ok tuşunun ASCII kodu
#define KEYERROR -1 // Yanlış bir tuşa basıldığında döndürülen ASCII kodu
#define SAVEKEY 115 // S'nin ASCII kodu

// Seviye ve hız sınırları
#define levelBound 300 // 300 puanı geçince seviyeyi artırmak için
#define MAXSLEVEL 5 // Maksimum seviye
#define ISPEED 500000 // Oyun hareket hızı için başlangıç değeri
#define DRATESPEED 100000 // Her yeni seviyeden sonra hareket hızını azaltmak için

// Araba özellikleri
#define MINX 5 // Arabaların oluşturulurken minimum x koordinat değeri
#define MINY 10 // Arabaların oluşturulurken maksimum y koordinat değeri
#define MINH 5 // Arabaların oluşturulurken minimum yükseklik değeri
#define MINW 5 // Arabaların oluşturulurken minimum genişlik değeri
#define SPEEDOFCAR 3 // Oyuncu tarafından kullanılan arabanın hızı
#define YOFCAR 34 // Oyuncu tarafından kullanılan arabanın y koordinatı
#define XOFCAR 45 // Oyuncu tarafından kullanılan arabanın x koordinatı
#define IDSTART 10 // Araba kimliği için başlangıç değeri
#define IDMAX 20 // Araba kimliği için maksimum değer
#define COLOROFCAR 3 // Oyuncu tarafından kullanılan arabanın renk değeri
#define POINTX 91 // Puanın yazıldığı x koordinatı
#define POINTY 42 // Puanın yazıldığı y koordinatı

// Menü özellikleri
#define MENUX 10 // Menülerin başlangıç satırı için x koordinatı
#define MENUY 5 // Menülerin başlangıç satırı için y koordinatı
#define MENUDIF 2 // Menü satırları arasındaki fark
#define MENUDIFX 20 // Menü sütunları arasındaki fark
#define MENSLEEPRATE 200000 // Menü giriş için bekleme süresi
#define GAMESLEEPRATE 250000 // Oyuncu ok tuşları için bekleme süresi

// Diğer sabitler
#define EnQueueSleep 1 // EnQueue bekleme süresi
#define DeQueueSleepMin 2 // DeQueue minimum bekleme süresi
#define numOfcolors 4 // Arabalar için seçilebilecek maksimum renk değeri
#define maxCarNumber 5 // Kuyruktaki maksimum araba sayısı
#define numOfChars 3 // Arabalar için seçilebilecek maksimum desen sayısı
#define settingMenuItem 2 // Ayarlar menüsündeki seçenek sayısı
#define mainMenuItem 6 // Ana menüdeki seçenek sayısı
using namespace std;


typedef struct Car{ //
    int ID;
    int x;
    int y;
    int height;
    int width;
    int speed;
    int clr;
    bool isExist;
    char chr;
}Car;

typedef struct Game{
    int leftKey;
    int rightKey;
    queue<Car> cars;
    bool IsGameRunning;
    bool IsSaveCliked;
    int counter;
    pthread_mutex_t mutexFile;
    Car current;
    int level;
    int moveSpeed;
    int points;
}Game;

Game playingGame; // Global variable used for new game
const char *gameTxt =  "game.txt";
const char *CarsTxt =  "cars.txt";
const char *pointsTxt =  "points.txt";
//Array with options for the Setting menu
const char *settingMenu[50] = {"Play with < and > arrow keys","Play with A and D keys"};
//Array with options for the Main menu
const char *mainMenu[50] = {"New Game","Load the last game","Instructions", "Settings","Points","Exit"};
void drawCar(Car c, int type, int direction); //prints or remove the given car on the screen
void printWindow(); //Draws the road on the screen
void *newGame(void *); // manages new game
void initGame(); // Assigns initial values to all control parameters for the new game
void initWindow(); //Creates a new window and sets I/O settings


int main()
{
    playingGame.leftKey = leftKeyArrow;
    playingGame.rightKey = RightKeyArrow;
    initGame(); // ekran baslatildi
    initWindow();
    pthread_t th1; //create new thread
    pthread_create(&th1, NULL, newGame,NULL);// Run newGame function with thread
    pthread_join(th1, NULL); //Wait for the thread to finish, when the newGame function finishes, the thread will also finish.
    return 0;
}


// init metodu ellenmicek
void initGame()
{
    playingGame.cars = queue<Car>();
    playingGame.counter =IDSTART;
    playingGame.mutexFile = PTHREAD_MUTEX_INITIALIZER; //assigns the initial value for the mutex
    playingGame.level = 1;
    playingGame.moveSpeed = ISPEED;
    playingGame.points = 0;
    playingGame.IsSaveCliked = false;
    playingGame.IsGameRunning = true;
    playingGame.current.ID = IDSTART-1;
    playingGame.current.height = MINH;
    playingGame.current.width = MINW;
    playingGame.current.speed = SPEEDOFCAR;
    playingGame.current.x = XOFCAR;
    playingGame.current.y = YOFCAR;
    playingGame.current.clr = COLOROFCAR;
    playingGame.current.chr = '*';
}

// new game baslar baslamaz yolu ciziyor
// kullanıcı
void *newGame(void *)
{
    printWindow(); 
    drawCar(playingGame.current,2,1); // Draw the car the player is driving on the screen
    int key;
    while (playingGame.IsGameRunning) { //continue until the game is over
            key = getch(); //Get input for the player to press the arrow keys
            if (key != KEYERROR) {
                 if (key == playingGame.leftKey) { // If the left  key is pressed
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x-=playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1); // draw player's car with new position
                }
            }
         usleep(GAMESLEEPRATE); // sleep  0.25 sn de bir gerceklestirecek
        }
}
void initWindow()
{
	initscr();            // initialize the ncurses window
	start_color();        // enable color manipulation
	keypad(stdscr, true); // enable the keypad for the screen
	nodelay(stdscr, true);// set the getch() function to non-blocking mode
	curs_set(0);          // hide the cursor
	cbreak();             // disable line buffering
	noecho();             // don't echo characters entered by the user, ekrana a ya da d yazilmamasi icin
	clear();              // clear the screen
    sleep(1);
}
void printWindow()
{
    for (int i = 1; i < wHeight - 1; ++i) {
		//mvprintw: Used to print text on the window, paramters order: y , x , string
        mvprintw(i, 2, "*"); //left side of the road
        mvprintw(i, 0, "*"); 
        mvprintw(i, wWidth - 1, "*");// right side of the road
        mvprintw(i, wWidth - 3, "*");
    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) { //line in the middle of the road
        mvprintw(i, lineX, "#");
    }
}


// type 2 oldugu zaman ciz , 1 oldugu zaman sil
void drawCar(Car c, int type, int direction )
{
	//If the user does not want to exit the game and the game continues
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)
    {

            init_pair(c.ID, c.clr, 0);// Creates a color pair: init_pair(short pair ID, short foregroundcolor, short backgroundcolor);
                                                //0: Black (COLOR_BLACK)
                                                //1: Red (COLOR_RED)
                                                //2: Green (COLOR_GREEN)
                                                //3: Yellow (COLOR_YELLOW)
                                                //4: Blue (COLOR_BLUE)
			attron(COLOR_PAIR(c.ID));//enable color pair
            char drawnChar;
            if (type == 1 ) 
               drawnChar = ' '; // to remove car
            else
               drawnChar= c.chr; //  to draw char
		    //mvhline: used to draw a horizontal line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvhline(c.y, c.x, drawnChar, c.width);// top line of rectangle
            mvhline(c.y + c.height - 1, c.x, drawnChar, c.width); //bottom line of rectangle
            if(direction == 0) // If it is any car on the road
                mvhline(c.y + c.height, c.x, drawnChar, c.width); 
            else //player's card
                mvhline(c.y -1, c.x, drawnChar, c.width);
		    //mvvline: used to draw a vertical line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvvline(c.y, c.x, drawnChar, c.height); //left line of rectangle
            mvvline(c.y, c.x + c.width - 1, drawnChar, c.height); //right line of rectangle
            char text[5];
            if (type == 1 )
                sprintf(text,"  "); //to remove point 
            else
                 sprintf(text,"%d",c.height * c.width); // to show car's point in rectangle
            mvprintw(c.y+1, c.x +1, text);// display car's point in rectangle
            attroff(COLOR_PAIR(c.ID));// disable color pair
    }
}
