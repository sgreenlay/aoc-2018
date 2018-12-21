#include <stdio.h>

#include <set>

int main()
{
    int a = 0;
    int b = 0;
    int c = 0;
    int e = 0;
    int f = 0;

    std::set<int> seen;
    int last_e = 0;

    do
    {
        b = e | 65536;
        e = 678134;
    
        while (true)
        {
            e = e + (b & 255);
            e = e & 16777215;
            e = e * 65899;
            e = e & 16777215;

            if (256 > b) {
                goto line_28;
            }

            b = b / 256;
        }

    line_28:

        if (seen.empty())
        {
            printf("%d\n", e);
        }
        if (seen.count(e))
        {
            printf("%d\n", last_e);
            break;
        }
        seen.emplace(e);
        last_e = e;
    }
    while (e != a);
}