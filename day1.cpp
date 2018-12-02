
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <set>

std::vector<int> valuesFromFile(const char * path)
{
    std::vector<int> values;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        values.push_back(std::stoi(line));
    }
    input.close();

    return values;
}

int main()
{
    auto values = valuesFromFile("day1.txt");

    {
        int sum = 0;
        for (auto && value : values)
        {
            sum += value;
        }
        printf("Sum: %d\n", sum);
    }

    {
        int sum = 0;

        std::set<int> seen;
        seen.emplace(sum);

        int i = 0;
        while (true)
        {
            if (i == values.size())
            {
                i = 0;
            }

            sum += values[i];
            if (seen.count(sum) != 0)
            {
                break;
            }
            seen.emplace(sum);

            i++;
        }

        printf("First Duplicate Sum: %d\n", sum);
    }

    std::getc(stdin);
}