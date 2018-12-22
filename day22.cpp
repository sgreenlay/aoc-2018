
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <set>
#include <map>
#include <queue>

#define MAX_X 100
#define MAX_Y 800
#define TOOL_MAX 3

enum class region_type
{
    Rocky = 0,
    Wet = 1,
    Narrow = 2,
    Target = 3
};

enum class tool
{
    Neither = 0,
    Torch = 1,
    Climbing_Gear = 2
};

struct point
{
    int x;
    int y;

    bool operator== (const point& p) const
    {
        return (p.y == this->y) && (p.x == this->x);
    }

    bool operator!= (const point& p) const
    {
        return !(p == *this);
    }
};

inline bool operator <(const point& a, const point& b)
{
    if (a.y == b.y) return (a.x < b.x);
    return (a.y < b.y);
}

struct point_and_tool
{
    point p;
    tool t;
};

inline bool operator <(const point_and_tool& a, const point_and_tool& b)
{
    if (a.t == b.t) return (a.p < b.p);
    return (a.t < b.t);
}

static unsigned long long geologic_indexes[MAX_Y][MAX_X];
static region_type area_map[MAX_Y][MAX_X];

static int min_distances[MAX_Y][MAX_X][TOOL_MAX];
static point_and_tool previous[MAX_Y][MAX_X][TOOL_MAX];

unsigned long long erosion_level(unsigned long long index, int depth)
{
    // A region's erosion level is its geologic index plus the cave system's 
    // depth, all modulo 20183.
    return (index + depth) % 20183;
}

unsigned long long geologic_index(point p, point target, int depth)
{
    if (p.x == 0 && p.y == 0)
    {
        // The region at 0, 0 (the mouth of the cave) has a geologic index of 0.
        return 0;
    }
    if (p == target)
    {
        // The region at the coordinates of the target has a geologic index of 0.
        return 0;
    }
    if (p.y == 0)
    {
        // If the region's Y coordinate is 0, the geologic index is its 
        // X coordinate times 16807.
        return p.x * 16807;
    }
    if (p.x == 0)
    {
        // If the region's X coordinate is 0, the geologic index is its 
        // Y coordinate times 48271.
        return p.y * 48271;
    }

    // Otherwise, the region's geologic index is the result of multiplying 
    // the erosion levels of the regions at X-1,Y and X,Y-1.

    auto a = geologic_indexes[p.y][p.x - 1];
    auto b = geologic_indexes[p.y - 1][p.x];

    return erosion_level(a, depth) * erosion_level(b, depth);
}

char char_from_region(region_type r)
{
    if (r == region_type::Rocky)
    {
        return '.';
    }
    if (r == region_type::Wet)
    {
        return '=';
    }
    if (r == region_type::Narrow)
    {
        return '|';
    }
    if (r == region_type::Target)
    {
        return 'T';
    }
    abort();
}

region_type region(unsigned long long erosion)
{
    if (erosion % 3 == 0)
    {
        // If the erosion level modulo 3 is 0, the region's type is rocky.
        return region_type::Rocky;
    }
    if (erosion % 3 == 1)
    {
        // If the erosion level modulo 3 is 1, the region's type is wet.
        return region_type::Wet;
    }
    if (erosion % 3 == 2)
    {
        // If the erosion level modulo 3 is 2, the region's type is narrow.
        return region_type::Narrow;
    }
    abort();
}

std::set<tool> valid_tools(region_type r)
{
    if (r == region_type::Rocky) // '.'
    {
        // In rocky regions, you can use the climbing gear or the torch. 
        // You cannot use neither (you'll likely slip and fall).
        return { tool::Climbing_Gear, tool::Torch };
    }
    if (r == region_type::Wet) // '='
    {
        // In wet regions, you can use the climbing gear or neither tool.
        // You cannot use the torch (if it gets wet, you won't have a light source).
        return { tool::Climbing_Gear, tool::Neither };
    }
    if (r == region_type::Narrow) // '|'
    {
        // In narrow regions, you can use the torch or neither tool. You cannot 
        // use the climbing gear (it's too bulky to fit).
        return { tool::Torch, tool::Neither };
    }
    if (r == region_type::Target)
    {
        return { tool::Torch };
    }
    abort();
}

