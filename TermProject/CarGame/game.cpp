#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>

#define wHeight 40 // height of the road
#define wWidth 100 // width of the road
#define lineX 45 // x coordinate of the middle line
#define lineLEN 10 // distance of the middle line from the beginning and the end
#define EXITY 35 // coordinate showing the end of the road
#define leftKeyArrow 260 // ASCII code of the left arrow key
#define RightKeyArrow 261 // ASCII code of the right arrow key
#define leftKeyA 97// ASCII code of A
#define RightKeyD 100 // ASCII code of D
#define ESC 27 // // ASCII code of the ESC key
#define ENTER 10 // ASCII code of the ENTER key
#define KEYPUP 259 // ASCII code of the up arrow key
#define KEYDOWN 258 // ASCII code of the down arrow key
#define KEYERROR -1 // ASCII code returned if an incorrect key is pressed
#define SAVEKEY 115 // ASCII code of S
#define levelBound 300 // To increase level after 300 points
#define MAXSLEVEL 5 // maximum level
#define ISPEED 500000 // initial value for game moveSpeed
#define DRATESPEED 100000 // to decrease moveSpeed after each new level
#define MINX 5 // minimum x coordinate value when creating cars
#define MINY 10 // the maximum y coordinate value when creating the cars, then we multiply it by -1 and take its inverse
#define MINH 5 // minimum height when creating cars
#define MINW 5 // minimum width when creating cars
#define SPEEDOFCAR 3 // speed of the car driven by the player
#define YOFCAR 34 // y coordinate of the car used by the player
#define XOFCAR 45 // x coordinate of the car used by the player
#define IDSTART 10 // initial value for cars ID
#define IDMAX 20// maximum value for cars ID
#define COLOROFCAR 3 // color value of the car used by the player
#define POINTX 91 //x coordinate where the point is written
#define POINTY 42 //y coordinate where the point is written
#define MENUX 10 // x coordinate for the starting row of the menus
#define MENUY 5 // y coordinate for the starting row of the menus
#define MENUDIF 2 // difference between menu rows
#define MENUDIFX 20 // difference between menu columns
#define MENSLEEPRATE 200000 // sleep time for menu input
#define GAMESLEEPRATE 250000 // sleep time for player arrow keys
#define EnQueueSleep 1 // EnQueue sleep time
#define DeQueueSleepMin 2 // DeQueue minimum sleep time
#define numOfcolors 4 // maximum color value that can be selected for cars
#define maxCarNumber 5 // maximum number of cars in the queue
#define numOfChars 3 // maximum number of patterns that can be selected for cars
#define settingMenuItem 2 // number of options in the setting menu
#define mainMenuItem 6 // number of options in the main menu
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



void printMenu();
void printInstructor();
void printSettings();
void printPoint();
void printInstructor();
void *generateCar(void *);
void *enqueue(void *);
void *moveCar(void *data);
void *Dequeue(void *);
int create_car_x(int width);
int create_car_y();
int create_car_height();
int create_car_width();
int create_car_speed(int height);
int create_car_clr();
char create_car_type();

int main()
{
    playingGame.leftKey = leftKeyArrow;
    playingGame.rightKey = RightKeyArrow;
    initGame();
    initWindow();
    printMenu();
    pthread_t th2;

    return 0;
}


