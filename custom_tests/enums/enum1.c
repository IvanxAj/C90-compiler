enum Color {
    RED = 1,
    GREEN,
    BLUE
};

enum Size {
    SMALL = 10,
    MEDIUM,
    LARGE = 20
};

int f() {
    enum Color color = RED;
    enum Size size = LARGE;

    int result = color * size + GREEN - MEDIUM;

    return result;
}
