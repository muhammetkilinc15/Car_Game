#include <iostream>
#include <pthread.h>
#include <string.h>
using namespace std;

typedef struct params {
    char name[20];
    int number;
}params;
int c = 0;
void *funThreadSingleParam(void *data)
{
    int *number = (int *)data;
    *number = 25;
    c++;
    printf("c: %d\n",c);
    printf("Exiting from thread\n");
    return 0;
}
void *funThreadMultiParam(void *data)
{
    params *newP = (params *)data;
    newP->number = 15;
    strcpy(newP->name,"Ayana");
    printf("Exiting from thread worked with multi parameters\n");
    return 0;
}
int main() {
    int a = 5, b = 14;
    params p = {"Omer",3};
    pthread_t th1, th2, th3;
    pthread_create(&th1, NULL, funThreadSingleParam,(void *)&a);
    pthread_create(&th2, NULL, funThreadSingleParam, (void *)&b);
    pthread_create(&th3, NULL, funThreadMultiParam, (void *)&p);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);
    printf("Name: %s\n",p.name);
    printf("Number: %d\n",p.number);
    printf("a : %d\n",a);
    printf("b : %d\n",b);
    printf("c : %d\n",c);
    printf("Exiting from main program\n");
    return 0;
}
