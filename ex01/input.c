#include"stdio.h"

void input(){
    char str[100];
    printf("Enter what you want to enter:\n");
    gets(str);
    printf("You entered:%s\n",str);
}

int main(){
    input();
    return 0;
}