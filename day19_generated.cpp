#include <stdio.h>

int main()
{
    bool part2 = true;

    int a = part2 ? 1 : 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int f = 0;

    f = f + 2;
    f = f * f;
    f = 19 * f;
    f = f * 11;
    c = c + 5;
    c = c * 22;
    c = c + 18;
    f = f + c;

    if (a == 1)
    {
        c = 27;
        c = c * 28;
        c = 29 + c;
        c = 30 * c;
        c = c * 14;
        c = c * 32;
        f = f + c;
        a = 0;
    }

    printf("Sum of factors of %d\n", f);

    if (part2)
    {
        printf("https://www.wolframalpha.com/input/?i=sum+of+factors+%d\n", f);
        return 0;
    }

    for (d = 1; d <= f; ++d)
    {
        for (b = 1; b <= f; b++)
        {
            c = d * b;

            if (c == f) {
                if (a != 0)
                {
                    printf(" + ");
                }

                a = d + a;

                printf("%d", d);
            }
        }
    }
    printf(" = %d \n", a);
}
