int g(int a, int b, int c)
{
    int y = a + b + c;
    int z = 5 + y;
    return z;
}

int f() {
    int a = 1;
    int b = 2;
    int c = g(a,b,3);
    return c;
}
