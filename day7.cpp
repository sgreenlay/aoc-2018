
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <map>
#include <set>
#include <queue>

struct step
{
    char label;
    char next;
};

step stepFromString(const std::string string)
{
    auto start = string.find("Step ") + 5;
    auto end = string.find(" ", start);

    auto label = string.substr(start, end - start);

    start = string.find("step ") + 5;
    end = string.find(" ", start);

    auto next = string.substr(start, end - start);

    return step{
        label[0],
        next[0]
    };
}

std::vector<step> stepsFromFile(const char * path)
{
    std::vector<step> steps;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        steps.push_back(stepFromString(line));
    }
    input.close();

    return steps;
}

int main()
{
    auto steps = stepsFromFile("data/day7.txt");

    std::map<char, std::set<char>> requirements;
    for (auto && step : steps)
    {
        if (requirements.count(step.label) == 0)
        {
            requirements[step.label] = std::set<char>();
        }
        requirements[step.next].emplace(step.label);
    }

    std::string visited;
    std::priority_queue<char, std::vector<char>, std::greater<char>> potentials;

    while (!requirements.empty())
    {
        std::vector<char> remove;
        for (auto && requirement : requirements)
        {
            if (requirement.second.empty())
            {
                potentials.emplace(requirement.first);
                remove.push_back(requirement.first);
            }
        }

        for (auto && c : remove)
        {
            requirements.erase(c);
        }

        char c = potentials.top();
        visited.push_back(c);
        potentials.pop();

        for (auto && requirement : requirements)
        {
            if (requirement.second.count(c) > 0)
            {
                requirement.second.erase(c);
            }
        }
    }

    printf("%s\n", visited.c_str());

    std::getc(stdin);
}