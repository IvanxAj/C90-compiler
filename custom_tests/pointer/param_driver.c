int f(int *p);

int main()
{
    int a;
    int *p = &a;

    f(p);
    return (a != 100);
}
