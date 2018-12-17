
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <variant>
#include <map>

struct point
{
    int x;
    int y;

    bool operator== (const point& p) const
    {
        return (p.y == this->y) && (p.x == this->x);
    }
};

inline bool operator <(const point& a, const point& b)
{
    if (a.y == b.y) return (a.x < b.x);
    return (a.y < b.y);
}

struct clay_deposit
{
    std::variant<int, std::pair<int, int>> x;
    std::variant<int, std::pair<int, int>> y;
};

clay_deposit clayDepositFromString(std::string string)
{
    clay_deposit deposit = {};

    auto start = 0;
    auto end = string.find("=", start);

    auto ch = string.substr(start, end - start);

    start = end + 1;
    end = string.find(", ", start);

    auto value = string.substr(start, end - start);

    if (ch == "x")
    {
        deposit.x = std::stoi(value);
    }
    else if (ch == "y")
    {
        deposit.y = std::stoi(value);
    }
    else
    {
        abort();
    }

    start = end + 2;
    end = string.find("=", start);

    ch = string.substr(start, end - start);

    start = end + 1;
    end = string.find("..", start);

    auto range_start = string.substr(start, end - start);

    start = end + 2;

    auto range_end = string.substr(start);

    if (ch == "x")
    {
        deposit.x = std::pair<int, int>{ 
            std::stoi(range_start),
            std::stoi(range_end)
        };
    }
    else if (ch == "y")
    {
        deposit.y = std::pair<int, int>{
            std::stoi(range_start),
            std::stoi(range_end)
        };
    }
    else
    {
        abort();
    }

    return deposit;
}

std::vector<clay_deposit> clayDepositsFromFile(const char * path)
{
    std::vector<clay_deposit> deposits;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        deposits.push_back(clayDepositFromString(line));
    }
    input.close();

    return deposits;
}

struct cross_section
{
    std::pair<int, int> x_range = { 0, 0 };
    std::pair<int, int> y_range = { 0, 0 };

    std::map<point, char> map;
};

void printCrossSection(const cross_section& cs)
{
    std::string output;
    for (int y = cs.y_range.first; y <= cs.y_range.second; ++y)
    {
        for (int x = cs.x_range.first; x <= cs.x_range.second; ++x)
        {
            auto p = point{ x, y };
            if (cs.map.count(p) > 0)
            {
                output.push_back(cs.map.at(p));
            }
            else
            {
                output.push_back('.');
            }
        }
        output.push_back('\n');
    }
    printf("%s", output.c_str());
}

void floodFill(point start, cross_section& cs)
{
    int x = start.x;
    int y = start.y + 1;

    while ((y > start.y) && (y <= cs.y_range.second))
    {
        if (cs.map.count(point{ x, y + 1 }))
        {
            if (cs.map[point{ x, y + 1 }] == '|')
            {
                cs.map[point{ x, y }] = '|';
                y--;
            }
            else
            {
                //|||
                //#|#
                //

                int x_min = x;
                while (cs.map.count(point{ x_min - 1, y }) == 0 ||
                       cs.map[point{ x_min - 1, y }] == '|')
                {
                    x_min--;

                    if (cs.map.count(point{ x_min, y + 1 }) == 0)
                    {
                        floodFill(point{ x_min, y }, cs);

                        if (cs.map[point{ x_min, y + 1 }] == '|')
                        {
                            break;
                        }
                    }
                }

                int x_max = x;
                while (cs.map.count(point{ x_max + 1, y }) == 0 ||
                       cs.map[point{ x_max + 1, y }] == '|')
                {
                    x_max++;

                    if (cs.map.count(point{ x_max, y + 1 }) == 0)
                    {
                        floodFill(point{ x_max, y }, cs);

                        if (cs.map[point{ x_max, y + 1 }] == '|')
                        {
                            break;
                        }
                    }
                }

                char ch = '|';

                if ((cs.map.count(point{ x_max + 1, y }) > 0) &&
                    (cs.map[point{ x_max + 1, y }] != '|') &&
                    (cs.map.count(point{ x_min - 1, y }) > 0) &&
                    (cs.map[point{ x_min - 1, y }] != '|'))
                {
                    ch = '~';
                }

                for (int xi = x_min; xi <= x_max; ++xi)
                {
                    cs.map[point{ xi, y }] = ch;
                }

                y--;
            }
        }
        else
        {
            cs.map[point{ x, y }] = '|';
            y++;
        }

        //printCrossSection(cs);
        //printf("\n");
    }
}

int main()
{
    auto deposits = clayDepositsFromFile("data/day17.txt");

    cross_section cs;

    point spring = { 500, 0 };
    cs.map[spring] = '+';

    cs.x_range.first = spring.x;
    cs.x_range.second = spring.x;

    cs.y_range.first = spring.y;
    cs.y_range.second = spring.y;

    for (auto && deposit : deposits)
    {
        std::pair<int, int> x_range;
        std::pair<int, int> y_range;

        if (auto x = std::get_if<int>(&deposit.x))
        {
            x_range.first = *x;
            x_range.second = *x;
        }
        else
        {
            x_range = std::get<std::pair<int, int>>(deposit.x);
        }

        if (auto y = std::get_if<int>(&deposit.y))
        {
            y_range.first = *y;
            y_range.second = *y;
        }
        else
        {
            y_range = std::get<std::pair<int, int>>(deposit.y);
        }

        if (x_range.first < cs.x_range.first)
        {
            cs.x_range.first = x_range.first;
        }
        else if (x_range.second > cs.x_range.second)
        {
            cs.x_range.second = x_range.second;
        }

        if (y_range.first < cs.y_range.first)
        {
            cs.y_range.first = y_range.first;
        }
        else if (y_range.second > cs.y_range.second)
        {
            cs.y_range.second = y_range.second;
        }

        for (int y = y_range.first; y <= y_range.second; ++y)
        {
            for (int x = x_range.first; x <= x_range.second; ++x)
            {
                cs.map[point{ x, y }] = 'X';
            }
        }
    }

    floodFill(spring, cs);

    unsigned int water = 0;
    for (auto && loc : cs.map)
    {
        if ((loc.second == '|') || (loc.second == '~'))
        {
            water++;
        }
    }

    printCrossSection(cs);
    printf("%u squares of water\n", water);

    std::getc(stdin);
}