int main()
{
    int depth = 5616;
    point target = point{ 10, 785 };

    unsigned long long risk = 0;

    std::set<point> regions;
    
    std::vector<std::string> debug_output;
    for (int y = 0; y < MAX_Y; ++y)
    {
        std::string debug_line;
        for (int x = 0; x < MAX_X; ++x)
        {
            point p = point{ x, y };
            auto index = geologic_index(p, target, depth);
            auto erosion = erosion_level(index, depth);
            
            auto r = region(erosion);

            if (x <= target.x && y <= target.y)
            {
                risk += (int)r;
            }

            if (p == target)
            {
                r = region_type::Target;
            }

            geologic_indexes[p.y][p.x] = index;
            area_map[p.y][p.x] = r;

            regions.emplace(p);

            debug_line.push_back(char_from_region(r));
        }
        debug_output.push_back(debug_line);
    }

    for (auto debug_line : debug_output)
    {
        printf("%s\n", debug_line.c_str());
    }
    printf("\n");

    printf("%d Risk\n", risk);

    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            for (int t = 0; t < TOOL_MAX; ++t)
            {
                min_distances[y][x][t] = INT_MAX;
            }
        }
    }

    auto current = point_and_tool{ point{ 0, 0 }, tool::Torch };
    min_distances[current.p.y][current.p.x][(int)tool::Torch] = 0;

    std::map<point_and_tool, int> points_and_distances;
    points_and_distances[current] = 0;

    while (!points_and_distances.empty())
    {
        current = points_and_distances.begin()->first;
        auto current_distance = points_and_distances.begin()->second;

        for (auto possible : points_and_distances)
        {
            if (possible.second < current_distance)
            {
                current = possible.first;
                current_distance = possible.second;
            }
        }

        if (current.p == target)
        {
            break;
        }

        points_and_distances.erase(current);

        auto r = area_map[current.p.y][current.p.x];
        auto d = min_distances[current.p.y][current.p.x][(int)current.t];

        std::vector<point> adjs = {
            point{ current.p.x - 1, current.p.y },
            point{ current.p.x + 1, current.p.y },
            point{ current.p.x, current.p.y - 1 },
            point{ current.p.x, current.p.y + 1 }
        };

        // From this location I can move to the an adjacent location if I have the right tool...
        std::set<point_and_tool> possibilities;
        for (auto adj : adjs)
        {
            if (adj.x >= 0 && adj.y >= 0 && adj.x < MAX_X && adj.y < MAX_Y)
            {
                auto adj_r = area_map[adj.y][adj.x];

                if ((adj_r == r) || (valid_tools(adj_r).count(current.t) > 0))
                {
                    possibilities.emplace(point_and_tool{ adj, current.t });
                }
            }
        }

        // ...or I can switch tools.
        auto switch_tools = valid_tools(r);
        for (auto t : switch_tools)
        {
            if (t != current.t)
            {
                possibilities.emplace(point_and_tool{ current.p, t });
            }
        }

        for (auto possibility : possibilities)
        {
            auto adj = possibility.p;
            auto adj_t = possibility.t;
            int adj_d = d;

            if (possibility.p != current.p)
            {
                adj_d += 1;
            }
            else
            {
                adj_d += 7;
            }

            auto existing_distance = min_distances[adj.y][adj.x][(int)adj_t];

            if (adj_d < existing_distance)
            {
                min_distances[adj.y][adj.x][(int)adj_t] = adj_d;
                previous[adj.y][adj.x][(int)adj_t] = current;

                points_and_distances[possibility] = adj_d;
            }
        }
    }

    /*
    current = point_and_tool{ target, tool::Torch };
    std::vector<point_and_tool> path;

    int transitions = 0;
    while (current.p.x != 0 && current.p.y != 0)
    {
        path.push_back(current);

        auto next = previous[current.p.y][current.p.x][(int)current.t];
        if (next.t != current.t)
        {
            transitions++;
        }
        current = next;
    }

    for (int i = path.size() - 1; i >= 0; --i)
    {
        auto step = path[i];
        printf("(%d, %d) [%c] with %d\n", 
            step.p.x, step.p.y, 
            char_from_region(area_map[step.p.y][step.p.x]),
            step.t);
    }
    */

    printf("Minimum travel to target: %d\n", 
        points_and_distances[point_and_tool{ target, tool::Torch }]);

    std::getc(stdin);
}
