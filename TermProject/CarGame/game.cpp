#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>

// Ekranýn boyutlarý
#define wHeight 40 // Yolun yüksekliði
#define wWidth 100 // Yolun geniþliði

// Orta çizgi ve bitiþ koordinatlarý
#define lineX 45 // Ortadaki çizginin x koordinatý
#define lineLEN 10 // Orta çizginin baþlangýc ve sonundan uzaklýðý
#define EXITY 35 // Yolun bitis koordinatý

// Klavye girdileri
#define leftKeyArrow 260 // Sol ok tuþunun ASCII kodu
#define RightKeyArrow 261 // Sað ok tuþunun ASCII kodu
#define leftKeyA 97 // A'nýn ASCII kodu
#define RightKeyD 100 // D'nin ASCII kodu
#define ESC 27 // ESC tuþunun ASCII kodu
#define ENTER 10 // ENTER tuþunun ASCII kodu
#define KEYPUP 259 // Yukarý ok tuþunun ASCII kodu
#define KEYDOWN 258 // Aþaðý ok tuþunun ASCII kodu
#define KEYERROR -1 // Yanlýþ bir tuþa basýldýðýnda döndürülen ASCII kodu
#define SAVEKEY 115 // S'nin ASCII kodu

// Seviye ve hýz sýnýrlarý
#define levelBound 300 // 300 puaný geçince seviyeyi artýrmak için
#define MAXSLEVEL 5 // Maksimum seviye
#define ISPEED 500000 // Oyun hareket hýzý için baþlangýç deðeri
#define DRATESPEED 100000 // Her yeni seviyeden sonra hareket hýzýný azaltmak için

// Araba özellikleri
#define MINX 5 // Arabalarýn oluþturulurken minimum x koordinat deðeri
#define MINY 10 // Arabalarýn oluþturulurken maksimum y koordinat deðeri
#define MINH 5 // Arabalarýn oluþturulurken minimum yükseklik deðeri
#define MINW 5 // Arabalarýn oluþturulurken minimum geniþlik deðeri
#define SPEEDOFCAR 3 // Oyuncu tarafýndan kullanýlan arabanýn hýzý
#define YOFCAR 34 // Oyuncu tarafýndan kullanýlan arabanýn y koordinatý
#define XOFCAR 45 // Oyuncu tarafýndan kullanýlan arabanýn x koordinatý
#define IDSTART 10 // Araba kimliði için baþlangýç deðeri
#define IDMAX 20 // Araba kimliði için maksimum deðer
#define COLOROFCAR 3 // Oyuncu tarafýndan kullanýlan arabanýn renk deðeri
#define POINTX 91 // Puanýn yazýldýðý x koordinatý
#define POINTY 42 // Puanýn yazýldýðý y koordinatý

// Menü özellikleri
#define MENUX 10 // Menülerin baþlangýç satýrý için x koordinatý
#define MENUY 5 // Menülerin baþlangýç satýrý için y koordinatý
#define MENUDIF 2 // Menü satýrlarý arasýndaki fark
#define MENUDIFX 20 // Menü sütunlarý arasýndaki fark
#define MENSLEEPRATE 200000 // Menü giriþ için bekleme süresi
#define GAMESLEEPRATE 250000 // Oyuncu ok tuþlarý için bekleme süresi