void printMenu()
{
	endwin();
    int selectedItem = 0; // Seçili menü öğesinin indisini saklar
    int key = -1; // Klavyeden alınacak tuş değeri için değişken

    while (true) // ENTER tuşuna basılmadığı sürece devam et
    {
        for (int i = 0; i < mainMenuItem; i++)
        {
            // Seçili öğenin rengini belirle
            if (i == selectedItem)
            {
                attron(COLOR_PAIR(2)); // Kırmızı renk çiftini etkinleştir
                mvprintw(MENUY + (i * MENUDIF), MENUX - 2,"->");
                mvprintw(MENUY + (i * MENUDIF), MENUX,mainMenu[i]); // Menü öğesini ekrana yazdır
            }else
            {
                attron(COLOR_PAIR(1)); // Yeşil renk çiftini etkinleştir
                mvprintw(MENUY + (i * MENUDIF), MENUX,mainMenu[i]); // Menü öğesini ekrana yazdır
            }
        }

        attroff(COLOR_PAIR(1)); // Yeşil renk çiftini devre dışı bırak
        attroff(COLOR_PAIR(2)); // Kırmızı renk çiftini devre dışı bırak

        key = getch(); // Klavyeden giriş al
        if(key != KEYERROR)
        {
            clear();
        }
        if (key == KEYDOWN) // Aşağı ok tuşuna basıldığında
        {
            selectedItem = (selectedItem + 1) % mainMenuItem; // Seçili öğeyi bir sonraki öğeye taşı
        }
        else if (key == KEYPUP) // Yukarı ok tuşuna basıldığında
        {
            selectedItem = (selectedItem - 1 + mainMenuItem) % mainMenuItem; // Seçili öğeyi bir önceki öğeye taşı
        }else if(key ==ENTER)
        {
            pthread_t th2;
            clear();

			initWindow();
            switch(selectedItem)
            {
                case 0:
						initGame();
                        pthread_create(&th2, NULL, newGame, NULL);
                        pthread_join(th2, NULL);
                    break;
                case 2:
                        printInstructor();
                    break;
                case 3:
                     printSettings();
                    break;
                case 4:
                         printPoint();
                        break;
                case 5:
                    playingGame.IsGameRunning=false;
                    break;
            }
			endwin();
			initWindow();
            clear();
        }
        usleep(MENSLEEPRATE);
    }
}


void printInstructor()
{
	endwin();
	initWindow();
    attron(COLOR_PAIR(1));
    char instructMenu[4][50]={"< or A: moves the car to the left","> or D: moves the car to right","ESC: exist the game without saving","S: saves and exists the game"};

    for(int i=0;i<4;i++)
    {
         mvprintw(MENUY + (i * MENUDIF), MENUX,instructMenu[i]);
    }
    refresh();
    sleep(5);
}


void printSettings()
{
	endwin();
	initWindow();
    int selectedItem = 0; // Seçili menü öğesinin indisini saklar
    int key = -1; // Klavyeden alınacak tuş değeri için değişken

    while (true) // ENTER tuşuna basılmadığı sürece devam et
    {

        for (int i = 0; i < settingMenuItem; i++)
        {
            if (i == selectedItem)
            {
                attron(COLOR_PAIR(2)); // Kırmızı renk çiftini etkinleştir
                mvprintw(MENUY + (i * MENUDIF), MENUX - 2,"->");
                mvprintw(MENUY + (i * MENUDIF), MENUX,settingMenu[i]); // Menü öğesini ekrana yazdır
            }else
            {
                attron(COLOR_PAIR(1)); // Yeşil renk çiftini etkinleştir
                mvprintw(MENUY + (i * MENUDIF), MENUX,settingMenu[i]); // Menü öğesini ekrana yazdır
            }
        }

        attroff(COLOR_PAIR(1)); // Yeşil renk çiftini devre dışı bırak
        attroff(COLOR_PAIR(2)); // Kırmızı renk çiftini devre dışı bırak

        key = getch(); // Klavyeden giriş al
        if(key != KEYERROR)
        {
            clear();
        }
        if (key == KEYDOWN) // Aşağı ok tuşuna basıldığında
        {
            selectedItem = (selectedItem + 1) % settingMenuItem; // Seçili öğeyi bir sonraki öğeye taşı
        }
        else if (key == KEYPUP) // Yukarı ok tuşuna basıldığında
        {
            selectedItem = (selectedItem - 1 + settingMenuItem) % settingMenuItem; // Seçili öğeyi bir önceki öğeye taşı
        }else if(key ==ENTER)
        {
             switch(selectedItem)
             {
                case 0:
                    playingGame.leftKey= leftKeyArrow;
                    playingGame.rightKey= RightKeyArrow;
                    break;
                case 1:
                    playingGame.leftKey= leftKeyA;
                    playingGame.rightKey= RightKeyD;
                    break;
             }
            break;
        }
    }
}

