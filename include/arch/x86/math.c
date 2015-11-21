long power(long number, int power) {
    long tmp = number;
    for (int i = 1; i < power; i++) {
        tmp *= number;
    }

    return tmp;
}
