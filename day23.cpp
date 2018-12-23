
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

struct point
{
    long long X;
    long long Y;
    long long Z;
};

long long manhatten_distance(
    long long X1, long long Y1, long long Z1,
    long long X2 = 0, long long Y2 = 0, long long Z2 = 0)
{
    long long dx = std::abs(X2 - X1);
    long long dy = std::abs(Y2 - Y1);
    long long dz = std::abs(Z2 - Z1);

    return dx + dy + dz;
}

struct nanobot
{
    long long X;
    long long Y;
    long long Z;
    unsigned long long R;

    unsigned long long distance_to(const nanobot& other) const
    {
        return manhatten_distance(X, Y, Z, other.X, other.Y, other.Z);
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

int main()
{
    auto nanobots = nanobotsFromFile("data/day23.txt");

    // Part 1
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

    // Part 2
    {
        long long min_x = nanobots[0].X;
        long long min_y = nanobots[0].Y;
        long long min_z = nanobots[0].Z;

        long long max_x = nanobots[0].X;
        long long max_y = nanobots[0].Y;
        long long max_z = nanobots[0].Z;

        for (auto bot : nanobots)
        {
            min_x = min(min_x, bot.X);
            min_y = min(min_y, bot.Y);
            min_z = min(min_z, bot.Z);

            max_x = max(max_x, bot.X);
            max_y = max(max_y, bot.Y);
            max_z = max(max_z, bot.Z);
        }

        point p;
        int largest_count = 0;

        long long search_distance = 1;
        while (search_distance < max_x - min_x)
        {
            search_distance *= 2;
        }

        while (search_distance > 0)
        {
            for (long long x = min_x; x <= max_x; x += search_distance)
            {
                for (long long y = min_y; y <= max_y; y += search_distance)
                {
                    for (long long z = min_z; z <= max_z; z += search_distance)
                    {
                        int count = 0;
                        for (auto bot : nanobots)
                        {
                            if (manhatten_distance(x, y, z, bot.X, bot.Y, bot.Z) <= bot.R)
                            {
                                count++;
                            }
                        }

                        if (count > largest_count)
                        {
                            p = { x, y, z };
                            largest_count = count;
                        }

                        if (count == largest_count)
                        {
                            if (manhatten_distance(x, y, z) < manhatten_distance(p.X, p.Y, p.Z))
                            {
                                p = { x, y, z };
                                largest_count = count;
                            }
                        }
                    }
                }
            }

            if (search_distance == 1)
            {
                break;
            }
            else
            {
                min_x = p.X - search_distance;
                max_x = p.X + search_distance;

                min_y = p.Y - search_distance;
                max_y = p.Y + search_distance;

                min_z = p.Z - search_distance;
                max_z = p.Z + search_distance;

                search_distance /= 2;
            }
        }

        printf("Optimal position (%lld, %lld, %lld) => %lld\n",
            p.X, p.Y, p.Z, manhatten_distance(p.X, p.Y, p.Z));
    }

    std::getc(stdin);
}
