int g(int a, int b, int c, int d, int e, int f, int g, int h, int i, int l, int m)
{
    int y = a + e + l + m;
    return y;
}

int f() {
    int a = 1;
    int b = 2;
    int c = g(1,2,3, 4, 5, 6, 7, 8, 9, a, b);
    return c;
}