// Diðer sabitler
#define EnQueueSleep 1 // EnQueue bekleme süresi
#define DeQueueSleepMin 2 // DeQueue minimum bekleme süresi
#define numOfcolors 4 // Arabalar için seçilebilecek maksimum renk deðeri
#define maxCarNumber 5 // Kuyruktaki maksimum araba sayýsý
#define numOfChars 3 // Arabalar için seçilebilecek maksimum desen sayýsý
#define settingMenuItem 2 // Ayarlar menüsündeki seçenek sayýsý
#define mainMenuItem 6 // Ana menüdeki seçenek sayýsý
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
int main()
{
    playingGame.leftKey = leftKeyArrow;      // Oyuncunun aracýný sola yönlendirmek için atanmýþ sol ok tuþu
    playingGame.rightKey = RightKeyArrow;    // Oyuncunun aracýný saða yönlendirmek için atanmýþ sað ok tuþu

    initGame();
    initWindow();
    printMenu();


     pthread_t th2; // Yeni bir iþ parçacýðý oluþtur

     pthread_create(&th2, NULL, newGame, NULL); // newGame fonksiyonunu bir iþ parçacýðýnda çalýþtýr
     pthread_join(th2, NULL); // Ýþ parçacýðýnýn bitmesini bekleyin, newGame fonksiyonu bittiðinde iþ parçacýðý da sona erecektir.

    return 0;                               // Programý normal þekilde sonlandýr
}


void printMenu()
{
    int selectedItem = 0; // Seçili menü öğesinin indisini saklar
    int key = -1; // Klavyeden alınacak tuş değeri için değişken

    while (playingGame.IsGameRunning) // ENTER tuşuna basılmadığı sürece devam et
    {
        //clear();
        // Menü öğelerini ekrana yazdır
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
            switch(selectedItem)
            {
                case 0:
                        pthread_create(&th2, NULL, newGame, NULL); // newGame fonksiyonunu bir iþ parçacýðýnda çalýþtýr
                        pthread_join(th2, NULL);
                    break;
                case 2:
                        printInstructor();
                    break;
                case 3:
                     printSettings();
                    break;
                case 4:
                         initWindow();
                         printPoint();
                        break;
                case 5:
                    playingGame.IsGameRunning=false;
                    break;
            }
            clear();
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
    printWindow();                                          // yolun çizilmesini baþlat
    drawCar(playingGame.current,2,1);                       // oyuncunun kullandýðý aracý ekrana çiz
    int key;
    while (playingGame.IsGameRunning && key!=ESC) {                     // oyun sona erene kadar devam et



            pthread_t th2;
            pthread_create(&th2, NULL, generateCar, NULL);
            key = getch();

            if (key != KEYERROR) {
                 if (key == playingGame.leftKey) {          // sol ok tuþu basýldýðýnda
                        drawCar(playingGame.current,1,1);  // oyuncunun aracýný ekrandan kaldýr
                        playingGame.current.x-=playingGame.current.speed; // pozisyonu güncelle
                        drawCar(playingGame.current,2,1);  // yeni pozisyonla oyuncunun aracýný çiz
                }else if(key == playingGame.rightKey)
                {
                    drawCar(playingGame.current,1,1);  // oyuncunun aracýný ekrandan kaldýr
                    playingGame.current.x+=playingGame.current.speed; // pozisyonu güncelle
                    drawCar(playingGame.current,2,1);  // yeni po
                }
            }
         usleep(GAMESLEEPRATE);                             // 0.25 saniye bekleyin
        }
}

void *moveCar(void *car)
{
    srand(time(NULL));
    int speed = 1 + rand() % car.speed;


}
void *generateCar(void *)
{
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // kırmızı rengi aktifleştirir
    init_pair(4, COLOR_BLUE, COLOR_BLACK); // Yeşil rengi aktifleştirir

         Car newCar;
        while(playingGame.cars.size()< 5)
        {
         newCar.clr = 2;
         newCar.ID =31;
         newCar.x = 10;
         newCar.y=  7;
         newCar.height = 8;
         newCar.width=7;
         newCar.speed = newCar.height / 2;
         newCar.chr = '#';
         drawCar(newCar,2,2);
         sleep(1);
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
               drawnChar= c.chr;                             // aracý çizmek için karakter
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
            mvprintw(c.y+1, c.x +1, text);                    // dikdörtgenin puanýný ekrana yazdýr
            attroff(COLOR_PAIR(c.ID));                        // renk çiftini devre dýþý býrak
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
