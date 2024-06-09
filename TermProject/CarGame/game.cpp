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


void printMenu(); //Printing the menu and get an input
void printInstructor(); //Print how to play the game
void printSettings(); //Shows and change the different input types for game
void printPoint(); //Shows point of the last games
void *enqueue(void *); //Create new car with random parameters and add to queue
void *dequeue(void *); //Generate car to road from queue
void *moveCar(void *data); //Control that movement and crash of a car
int create_car_x(int width); //Generate x coordinate of a car
int create_car_y(); //Generate y coordinate of a car
int create_car_height(); //Generate height of a car
int create_car_width(); //Generate width of a car
int create_car_speed(int height); //Generate speed of a car
int create_car_clr(); //Generate color of a car
char create_car_type(); //Generate type of a car
void printSingleLine(int y, int x, const char *text, bool selected); //Control the selected menu items colors
void saveCar(Car c); //Save car information to text file
void saveGame(); //Save game informations to text file
void loadGame(); //Load game informations from text files and create new game with theese
void initFiles(); //Empty old files
void printTree(); //Print the trees
void printPlayerPoint(int point); //Print player current point on the screen
void savePoint(); //Save points to text file

int main()
{
    playingGame.leftKey = leftKeyArrow; //Initialize game control keys
    playingGame.rightKey = RightKeyArrow;
    initGame(); //Initialize game settings and values
    initWindow(); //Iinitalize new window
    printMenu();
    return 0;
}
void *newGame(void *)
{
    printWindow();
	initFiles();
    drawCar(playingGame.current,2,1);
    int key; //Input key
    pthread_t enqueueThread , dequeueThread;
    pthread_create(&enqueueThread, NULL, enqueue, NULL); //Create thread for enqueue and dequeue processes
    pthread_create(&dequeueThread, NULL, dequeue, NULL);
	printPlayerPoint(playingGame.points);
    while (playingGame.IsGameRunning) { //Continue until game finish
        key = getch();
        if (key != KEYERROR)
            {
				int l_limit = playingGame.current.x - playingGame.current.speed;
				int r_limit = playingGame.current.x + playingGame.current.speed;
                if (key == playingGame.leftKey && l_limit > 2 ) //If next positions will exit the window do not enter the if statement
                {
                        drawCar(playingGame.current,1,1);
                        playingGame.current.x = l_limit;
                        drawCar(playingGame.current,2,1);
                }
                else if(key == playingGame.rightKey && r_limit<93)
                {
                    drawCar(playingGame.current,1,1);
                    playingGame.current.x = r_limit;
                    drawCar(playingGame.current,2,1);
                }
				else if(key == ESC) //If player press ESC, do nlot save and exit
				{
					playingGame.IsGameRunning = false;
				}
				else if(key == SAVEKEY)
				{
                    playingGame.IsGameRunning = false;
                    playingGame.IsSaveCliked = true;
                    saveGame();
				}
			}

			if((playingGame.points >= (playingGame.level * levelBound)) && (playingGame.level < MAXSLEVEL))  //Increase the level every 300 points
			{
				playingGame.level += 1;
				playingGame.moveSpeed -= DRATESPEED;

			}
        refresh();
        usleep(GAMESLEEPRATE);
    }
	pthread_join(enqueueThread, NULL);
	pthread_join(dequeueThread, NULL); //Wait the enqueue and dequeue processes
	savePoint();
}

void printTree() {
    attron(COLOR_PAIR(1));

    int x = wWidth + 5;
    int y = 5 ;

    for (int i = 0; i < 3; i++) {
        // Ağaç gövdesini çiz
        attron(COLOR_PAIR(1));
        mvprintw(y, x, "*");
        mvprintw(y + 1, x - 1, "*");
        mvprintw(y + 1, x + 1, "*");
        mvprintw(y + 2, x - 2, "*");
        mvprintw(y + 2, x, "*");
        mvprintw(y + 2, x + 2, "*");
        attron(COLOR_PAIR(2));
        mvprintw(y+3 , x , "#");
        mvprintw(y+4 , x , "#");
        y += 10;
    }

}

