
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <set>
#include <map>
#include <algorithm>

long long min(long long a, long long b)
{
    if (a < b) return a;
    return b;
}

long long max(long long a, long long b)
{
    if (a > b) return a;
    return b;
}

struct nanobot
{
    long long X;
    long long Y;
    long long Z;
    unsigned long long R;

    unsigned long long distance_to(const nanobot& other)
    {
        long long dx = std::abs(other.X - X);
        long long dy = std::abs(other.Y - Y);
        long long dz = std::abs(other.Z - Z);

        return dx + dy + dz;
    }
};

nanobot nanobotFromString(std::string string)
{
    auto start = string.find("<") + 1;
    auto end = string.find(",", start);

    auto x = string.substr(start, end - start);

    start = end + 1;
    end = string.find(",", start);

    auto y = string.substr(start, end - start);

    start = end + 1;
    end = string.find(">", start);

    auto z = string.substr(start, end - start);

    start = string.find("r=", start) + 2;
    auto r = string.substr(start);

    return nanobot{
        std::stoll(x),
        std::stoll(y),
        std::stoll(z),
        std::stoull(r)
    };
}

std::vector<nanobot> nanobotsFromFile(const char * path)
{
    std::vector<nanobot> nanobots;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        nanobots.push_back(nanobotFromString(line));
    }
    input.close();

    return nanobots;
}

enum class range
{
    start,
    end
};

std::vector<std::pair<long long, std::set<unsigned long long>>> overlaps(
    std::map<long long, std::vector<std::pair<unsigned long long, range>>>& a_range)
{
    std::vector<std::pair<long long, std::set<unsigned long long>>> a_overlaps;
    std::set<unsigned long long> active;
    for (auto a : a_range)
    {
        for (auto i : a.second)
        {
            if (i.second == range::start)
            {
                active.emplace(i.first);
            }
        }

        a_overlaps.push_back({ a.first, active });

        for (auto i : a.second)
        {
            if (i.second == range::end)
            {
                active.erase(i.first);
            }
        }
    }

    std::sort(a_overlaps.begin(), a_overlaps.end(), [](auto& a, auto& b) {
        return a.second.size() > b.second.size();
    });

    return a_overlaps;
}

int main()
{
    auto nanobots = nanobotsFromFile("data/day23.txt");

    // Part 1
    /*
    {
        nanobot leader = nanobots[0];

        for (int i = 1; i < nanobots.size(); ++i)
        {
            if (nanobots[i].R > leader.R)
            {
                leader = nanobots[i];
            }
        }

        int count = 0;
        for (int i = 0; i < nanobots.size(); ++i)
        {
            auto distance = leader.distance_to(nanobots[i]);

            printf("The nanobot at %lld,%lld,%lld is distance %u away, and so it is ",
                nanobots[i].X, nanobots[i].Y, nanobots[i].Z, distance);

            if (distance <= leader.R)
            {
                printf("in range\n");
                count++;
            }
            else
            {
                printf("not in range\n");
            }
        }

        printf("Nanobots in range of leader: %d\n", count);
    }
    */

    // Part 2
    {
        nanobot optimal_bot;
        int largest_count = -1;

        long long min_x = nanobots[0].X;
        long long min_y = nanobots[0].Y;
        long long min_z = nanobots[0].Z;

        long long max_x = nanobots[0].X;
        long long max_y = nanobots[0].Y;
        long long max_z = nanobots[0].Z;

        for (int i = 0; i < nanobots.size(); ++i)
        {
            auto bot = nanobots[i];

            min_x = min(min_x, bot.X);
            min_y = min(min_y, bot.Y);
            min_z = min(min_z, bot.Z);

            max_x = max(max_x, bot.X);
            max_y = max(max_y, bot.Y);
            max_z = max(max_z, bot.Z);
        }

        nanobot root_bot = { 0, 0, 0, 0 };
        for (long long x = min_x; x <= max_x; ++x)
        {
            for (long long y = min_y; y <= max_y; ++y)
            {
                for (long long z = min_z; z <= max_z; ++z)
                {
                    nanobot test_bot = { x, y, z, 0 };

                    int count = 0;
                    for (int i = 0; i < nanobots.size(); ++i)
                    {
                        auto bot = nanobots[i];
                        if (test_bot.distance_to(bot) <= bot.R)
                        {
                            count++;
                        }
                    }

                    if (count > largest_count)
                    {
                        optimal_bot = test_bot;
                        largest_count = count;
                    }
                    else if (count == largest_count)
                    {
                        if (test_bot.distance_to(root_bot) < optimal_bot.distance_to(root_bot))
                        {
                            optimal_bot = test_bot;
                            largest_count = count;
                        }
                    }
                }
            }
        }

        printf("The optimal position is %lld,%lld,%lld\n",
            optimal_bot.X, optimal_bot.Y, optimal_bot.Z);
        printf("");
    }

    std::getc(stdin);
}
