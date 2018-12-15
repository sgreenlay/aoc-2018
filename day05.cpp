
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

const int caps_diff = 'a' - 'A';

int find_nonzero(const std::string s, int i, int d)
{
    if (i < 0 || i > s.length())
    {
        return i;
    }
    while (s[i] == '0')
    {
        if ((i == 0) && (d < 0))
        {
            return i;
        }
        if ((i == s.length() - 1) && (d > 0))
        {
            return i;
        }
        i += d;
    }
    return i;
}

int reactPolymer(std::string polymer)
{
    int i = find_nonzero(polymer, 0, +1);
    int j = find_nonzero(polymer, i + 1, +1);

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
            //printf("Remove %c%c\n", a, b);

            polymer[i] = '0';
            polymer[j] = '0';

            i = find_nonzero(polymer, i, -1);
            if (polymer[i] == '0')
            {
                i = find_nonzero(polymer, j, +1);
                j = i + 1;
            }
            j = find_nonzero(polymer, j, +1);
        }
        else
        {
            i = j;
            j = find_nonzero(polymer, i + 1, +1);
        }
    }

    int count = 0;
    for (int k = 0; k < polymer.length(); ++k)
    {
        if (polymer[k] != '0')
        {
            count++;
        }
    }

    return count;
}

int main()
{
    auto polymer = polymerFromFile("data/day5.txt");

    int count = reactPolymer(polymer);
    printf("Base polymer: %d\n", count);

    char ch = 'a';
    for (char C = 'A'; C <= 'Z'; ++C)
    {
        char c = C + caps_diff;

        std::string newPolymer = polymer;
        int removed = 0;
        for (int k = 0; k < newPolymer.length(); ++k)
        {
            if (newPolymer[k] == C || newPolymer[k] == c)
            {
                newPolymer[k] = '0';
                removed++;
            }
        }

        if (removed > 0)
        {
            int newCount = reactPolymer(newPolymer);
            if (newCount < count)
            {
                ch = C;
                count = newCount;
            }
        }
    }
    printf("Remove %c for count of %d\n", ch, count);

    std::getc(stdin);
}