void saveCar(Car c)
{
	pthread_mutex_lock(&playingGame.mutexFile); //Lock the file for moveCar threads
	FILE *carsFile = fopen(CarsTxt,"ab+");
	fwrite(&c, sizeof(Car), 1, carsFile);
	fclose(carsFile);
	pthread_mutex_unlock(&playingGame.mutexFile);
}

void saveGame()
{
	FILE *gameFile = fopen(gameTxt,"ab+");
	fwrite(&playingGame, sizeof(Game), 1, gameFile);
	fclose(gameFile);
}

void savePoint()
{
	FILE *pointFile = fopen(pointsTxt,"a+");
	fprintf(pointFile,"%d\n",playingGame.points);
	fclose(pointFile);
}

void initFiles()
{
	FILE *file = fopen(CarsTxt, "w");
	fclose(file);
	FILE *file2 = fopen(gameTxt, "w");
	fclose(file2);
}

void loadGame()
{

	FILE *file2 = fopen(gameTxt,"rb+");
	fread(&playingGame, sizeof(Game), 1, file2);
	fclose(file2);
    playingGame.cars = queue<Car>();
    pthread_mutex_init(&playingGame.mutexFile, NULL);
	playingGame.IsGameRunning = true;
	playingGame.IsSaveCliked = false;
    sleep(1);

	FILE *file = fopen(CarsTxt,"rb+");
	Car current;
	fread(&current, sizeof(Car), 1, file);
	while(!feof(file))
	{
		playingGame.cars.push(current);
		fread(&current, sizeof(Car), 1, file);
	}
	fclose(file);
}

//This method runs within its own thread. So each car move the independently

