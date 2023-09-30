#include <stdio.h>
float f();

int main()
{
    float b = f();
    printf("%f\n", b);
    return !(b==9.0f);
}
