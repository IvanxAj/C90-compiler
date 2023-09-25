#include <stdio.h>
int f();

int main()
{
    int c = f();
    printf("%d\n", c);
    return !(c==92);
}