void printPoint()
{
	endwin();
	initWindow();
    FILE *file = fopen(pointsTxt,"r");
    attron(COLOR_PAIR(1));
    int column = MENUX; // İlk sütunun başlangıç koordinatı
    int counter = 0; // Toplam oyun sayacı
    int x = MENUY; // Yükseklik koordinatı

    if (file != NULL) {
        int point;
        while (fscanf(file, "%d", &point) != EOF)
        {
            counter++; // Her puan alındığında toplam oyun sayısını artır
            char text[50];
            sprintf(text, "Game %d : %d", counter, point); // Yazdırılacak metni oluştur
            mvprintw(x, column, text); // Metni ekrana yazdır
            if (counter % 10 == 0)
            {
                column += MENUDIFX; // Her 10 oyun sonrasında sütunları kaydır
                x = MENUY; // Yeni sütunda yükseklik koordinatını sıfırla
            }
            else
            {
                x += MENUDIF; // Her puan sonrasında yükseklik koordinatını artır
            }
        }
        fclose(file);
    }
    else
    {
        mvprintw(MENUY, MENUX, "Error: Cannot open points file");
    }
    refresh();
    sleep(5);
}





void initWindow()
{
	initscr();            // ncurses penceresini baþlat
	start_color();        // renk iþlemlerini etkinleþtir
	keypad(stdscr, true); // ekran için klavye giriþini etkinleþtir
	nodelay(stdscr, true);// getch() fonksiyonunu bloklamayan modda ayarla
	curs_set(0);          // imleci gizle
	cbreak();             // satýr tamponlamayý devre dýþý býrak
	noecho();             // kullanýcýnýn girdiðini ekrana yazma
	clear();              // ekranı temizle
    sleep(1);             // 1 saniye bekle
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Yeşil rengi aktifleştirir
    init_pair(2, COLOR_RED, COLOR_BLACK); // kırmızı rengi aktifleştirir
}


void initGame()
{
    playingGame.cars = queue<Car>();                        // araç kuyruðunu boþalt
    playingGame.counter =IDSTART;                           // araç kimliði için baþlangýç deðerini ayarla
    playingGame.mutexFile = PTHREAD_MUTEX_INITIALIZER;      // mutex için baþlangýç deðerini ata
    playingGame.level = 1;                                  // seviye baþlangýç deðerini ayarla
    playingGame.moveSpeed = ISPEED;                         // hareket hýzý baþlangýç deðerini ayarla
    playingGame.points = 0;                                 // puan baþlangýç deðerini ayarla
    playingGame.IsSaveCliked = false;                       // kaydetme butonunun baþlangýç durumunu ayarla
    playingGame.IsGameRunning = true;                       // oyunun çalýþma durumunu ayarla
    playingGame.current.ID = IDSTART-1;                     // kullanýcýnýn aracý için kimlik baþlangýç deðerini ayarla
    playingGame.current.height = MINH;                      // kullanýcýnýn aracý için yükseklik baþlangýç deðerini ayarla
    playingGame.current.width = MINW;                       // kullanýcýnýn aracý için geniþlik baþlangýç deðerini ayarla
    playingGame.current.speed = SPEEDOFCAR;                 // kullanýcýnýn aracý için hýz baþlangýç deðerini ayarla
    playingGame.current.x = XOFCAR;                         // kullanýcýnýn aracý için x koordinat baþlangýç deðerini ayarla
    playingGame.current.y = YOFCAR;                         // kullanýcýnýn aracý için y koordinat baþlangýç deðerini ayarla
    playingGame.current.clr = COLOROFCAR;                   // kullanýcýnýn aracý için renk baþlangýç deðerini ayarla
    playingGame.current.chr = '*';                          // kullanýcýnýn aracý için karakter baþlangýç deðerini ayarla
}

