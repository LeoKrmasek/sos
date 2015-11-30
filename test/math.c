#include <arch/x86/math.h>
#include <tap.h>

int main() {
    plan(2);
    int p = power(2, 4);
    ok(p == 16, "power(2, 4), expected %d, was %d", 16, p);

    int p2 = power(10, 0);
    ok(p2 == 1, "power(10, 0), expected %d, was %d", 1, p2);
}
