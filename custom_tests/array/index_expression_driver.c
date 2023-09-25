#include <stdio.h>
int f();

int main()
{
    int a = f();
    printf("%d\n", a);
    return !(a==4);
}