void *newGame(void *)
{
	endwin();
	initWindow();
    printWindow();                                          // yolun çizilmesini baþlat
    drawCar(playingGame.current,2,1);                       // oyuncunun kullandýðý aracý ekrana çiz
    int key;
    pthread_t enqueueThread , dequeueThread;
    pthread_create(&enqueueThread, NULL, enqueue, NULL);
    pthread_create(&dequeueThread, NULL, Dequeue, NULL);
    while (playingGame.IsGameRunning) {                     // oyun sona erene kadar devam et
            key = getch();
            if (key != KEYERROR) {
                 if (key == playingGame.leftKey && playingGame.current.x-playingGame.current.speed>2 ) {
                        drawCar(playingGame.current,1,1);
                        playingGame.current.x-=playingGame.current.speed;
                        drawCar(playingGame.current,2,1);
                } else if(key == playingGame.rightKey && playingGame.current.x+playingGame.current.speed<93)
                {
                    drawCar(playingGame.current,1,1);
                    playingGame.current.x+=playingGame.current.speed;
                    drawCar(playingGame.current,2,1);
                }
            }
			if(key == ESC)
			{
				playingGame.IsGameRunning = false;
			}
        usleep(GAMESLEEPRATE);                             // 0.25 saniye bekleyin
    }
	pthread_join(enqueueThread, NULL);
	pthread_join(dequeueThread, NULL);
	
}




void *moveCar(void *data) {
    srand(time(NULL));
    bool isCarExit = false;
	bool isCarCrash = false;
        while (playingGame.IsGameRunning || (!isCarExit && !isCarCrash)) {
            Car *currentCar = (Car *)data;
            // Aracın yolu terk ettiğinde
                drawCar(*currentCar, 1, 0); // Aracı sil
                currentCar[0].y += 1 + rand() % currentCar[0].speed;

                if (currentCar[0].y >= EXITY) {
                    playingGame.points += currentCar[0].height * currentCar[0].width;
                    isCarExit = true;
                    break;
                }
				bool isCrashY = (playingGame.current.y < currentCar[0].y + currentCar[0].height) && (playingGame.current.y > currentCar[0].y);
				bool isCrashXL = (playingGame.current.x < currentCar[0].x + currentCar[0].width) && (playingGame.current.x > currentCar[0].x);
				bool isCrashXR = (playingGame.current.x + playingGame.current.width > currentCar[0].x) && (playingGame.current.x < currentCar[0].x);

				if(isCrashY && (isCrashXL || isCrashXR))
				{
					isCarCrash = true;
					playingGame.IsGameRunning = false;
					break;
				}

                drawCar(*currentCar, 2, 0);
				usleep(playingGame.moveSpeed - currentCar[0].speed);
            }
		
}

// Dequeue
void *Dequeue(void *) {
    srand(time(NULL));
	int i = 0;
	pthread_t moveProcess[10];
    while (playingGame.IsGameRunning) { // Oyun devam ettiği sürece
        sleep((rand() % 2) + 2);
        if (!playingGame.cars.empty()) {
            Car *currentCar = new Car(playingGame.cars.front());
            playingGame.cars.pop();
            pthread_create(moveProcess+i, NULL, moveCar,(void *) currentCar);
        }
		i++;
    }
	while(i > 0)
	{
		pthread_cancel(moveProcess[i]);
		i--;
	}
}


