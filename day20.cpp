
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
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

std::string lineFromFile(const char * path)
{
    std::ifstream input;
    input.open(path);

    std::string line;
    std::getline(input, line);
    input.close();

    return line;
}

enum class directions {
    North = 0x1,
    South = 0x2,
    East = 0x4,
    West = 0x8
};

directions directionsFromChar(char ch)
{
    switch (ch)
    {
        case 'N':
            return directions::North;
        case 'S':
            return directions::South;
        case 'E':
            return directions::East;
        case 'W':
            return directions::West;
        default:
            abort();
    }
}

directions operator|(directions lhs, directions rhs)
{
    directions ret = static_cast<directions>(static_cast<char>(lhs) | static_cast<char>(rhs));
    return ret;
}

directions operator&(directions lhs, directions rhs)
{
    directions ret = static_cast<directions>(static_cast<char>(lhs) & static_cast<char>(rhs));
    return ret;
}

int min(int a, int b)
{
    if (a < b) return a;
    return b;
}

int max(int a, int b)
{
    if (a > b) return a;
    return b;
}

struct room
{
    directions doors;
    int distance;
};

void printRooms(std::map<point, room> rooms, int x_min, int y_min, int x_max, int y_max)
{
    std::string output;
    for (int y = y_min; y <= y_max; ++y)
    {
        std::string a;
        std::string b;
        std::string c;

        for (int x = x_min; x <= x_max; ++x)
        {
            point p = point{ x, y };
            if (rooms.count(p) == 0)
            {
                a.append("###");
                b.append("###");
                c.append("###");
            }
            else
            {
                auto d = rooms[p];

                a.push_back('#');
                if ((d.doors & directions::North) == directions::North)
                {
                    a.push_back('-');
                }
                else
                {
                    a.push_back('#');
                }
                a.push_back('#');


                if ((d.doors & directions::West) == directions::West)
                {
                    b.push_back('|');
                }
                else
                {
                    b.push_back('#');
                }

                if (x == 0 && y == 0)
                {
                    b.push_back('X');
                }
                else
                {
                    b.push_back('.');
                }
                if ((d.doors & directions::East) == directions::East)
                {
                    b.push_back('|');
                }
                else
                {
                    b.push_back('#');
                }

                c.push_back('#');
                if ((d.doors & directions::South) == directions::South)
                {
                    c.push_back('-');
                }
                else
                {
                    c.push_back('#');
                }
                c.push_back('#');
            }
        }
        output.append(a);
        output.push_back('\n');
        output.append(b);
        output.push_back('\n');
        output.append(c);
        output.push_back('\n');
    }
    printf("%s", output.c_str());
}

struct junction
{
    point p;
    int depth;
};

int main()
{
    auto line = lineFromFile("data/day20.txt");

    point p = point{ 0, 0 };
    std::vector<junction> junctions;
    std::map<point, room> rooms;

    int x_min = 0;
    int x_max = 0;
    int y_min = 0;
    int y_max = 0;

    int depth = 0;
    for (int i = 1; i < line.length() - 1; ++i)
    {
        if (line[i] == '(')
        {
            junctions.push_back(junction{ p, depth });
        }
        else if (line[i] == ')')
        {
            p = junctions.back().p;
            depth = junctions.back().depth;

            junctions.pop_back();
        }
        else if (line[i] == '|')
        {
            p = junctions.back().p;
            depth = junctions.back().depth;
        }
        else
        {
            auto d = directionsFromChar(line[i]);

            if (rooms.count(p) == 0)
            {
                rooms[p] = room{ d, depth };
            }
            else
            {
                rooms[p].doors = rooms[p].doors | d;
                rooms[p].distance = min(rooms[p].distance, depth);
            }

            directions d_from;
            switch (d) {
                case directions::North:
                    p.y -= 1;
                    d_from = directions::South;
                    break;
                case directions::South:
                    p.y += 1;
                    d_from = directions::North;
                    break;
                case directions::East:
                    p.x += 1;
                    d_from = directions::West;
                    break;
                case directions::West:
                    p.x -= 1;
                    d_from = directions::East;
                    break;
                default:
                    abort();
            }

            depth++;

            if (rooms.count(p) == 0)
            {
                rooms[p] = room{ d, depth };
            }
            else
            {
                rooms[p].doors = rooms[p].doors | d_from;
                rooms[p].distance = min(rooms[p].distance, depth);
            }

            x_min = min(x_min, p.x);
            x_max = max(x_max, p.x);
            y_min = min(y_min, p.y);
            y_max = max(y_max, p.y);
        }
    }

    int max_depth = 0;
    int count = 0;

    for (auto r : rooms)
    {
        if (r.second.distance >= 1000)
        {
            count++;
        }
        max_depth = max(max_depth, r.second.distance);
    }

    //printRooms(rooms, x_min, y_min, x_max, y_max);
    printf("Max depth of %d\n", max_depth);
    printf("%d rooms require at least 1000 doors\n", count);

    std::getc(stdin);
}
