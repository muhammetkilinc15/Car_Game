#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
using namespace std;

int main()
{
    srand(time(NULL));
    int number, n = 100;
    for(int i = 0; i < 5;i++)
    {
        number = rand() % n;
        printf("%d\n",number);
    }
    return 0;
}
