#include <stdio.h>

int main()
{
    int a = 0;
    int b = 0;
    int c = 0;
    int e = 0;
    int f = 0;

    e = 123;
line_1:
    e = e & 456;
line_2:
    if (e == 72) {
        goto line_5;
    }
    else {
        goto line_1;
    }

line_5:
    e = 0;
line_6:
    b = e | 65536;
    e = 678134;
line_8:
    f = b & 255;
    e = e + f;
    e = e & 16777215;
    e = e * 65899;
    e = e & 16777215;
line_13:
    if (256 > b) {
        goto line_28;
    }
    else {
        goto line_17;
    }

line_17:
    f = 0;
line_18:
    c = f + 1;
    c = c * 256;
    if (c > b) {
        goto line_26;
    }
    else {
        goto line_24;
    }

line_24:
    f = f + 1;
    goto line_18;
line_26:
    b = f;
    goto line_8;

line_28:

    printf("%d\n", e);

    if (e == a) {
        return 0;
    }
    else {
        goto line_6;
    }
}