// add cars to queue
void *enqueue(void *) {
    srand(time(NULL));
    while (playingGame.IsGameRunning) { // Oyun devam ettiği sürece
        if (playingGame.cars.size() < maxCarNumber) { // Kuyrukta maksimum araç sayısına ulaşmadıysa
            Car newCar;
            newCar.ID = playingGame.counter;
            newCar.height = create_car_height();
            newCar.width = create_car_width();
            newCar.x = create_car_x(newCar.width);
            newCar.y = create_car_y();
            newCar.speed = create_car_speed(newCar.height);
            newCar.clr = create_car_clr();
            newCar.chr = create_car_type();
            playingGame.counter += 1;
            if (playingGame.counter == IDMAX)
                playingGame.counter = IDSTART;

            playingGame.cars.push(newCar); // Oluşturulan aracı kuyruğa ekle

            sleep(1); // 2-4 saniye arasında beklet
        }
    }
}



void drawCar(Car c, int type, int direction )
{
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)  // oyun bitmediyse ve kaydetme týklanmadýysa
    {
            init_pair(c.ID, c.clr, 0);                       // renk çiftini baþlat
            attron(COLOR_PAIR(c.ID));                        // renk çiftini etkinleþtir
            char drawnChar;

            if (type == 1 )
               drawnChar = ' ';                              // aracý kaldýrmak için boþluk karakteri
            else
               drawnChar= c.chr;
                                          // aracý çizmek için karakter
            mvhline(c.y, c.x, drawnChar, c.width);           // dikdörtgenin üst çizgisi
            mvhline(c.y + c.height - 1, c.x, drawnChar, c.width); // dikdörtgenin alt çizgisi

            if(direction == 0) // Eðer yolda baþka bir araç varsa
                mvhline(c.y + c.height, c.x, drawnChar, c.width);
            else // oyuncunun aracý
                mvhline(c.y -1, c.x, drawnChar, c.width);
            mvvline(c.y, c.x, drawnChar, c.height);          // dikdörtgenin sol çizgisi
            mvvline(c.y, c.x + c.width - 1, drawnChar, c.height); // dikdörtgenin sað çizgisi
            char text[5];
            if (type == 1 )
                sprintf(text,"  ");                          // puaný kaldýrmak için boþluk karakteri
            else

            sprintf(text,"%d",c.height * c.width);      // dikdörtgenin puanýný göstermek için
            mvprintw(c.y+1, c.x +1, text);              // dikdörtgenin puanýný ekrana yazdýr
            attroff(COLOR_PAIR(c.ID));                   // renk çiftini devre dýþý býrak
    }
}





void printWindow()
{
    for (int i = 1; i < wHeight - 1; ++i) {
        mvprintw(i, 2, "*");                                 // yolun sol tarafına çiz
        mvprintw(i, 0, "*");
        mvprintw(i, wWidth - 1, "*");                        // yolun sag tarafýný çiz
        mvprintw(i, wWidth - 3, "*");
    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) {      // yolun ortasına çizgiyi çiz
        mvprintw(i, lineX, "#");
    }
       // Ağaçları çiz
    attron(COLOR_PAIR(1));
    char tree[4] = "";
    for(int i=1;i<=3;i++)
    {
        for(int j=1;j<=i;j++)
        {
            mvaddch(i,wWidth+5+j,'*');
        }

    }
}

int create_car_x(int width)
{
	int number, n;
    n = 85; //Max is 89
	do{
		number = (rand() % n) + 5;
	}while((number > lineX - width) && (number < lineX + width));
	
    return number;
}

int create_car_y()
{
    return (rand() % 10) - 10;
}

int create_car_height()
{	
    return (rand() % 3) + 5;
}

int create_car_width()
{
	return (rand() % 3) + 5;
}

int create_car_speed(int height)
{
	return height / 2;
}

int create_car_clr()
{
	return (rand() % numOfcolors) + 1;
}

char create_car_type()
{
	int randomType = (rand() % numOfChars) + 1;
    return randomType == 1 ? '*' : randomType == 2 ? '#' : '+';
}
