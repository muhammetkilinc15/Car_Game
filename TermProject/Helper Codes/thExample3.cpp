#include <iostream>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
using namespace std;
int counter = 0;
const char *fileName = "log.txt";
pthread_mutex_t mutexFile;
void *increase(void *)
{
    for(int i = 0;i <20;i++)
    {
        counter++;
        pthread_mutex_lock(&mutexFile);
        FILE *file = fopen(fileName,"a+");
        fprintf(file,"increase counter: %d\n",counter);
        fclose(file);
        printf("Thread increase wrote...\n");
        pthread_mutex_unlock(&mutexFile);
        sleep(1);
    }
    printf("Exiting from increase thread\n");
    return 0;
}
void *decrease(void *)
{
    for(int i = 0;i <10;i++)
    {
        counter--;
        pthread_mutex_lock(&mutexFile);
        FILE *file = fopen(fileName,"a+");
        fprintf(file,"decrease counter: %d\n",counter);
        fclose(file);
        printf("Thread decrease wrote...\n");
        pthread_mutex_unlock(&mutexFile);
        sleep(3);
    }
    printf("Exiting from decrease thread\n");
    return 0;
}
int main() {
    pthread_t th1, th2;
    pthread_create(&th1, NULL, increase,NULL);
    pthread_create(&th2, NULL, decrease,NULL);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    printf("Exiting from main\n");
    return 0;
}
