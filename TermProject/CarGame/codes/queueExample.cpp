#include <iostream>
#include <queue>
using namespace std;
int main() {
    queue<int> integers;
    integers.push(5);
    integers.push(66);
    integers.push(3);
    printf("Size : %d\n",integers.size());
    printf("Front : %d\n",integers.front());
    printf("Back : %d\n",integers.back());
    integers.pop();
    printf("Size : %d\n",integers.size());
    printf("Front : %d\n",integers.front());
    printf("Back : %d\n",integers.back());
    integers = queue<int>();
    printf("Size : %d\n",integers.size());
    printf("Front : %d\n",integers.front());
    printf("Back : %d\n",integers.back());
    return 0;
}
