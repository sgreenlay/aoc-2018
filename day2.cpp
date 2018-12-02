
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <set>

std::vector<std::string> valuesFromFile(const char * path)
{
    std::vector<std::string> values;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        values.push_back(line);
    }
    input.close();

    return values;
}

int main()
{
    auto values = valuesFromFile("day2.txt");

    {
        int twoCount = 0;
        int threeCount = 0;

        for (auto && value : values)
        {
            int counts[26] = { 0 };
            for (char ch : value)
            {
                counts[ch - 'a']++;
            }

            bool hasTwoCount = false;
            bool hasThreeCount = false;
            for (int count : counts)
            {
                if (count == 2)
                {
                    hasTwoCount = true;
                }
                else if (count == 3)
                {
                    hasThreeCount = true;
                }
            }

            if (hasTwoCount)
            {
                twoCount++;
            }
            if (hasThreeCount)
            {
                threeCount++;
            }
        }

        printf("Score: %d\n", twoCount * threeCount);
    }

    std::getc(stdin);
}