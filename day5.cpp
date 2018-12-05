
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <regex>

std::string polymerFromFile(const char * path)
{
    std::ifstream input;
    input.open(path);

    std::string line;
    std::getline(input, line);
    input.close();

    return line;
}

int main()
{
    auto polymer = polymerFromFile("data/day5.txt");

    {
        const int caps_diff = 'a' - 'A';

        int i = 0;
        int j = 1;

        while (j < polymer.length())
        {
            /*
            for (int k = 0; k < polymer.length(); ++k)
            {
                if (k == i)
                {
                    printf("[%c]", polymer[k]);
                }
                else if (k == j)
                {
                    printf("<%c>", polymer[k]);
                }
                else
                {
                    printf("%c", polymer[k]);
                }
            }
            printf("\n");
            */

            char a = polymer[i];
            char b = polymer[j];

            if (abs(a - b) == caps_diff)
            {
                printf("Remove %c%c\n", a, b);

                polymer[i] = '0';
                polymer[j] = '0';

                j = j + 1;
                while (polymer[i] == '0')
                {
                    if (i == 0)
                    {
                        i = j;
                        j = i + 1;

                        break;
                    }
                    else
                    {
                        i--;
                    }
                }
            }
            else
            {
                i = j;
                j = i + 1;
            }
        }

        int count = 0;
        for (int k = 0; k < polymer.length(); ++k)
        {
            if (polymer[k] != '0')
            {
                printf("%c", polymer[k]);
                count++;
            }
        }
        printf(", Count: %d\n", count);
    }

    std::getc(stdin);
}