int f() {
    int arr[5];
    int i;
    int result = 0;

    for(i = 1; i <= 5; i++) {
        arr[i - 1] = i;
    }

    i = 3;

    result = result + arr[i + 0];

    return result;
}
