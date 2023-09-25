#include <stdio.h>
float f();

int main()
{
    float a = f();
    printf("%f\n", a);
    return !(a==2.0f);
}
