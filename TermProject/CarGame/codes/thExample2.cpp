#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
using namespace std;
int counter = 0;
void *increase(void *)
{
    for(int i = 0;i <20;i++)
    {
        counter++;
        printf("increase counter: %d\n",counter);
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
        printf("decrease counter: %d\n",counter);
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
