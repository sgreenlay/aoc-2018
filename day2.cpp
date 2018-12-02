
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
    auto values = valuesFromFile("data/day2.txt");

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

    {
        std::string common;

        for (int i = 0; i < values.size(); ++i)
        {
            auto a = values[i];
            for (int j = i + 1; j < values.size(); ++j)
            {
                auto b = values[j];

                int differences = 0;
                for (int k = 0; k < b.size(); ++k)
                {
                    if (a[k] != b[k])
                    {
                        differences++;
                    }
                }

                if (differences == 1)
                {
                    for (int k = 0; k < b.size(); ++k)
                    {
                        if (a[k] == b[k])
                        {
                            common.push_back(a[k]);
                        }
                    }
                    break;
                }
            }

            if (common.size() > 0)
            {
                break;
            }
        }

        printf("Common Subsequence: %s\n", common.c_str());
    }

    std::getc(stdin);
}