
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <map>
#include <set>
#include <optional>

struct point
{
    int x;
    int y;

    bool operator< (const point& p) const
    {
        if (p.y == this->y) return (p.x > this->x);
        return (p.y > this->y);
    }
};

struct layout
{
    std::vector<std::string> tracks;
    int w, h;

    std::optional<char> getTrack(point p)
    {
        if ((p.x >= w) || (p.x < 0) || (p.y >= h) || (p.y < 0))
        {
            return {};
        }

        char ch = tracks[p.y][p.x];
        if (ch == ' ')
        {
            return {};
        }
        return ch;
    }
};

layout mapFromFile(const char * path)
{
    layout layout;

    std::ifstream input;
    input.open(path);

    std::string line;

    int y = 0;
    while (std::getline(input, line) && line.length())
    {
        layout.tracks.push_back(line);
        y++;
    }

    layout.h = y;
    layout.w = layout.tracks[0].length();

    input.close();

    return layout;
}

struct cart
{
    int id;
    char ch;
    int i_count;

    void handle_intersection()
    {
        // Each time a cart has the option to turn (by arriving at any intersection), 
        // it turns left the first time, goes straight the second time, turns right 
        // the third time, and then repeats those directions starting again with left 
        // the fourth time, straight the fifth time, and so on.

        if (ch == '>')
        {
            switch (i_count)
            {
                case 0: // left
                    ch = '^';
                    break;
                case 1: // straight
                    break;
                case 2: // right
                    ch = 'v';
                    break;
            }
        }
        else if (ch == '<')
        {
            switch (i_count)
            {
            case 0: // left
                ch = 'v';
                break;
            case 1: // straight
                break;
            case 2: // right
                ch = '^';
                break;
            }
        }
        else if (ch == 'v')
        {
            switch (i_count)
            {
            case 0: // left
                ch = '>';
                break;
            case 1: // straight
                break;
            case 2: // right
                ch = '<';
                break;
            }
        }
        else if (ch == '^')
        {
            switch (i_count)
            {
            case 0: // left
                ch = '<';
                break;
            case 1: // straight
                break;
            case 2: // right
                ch = '>';
                break;
            }
        }
        i_count = (i_count + 1) % 3;
    }
};

void outputMap(int iteration, layout map, std::map<point, cart> carts)
{
    std::string line;
    for (int y = 0; y < map.h; ++y)
    {
        for (int x = 0; x < map.w; ++x)
        {
            auto p = point{ x, y };
            if (carts.count(p))
            {
                //line.push_back('a' + carts[p].id);
                line.push_back(carts[p].ch);
            }
            else if (auto ch = map.getTrack(p))
            {
                line.push_back(*ch);
            }
            else
            {
                line.push_back(' ');
            }
        }
        line.push_back('\n');
    }

    std::ofstream output("output.txt");
    output << iteration << "\n";
    output << line;
    output.close();
}

int main()
{
    auto map = mapFromFile("data/day13.txt");

    std::map<point, cart> carts;

    int id = 0;
    for (int y = 0; y < map.h; ++y)
    {
        for (int x = 0; x < map.w; ++x)
        {
            auto p = point{ x, y };

            if (auto ch = map.getTrack(p))
            {
                if ((ch == '>') || (ch == '<') || (ch == 'v') || (ch == '^'))
                {
                    carts[p] = cart{ id++, *ch, 0 };

                    char replacement;
                    if ((ch == '>') || (ch == '<'))
                    {
                        replacement = '-';
                    }
                    else if ((ch == 'v') || (ch == '^'))
                    {
                        replacement = '|';
                    }
                    else
                    {
                        abort();
                    }

                    map.tracks[p.y][p.x] = replacement;
                }
            }
        }
    }
    //outputMap(0, map, {});

    int count = 0;
    std::optional<point> crash_point = {};
    while (carts.size() > 1)
    {
        std::set<point> remove_carts;
        std::map<point, cart> next_carts;
        for (auto && current_cart : carts)
        {
            // If someone has crashed into me
            if (remove_carts.count(current_cart.first) > 0)
            {
                continue;
            }

            // If someone's going to crash into me
            if (next_carts.count(current_cart.first) > 0)
            {
                if (!crash_point)
                {
                    crash_point = current_cart.first;
                }

                //outputMap(count, map, carts);
                //printf("[%d] Crash at %d,%d\n", count, current_cart.first.x, current_cart.first.y);

                remove_carts.emplace(current_cart.first);

                continue;
            }

            auto cart = current_cart.second;
            
            int x = current_cart.first.x;
            int y = current_cart.first.y;

            if (cart.ch == '>')
            {
                auto right = map.getTrack(point{ x + 1, y });
                if (right)
                {
                    if (*right == '+')
                    {
                        cart.handle_intersection();
                    }
                    else if (*right == '\\')
                    {
                        cart.ch = 'v';
                    }
                    else if (*right == '/')
                    {
                        cart.ch = '^';
                    }
                    x += 1;
                }
                else
                {
                    //outputMap(count, map, carts);
                    abort();
                }
            }
            else if (cart.ch == '<')
            {
                auto left = map.getTrack(point{ x - 1, y });
                if (left)
                {
                    if (*left == '+')
                    {
                        cart.handle_intersection();
                    }
                    else if (*left == '\\')
                    {
                        cart.ch = '^';
                    }
                    else if (*left == '/')
                    {
                        cart.ch = 'v';
                    }
                    x -= 1;
                }
                else
                {
                    //outputMap(count, map, carts);
                    abort();
                }
            }
            else if (cart.ch == 'v')
            {
                auto down = map.getTrack(point{ x, y + 1 });
                if (down)
                {
                    if (*down == '+')
                    {
                        cart.handle_intersection();
                    }
                    else if (*down == '\\')
                    {
                        cart.ch = '>';
                    }
                    else if (*down == '/')
                    {
                        cart.ch = '<';
                    }
                    y += 1;
                }
                else
                {
                    //outputMap(count, map, carts);
                    abort();
                }
            }
            else if (cart.ch == '^')
            {
                auto up = map.getTrack(point{ x, y - 1 });
                if (up)
                {
                    if (*up == '+')
                    {
                        cart.handle_intersection();
                    }
                    else if (*up == '\\')
                    {
                        cart.ch = '<';
                    }
                    else if (*up == '/')
                    {
                        cart.ch = '>';
                    }
                    y -= 1;
                }
                else
                {
                    //outputMap(count, map, carts);
                    abort();
                }
            }
            else
            {
                //outputMap(count, map, carts);
                abort();
            }

            point p_next = point{ x, y };

            // If I'm going to crash into someone
            if (next_carts.count(p_next) > 0)
            {
                if (!crash_point)
                {
                    crash_point = p_next;
                }

                //outputMap(count, map, carts);
                //printf("[%d] Crash at %d,%d\n", count, p_next.x, p_next.y);

                remove_carts.emplace(p_next);
            }
            else
            {
                next_carts[p_next] = cart;
            }
        }

        for (auto p : remove_carts)
        {
            next_carts.erase(p);
        }

        carts = next_carts;
        count++;
    }

    printf("First crash is at %d,%d\n", crash_point->x, crash_point->y);
    printf("Last cart is at %d,%d\n", carts.begin()->first.x, carts.begin()->first.y);

    std::getc(stdin);
}
