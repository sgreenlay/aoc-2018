
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <map>
#include <optional>

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

struct lumberyard
{
    std::vector<std::string> map;

    int width() const
    {
        return map[0].length();
    }

    int height() const
    {
        return map.size();
    }

    char acre(int x, int y)
    {
        if (x < 0 || x >= width() || y < 0 || y >= width())
        {
            return ' ';
        }
        return map[y][x];
    }

    char nextState(int x, int y)
    {
        // a b c
        // d   e
        // f g h

        int open = 0;
        int trees = 0;
        int lumber = 0;

        for (int iy = y - 1; iy <= y + 1; ++iy)
        {
            for (int ix = x - 1; ix <= x + 1; ++ix)
            {
                if (ix == x && iy == y)
                {
                    continue;
                }

                switch (acre(ix, iy))
                {
                case '.':
                    open++;
                    break;
                case '|':
                    trees++;
                    break;
                case '#':
                    lumber++;
                    break;
                case ' ':
                    break;
                default:
                    abort();
                    break;
                }
            }
        }


        char state = acre(x, y);
        
        // An open acre will become filled with trees if three or more 
        // adjacent acres contained trees. Otherwise, nothing happens.

        if (state == '.')
        {
            if (trees >= 3)
            {
                return '|';
            }
            return '.';
        }

        // An acre filled with trees will become a lumberyard if three 
        // or more adjacent acres were lumberyards. Otherwise, nothing 
        // happens.

        else if (state == '|')
        {
            if (lumber >= 3)
            {
                return '#';
            }
            return '|';
        }

        // An acre containing a lumberyard will remain a lumberyard if 
        // it was adjacent to at least one other lumberyard and at least 
        // one acre containing trees. Otherwise, it becomes open.

        else if (state == '#')
        {
            if (lumber >= 1 && trees >= 1)
            {
                return '#';
            }
            return '.';
        }

        abort();
        return ' ';
    }

    void print()
    {
        for (auto line : map)
        {
            printf("%s\n", line.c_str());
        }
    }
};

lumberyard lumberyardFromFile(const char * path)
{
    std::vector<std::string> lines;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        lines.push_back(line);
    }
    input.close();

    return lumberyard{ lines };
}

int main()
{
    auto ly = lumberyardFromFile("data/day18.txt");

    //ly.print();
    //printf("\n");

    std::vector<point> values;
    std::map<point, int> value_map;

    std::optional<point> sequence;

    for (int i = 0; i < 1000000000; ++i)
    {
        int open = 0;
        int trees = 0;
        int lumber = 0;

        lumberyard next_ly{ ly.map };
        for (int y = 0; y < ly.height(); ++y)
        {
            for (int x = 0; x < ly.width(); ++x)
            {
                next_ly.map[y][x] = ly.nextState(x, y);

                switch (next_ly.map[y][x])
                {
                case '.':
                    open++;
                    break;
                case '|':
                    trees++;
                    break;
                case '#':
                    lumber++;
                    break;
                case ' ':
                default:
                    abort();
                    break;
                }
            }
        }

        //next_ly.print();
        //printf("\n");

        if (i == 9)
        {
            printf("There are %d wooded and %d lumberyards = %d\n", trees, lumber, trees * lumber);
        }

        auto value = point{ trees, lumber };

        if (sequence.has_value())
        {
            if (values[sequence->x] == value)
            {
                printf("Cycle detected from %d to %d:\n", sequence->x, sequence->y);
                for (int j = sequence->x; j <= sequence->y; ++j)
                {
                    printf("  %d: (%d, %d)\n", j, values[j].x, values[j].y);
                }

                int cycle_length = sequence->y - sequence->x;
                int idx = (1000000000 - sequence->x) % cycle_length;
                auto last_repetition = values[sequence->x + idx];

                printf("There are %d wooded and %d lumberyards = %d\n", 
                    last_repetition.x,
                    last_repetition.y,
                    last_repetition.x * last_repetition.y);

                break;
            }
            else if (values[sequence->y + 1] == value)
            {
                sequence->y++;
            }
            else
            {
                sequence = {};
            }
        }
        else if (value_map.count(value))
        {
            sequence = point{ value_map[value], value_map[value] };
        }

        value_map[value] = i;
        values.push_back(value);


        ly = next_ly;
    }

    std::getc(stdin);
}
