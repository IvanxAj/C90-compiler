int f()
{
    int i;
    int x[8];
    int acc;
    for(i=8; i<16; i++){
        x[i-8]=i;
        acc += x[i - 8];
    }
    return acc;
}
