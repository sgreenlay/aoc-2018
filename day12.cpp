#include <fstream>
#include <sstream>
#include <string>

#include <vector>

struct growth_rule
{
    char rule[5];
    char result;

    bool matches(char state[5])
    {
        for (int i = 0; i < 5; ++i)
        {
            if (state[i] != rule[i])
            {
                return false;
            }
        }
        return true;
    }
};

struct config
{
    std::string initial_state;
    std::vector<growth_rule> rules;
};

config configFromFile(const char * path)
{
    config cfg;

    std::ifstream input;
    input.open(path);

    int label = 0;
    std::string line;

    std::getline(input, line);

    auto start = line.find(": ") + 2;
    cfg.initial_state = line.substr(start);

    while (std::getline(input, line))
    {
        if (line.size() == 0)
            continue;

        growth_rule rule;
        for (int i = 0; i < 5; ++i)
        {
            rule.rule[i] = line[i];
        }
        rule.result = line[9];

        if (rule.result == '#')
        {
            cfg.rules.push_back(rule);
        }
    }
    input.close();

    return cfg;
}

int main()
{
    auto cfg = configFromFile("data/day12.txt");

    {
        int start = 0;
        std::string state = cfg.initial_state;
        //printf("0: %s\n", state.c_str());

        for (int generation = 1; generation <= 20; ++generation)
        {
            std::string next_state;
            int i = -2;

            while (i < (int)state.size() + 2)
            {
                char local_state[5];

                for (int j = -2; j <= 2; ++j)
                {
                    int idx = i + j;
                    if ((idx < 0) || (idx >= state.size()))
                    {
                        local_state[j + 2] = '.';
                    }
                    else
                    {
                        local_state[j + 2] = state[idx];
                    }
                }

                bool found_match = false;
                for (auto && rule : cfg.rules)
                {
                    if (rule.matches(local_state))
                    {
                        found_match = true;
                        next_state.push_back(rule.result);

                        if (i < 0)
                        {
                            start += i;
                        }

                        break;
                    }
                }

                if (!found_match && (i >= 0) && (i < state.size()))
                {
                    next_state.push_back('.');
                }

                ++i;
            }

            //printf("%d: [%d] %s [%d]\n", generation, start, next_state.c_str(), start + next_state.size());
            state = next_state;
        }

        int sum = 0;
        for (int i = 0; i < state.size(); ++i)
        {
            if (state[i] == '#')
            {
                sum += i + start;
            }
        }

        printf("\nSum at 20: %d\n", sum);
    }

    {
        int start = 0;
        std::string state = cfg.initial_state;
        //printf("0: %s\n", state.c_str());

        unsigned long long generation = 1;
        unsigned long long max_generation = 50000000000;

        unsigned long long last_sum = 0;

        while (generation <= max_generation)
        {
            std::string next_state;

            int i = -2;
            int next_start = start + i;

            bool found_plant = false;

            while (i < (int)state.size() + 2)
            {
                char local_state[5];

                for (int j = -2; j <= 2; ++j)
                {
                    int idx = i + j;
                    if ((idx < 0) || (idx >= state.size()))
                    {
                        local_state[j + 2] = '.';
                    }
                    else
                    {
                        local_state[j + 2] = state[idx];
                    }
                }

                char ch = '.';
                for (auto && rule : cfg.rules)
                {
                    if (rule.matches(local_state))
                    {
                        found_plant = true;
                        ch = rule.result;
                        break;
                    }
                }

                if (!found_plant && (ch == '.'))
                {
                    next_start++;
                }
                else
                {
                    next_state.push_back(ch);
                }

                ++i;
            }

            unsigned long long sum = 0;
            for (int i = 0; i < next_state.size(); ++i)
            {
                if (next_state[i] == '#')
                {
                    sum += i + next_start;
                }
            }

            next_state.erase(next_state.find_last_of('#') + 1);
            //printf("%llu: [%d] %s [%d]\n", generation, next_start, next_state.c_str(), start + next_state.size());
            //printf("Sum: %llu\n\n", sum);

            if (next_state.compare(state) == 0)
            {
                //printf("Shifting of %d from previous generation", next_start - start);

                if (last_sum == 0)
                {
                    last_sum = sum;
                    printf("\n");
                }
                else
                {
                    unsigned long long diff = sum - last_sum;
                    //printf(" with diff of %lld\n", diff);

                    unsigned long long projected_sum = sum + diff * (max_generation - generation);
                    printf("Projected sum at %llu of %llu\n", max_generation, projected_sum);

                    break;
                }
            }

            state = next_state;
            start = next_start;

            generation++;
        }
    }

    //

    std::getc(stdin);
}
