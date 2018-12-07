
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

    {
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

        printf("Part 1: %s\n", visited.c_str());
    }

    {
        std::map<char, std::set<char>> requirements;
        for (auto && step : steps)
        {
            if (requirements.count(step.label) == 0)
            {
                requirements[step.label] = std::set<char>();
            }
            requirements[step.next].emplace(step.label);
        }

        int elf_count = 5;
        int base_cost = 60;

        struct work {
            char ch;
            int start;
            int duration;

            int total() const {
                return start + duration;
            }
        };

        std::string visited;
        std::vector<work> elves;

        int clock = 0;
        while (!requirements.empty())
        {
            std::vector<char> remove;
            for (auto && requirement : requirements)
            {
                if (requirement.second.empty())
                {
                    int cost = requirement.first - 'A' + 1 + base_cost;

                    elves.push_back(work{ requirement.first, clock, cost });

                    remove.push_back(requirement.first);

                    //printf("%d : Elf started %c with cost %d\n", clock, requirement.first, cost);

                    if (elves.size() == elf_count)
                    {
                        break;
                    }
                }
            }

            for (auto && c : remove)
            {
                requirements.erase(c);
            }

            std::sort(elves.begin(), elves.end(), [](auto && a, auto && b) {
                return a.total() < b.total();
                });

            auto elf = elves[0];
            clock += elf.total() - clock;

            while (!elves.empty())
            {
                elf = elves[0];

                if (elf.total() > clock)
                {
                    break;
                }

                elves.erase(elves.begin());

                //printf("%d : Elf completed %c\n", clock, elf);
                visited.push_back(elf.ch);

                for (auto && requirement : requirements)
                {
                    if (requirement.second.count(elf.ch) > 0)
                    {
                        requirement.second.erase(elf.ch);
                    }
                }
            }
        }

        printf("Part 2: %s\n", visited.c_str());
        printf("  Takes %d elves %d seconds\n", elf_count, clock);
    }

    std::getc(stdin);
}