void *moveCar(void *data) {
    srand(time(NULL));
	bool isCarCrash = false;
	Car *currentCar = (Car *)data;
        while (playingGame.IsGameRunning)
            {
                drawCar(*currentCar, 1, 0);
                currentCar[0].y += 1 + rand() % currentCar[0].speed;
                if (currentCar[0].y >= EXITY)
                {
					int point = (currentCar[0].height * currentCar[0].width);
					printPlayerPoint(point);
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

            if(playingGame.IsSaveCliked)
                {
                    saveCar(currentCar[0]);
                    playingGame.IsGameRunning=false;
                }


}

void printPlayerPoint(int point)
{
    char text[50];
    playingGame.points+=point;
    sprintf(text,"Point : %d - Size: %d",playingGame.points,playingGame.cars.size());
    mvprintw(POINTY, POINTX, text);


}


//This method dequeues a car from the queue with a random interval between 2 to 4 seconds

void *dequeue(void *) {
    srand(time(NULL));
	pthread_t moveProcess[10];
    while (playingGame.IsGameRunning) { // Oyun devam ettiği sürece
        if (!playingGame.cars.empty()) {
            Car *currentCar = new Car(playingGame.cars.front());
            playingGame.cars.pop();
            pthread_create(moveProcess, NULL, moveCar,(void *) currentCar);
        }
        sleep((rand() % 2) + 2);
    }

}





// This method enqueues a car into the queue every 1 seconds
void *enqueue(void *) {
    srand(time(NULL));
    while (playingGame.IsGameRunning)
    {
        if(playingGame.cars.size() < maxCarNumber)
        {
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
            {
                playingGame.counter = IDSTART;
            }
             playingGame.cars.push(newCar);
        }
        sleep(1);
    }
}



void printSingleLine(int y, int x, const char *text, bool selected) {
    if (selected) {
        attron(COLOR_PAIR(2)); // Kırmızı renk çiftini etkinleştir
        mvprintw(y, x - 2, "->");
        mvprintw(y, x, text); // Menü öğesini ekrana yazdır
        attroff(COLOR_PAIR(2)); // Kırmızı renk çiftini devre dışı bırak
    } else {
        mvprintw(y, x - 2, "  "); // Önceki işareti sil
        attron(COLOR_PAIR(1)); // Yeşil renk çiftini etkinleştir
        mvprintw(y, x, text); // Menü öğesini ekrana yazdır
        attroff(COLOR_PAIR(1)); // Yeşil renk çiftini devre dışı bırak
    }
}



void printMenu()
{
	endwin();
    int selectedItem = 0;
    int key = -1;

    while (true)
    {
        for (int i = 0; i < mainMenuItem; i++)
        {
            printSingleLine(MENUY + (i * MENUDIF), MENUX, mainMenu[i], i == selectedItem);
        }

        key = getch(); // Klavyeden giriş al
        if (key == KEYDOWN) // Aşağı ok tuşuna basıldığında
        {
            selectedItem = (selectedItem + 1) % mainMenuItem; // Seçili öğeyi bir sonraki öğeye taşı
        }
        else if (key == KEYPUP) // Yukarı ok tuşuna basıldığında
        {
            selectedItem = (selectedItem - 1 + mainMenuItem) % mainMenuItem; // Seçili öğeyi bir önceki öğeye taşı
        }else if(key ==ENTER)
        {
            endwin();
			initWindow();
            switch(selectedItem)
            {
                case 0:
						initGame();
						pthread_t th2;
                        pthread_create(&th2, NULL, newGame, NULL);
                        pthread_join(th2, NULL);
                    break;
                case 1:
						loadGame();

						pthread_t loadGameThread;
						pthread_create(&loadGameThread, NULL, newGame, NULL);
                        pthread_join(loadGameThread, NULL);
                        pthread_mutex_destroy(&playingGame.mutexFile);
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
                    return;

            }
			endwin();
            initWindow();
        }
        usleep(MENSLEEPRATE);
    }
}


void printInstructor()
{
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
    int selectedItem = 0;
    int key = -1;

    while (true)
    {
        for (int i = 0; i < settingMenuItem; i++)
        {
             printSingleLine(MENUY + (i * MENUDIF), MENUX, settingMenu[i], i == selectedItem);
        }

        key = getch();

        if (key == KEYDOWN)
        {
            selectedItem = (selectedItem + 1) % settingMenuItem;
        }
        else if (key == KEYPUP)
        {
            selectedItem = (selectedItem - 1 + settingMenuItem) % settingMenuItem;
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
            return;
        }
    }
}

void printPoint()
{
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
    init_pair(2, COLOR_RED, COLOR_BLACK); // kırmızı rengi aktifleştiri

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
    playingGame.current.chr = '*';
}


void drawCar(Car c, int type, int direction )
{
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)
    {
            init_pair(c.ID, c.clr, 0);
            attron(COLOR_PAIR(c.ID));
            char drawnChar;

            if (type == 1 )
               drawnChar = ' ';
            else
               drawnChar= c.chr;

            mvhline(c.y, c.x, drawnChar, c.width);
            mvhline(c.y + c.height - 1, c.x, drawnChar, c.width);

            if(direction == 0)
                mvhline(c.y + c.height, c.x, drawnChar, c.width);
            else
                mvhline(c.y -1, c.x, drawnChar, c.width);
            mvvline(c.y, c.x, drawnChar, c.height);
            mvvline(c.y, c.x + c.width - 1, drawnChar, c.height);
            char text[5];
            if (type == 1 )
                sprintf(text,"  ");
            else
            sprintf(text,"%d",c.height * c.width);
            mvprintw(c.y+1, c.x +1, text);
            attroff(COLOR_PAIR(c.ID));
    }
}





void printWindow()
{
    for (int i = 1; i < wHeight - 1; ++i) {
        mvprintw(i, 2, "*");
        mvprintw(i, 0, "*");
        mvprintw(i, wWidth - 1, "*");
        mvprintw(i, wWidth - 3, "*");
    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) {
        mvprintw(i, lineX, "#");
    }

    printTree();
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
