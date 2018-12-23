
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

    unsigned long long distance_to(const nanobot& other) const
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

long long count_of_bots_in_range_of(const nanobot test, const std::vector<nanobot> nanobots)
{
    long long count = 0;
    for (int i = 0; i < nanobots.size(); ++i)
    {
        auto distance = nanobots[i].distance_to(test);
        if (distance <= nanobots[i].R)
        {
            count++;
        }
    }
    return count;
};

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
        std::map<long long, std::vector<std::pair<unsigned long long, range>>> x_ranges;
        std::map<long long, std::vector<std::pair<unsigned long long, range>>> y_ranges;
        std::map<long long, std::vector<std::pair<unsigned long long, range>>> z_ranges;

        int shortest_range = nanobots[0].R;

        for (int i = 0; i < nanobots.size(); ++i)
        {
            auto bot = nanobots[i];

            x_ranges[bot.X - bot.R].push_back({ i, range::start });
            x_ranges[bot.X + bot.R].push_back({ i, range::end });

            y_ranges[bot.Y - bot.R].push_back({ i, range::start });
            y_ranges[bot.Y + bot.R].push_back({ i, range::end });

            z_ranges[bot.Z - bot.R].push_back({ i, range::start });
            z_ranges[bot.Z + bot.R].push_back({ i, range::end });

            shortest_range = min(shortest_range, bot.R);
        }

        auto x_overlaps = overlaps(x_ranges);
        auto y_overlaps = overlaps(y_ranges);
        auto z_overlaps = overlaps(z_ranges);

        std::vector<std::pair<
            std::tuple<long long, long long, long long>,
            std::set<unsigned long long>>> largest_overlaps;
        int largest_overlap_size = 0;

        for (int x = 0; x < x_overlaps.size(); ++x)
        {
            auto x_overlap = x_overlaps[x];
            if (x_overlap.second.size() < largest_overlap_size)
            {
                break;
            }
            for (int y = 0; y < y_overlaps.size(); ++y)
            {
                auto y_overlap = y_overlaps[y];
                if (y_overlap.second.size() < largest_overlap_size)
                {
                    break;
                }
                for (int z = 0; z < z_overlaps.size(); ++z)
                {
                    auto z_overlap = z_overlaps[z];
                    if (z_overlap.second.size() < largest_overlap_size)
                    {
                        break;
                    }

                    auto xyz = std::tuple<long long, long long, long long>{
                        x_overlap.first,
                        y_overlap.first,
                        z_overlap.first
                    };

                    std::set<unsigned long long> active;
                    for (auto i : x_overlap.second)
                    {
                        if (y_overlap.second.count(i) > 0 &&
                            z_overlap.second.count(i) > 0)
                        {
                            active.emplace(i);
                        }
                    }

                    if (active.size() > largest_overlap_size)
                    {
                        largest_overlaps.clear();
                    }
                    if (active.size() >= largest_overlap_size)
                    {
                        largest_overlap_size = active.size();
                        largest_overlaps.push_back({ xyz, active });
                    }
                }
            }
        }

        std::set<long long> xs;
        std::set<long long> ys;
        std::set<long long> zs;

        for (auto overlap : largest_overlaps)
        {
            xs.emplace(std::get<0>(overlap.first));
            ys.emplace(std::get<1>(overlap.first));
            zs.emplace(std::get<2>(overlap.first));
        }

        nanobot root_bot = { 0, 0, 0, 0 };

        nanobot optimal_bot = { 
            std::get<0>(largest_overlaps[0].first),
            std::get<0>(largest_overlaps[0].first),
            std::get<0>(largest_overlaps[0].first),
            0
        };
        long long largest_count = count_of_bots_in_range_of(optimal_bot, nanobots);
        printf("%d\n", largest_count);

        for (auto x : xs)
        {
            for (auto y : ys)
            {
                for (auto z : zs)
                {
                    nanobot test_bot = { x, y, z, 0 };
                    long long count = count_of_bots_in_range_of(test_bot, nanobots);

                    if (count > largest_count)
                    {
                        printf("%d\n", count);
                        optimal_bot = test_bot;
                        largest_count = count;
                    }
                    else if (count == largest_count)
                    {
                        if (test_bot.distance_to(root_bot) < optimal_bot.distance_to(root_bot))
                        {
                            printf("%d\n", count);
                            optimal_bot = test_bot;
                            largest_count = count;
                        }
                    }
                }
            }
        }

        printf("The optimal position is %lld,%lld,%lld (score %lld)\n",
            optimal_bot.X, optimal_bot.Y, optimal_bot.Z,
            optimal_bot.X + optimal_bot.Y + optimal_bot.Z);

        printf("");
    }

    std::getc(stdin);
}
