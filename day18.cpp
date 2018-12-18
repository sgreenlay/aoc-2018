
#include <fstream>
#include <sstream>
#include <string>

#include <vector>

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

    for (int i = 0; i < 10; ++i)
    {
        lumberyard next_ly{ ly.map };
        for (int y = 0; y < ly.height(); ++y)
        {
            for (int x = 0; x < ly.width(); ++x)
            {
                next_ly.map[y][x] = ly.nextState(x, y);
            }
        }

        //next_ly.print();
        //printf("\n");

        ly = next_ly;
    }

    int open = 0;
    int trees = 0;
    int lumber = 0;

    for (int y = 0; y < ly.height(); ++y)
    {
        for (int x = 0; x < ly.width(); ++x)
        {
            switch (ly.acre(x, y))
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

    printf("There are %d wooded and %d lumberyards = %d\n", trees, lumber, trees * lumber);

    std::getc(stdin);
}
