
#include <fstream>
#include <sstream>
#include <string>

#include <vector>

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

int main()
{
    auto nanobots = nanobotsFromFile("data/day23.txt");

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
        
        /*
        printf("The nanobot at %lld,%lld,%lld is distance %u away, and so it is ",
            nanobots[i].X, nanobots[i].Y, nanobots[i].Z, distance);
            */

        if (distance <= leader.R)
        {
            //printf("in range\n");
            count++;
        }
        else
        {
            //printf("not in range\n");
        }
    }

    printf("Nanobots in range of leader: %d\n", count);

    std::getc(stdin);
}
