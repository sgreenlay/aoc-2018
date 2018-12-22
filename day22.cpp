
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <set>
#include <map>
#include <queue>

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

unsigned long long erosion_level(unsigned long long index, int depth)
{
    // A region's erosion level is its geologic index plus the cave system's 
    // depth, all modulo 20183.
    return (index + depth) % 20183;
}

unsigned long long geologic_index(point p, point target, int depth, std::map<point, unsigned long long> geologic_indexes)
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

    auto a = geologic_indexes[point{ p.x - 1, p.y }];
    auto b = geologic_indexes[point{ p.x, p.y - 1 }];

    return erosion_level(a, depth) * erosion_level(b, depth);
}

enum class region_type
{
    Rocky = 0,
    Wet = 1,
    Narrow = 2,
    Target = 3
};

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

enum class tool
{
    Neither = 1,
    Torch = 2,
    Climbing_Gear = 3
};

std::vector<tool> valid_tools(region_type r)
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

struct distance_and_tool
{
    int distance;
    tool t;
};

std::vector<distance_and_tool> costs(region_type a, region_type b, tool t)
{
    if (a == b)
    {
        return { distance_and_tool{1, t} };
    }

    auto other_tools = valid_tools(b);

    std::vector<distance_and_tool> cs;
    for (auto && other_tool : other_tools)
    {
        if (t == other_tool)
        {
            cs.push_back(distance_and_tool{ 1, other_tool });
        }
        else
        {
            cs.push_back(distance_and_tool{ 7 + 1, other_tool });
        }
    }
    return cs;
}

struct point_and_distance
{
    point p;
    unsigned long long distance;
};

inline bool operator <(const point_and_distance& a, const point_and_distance& b)
{
    return (a.p < b.p);
}

int main()
{
    int depth = 5616;
    point target = point{ 10, 785 };
    point max = point{ target.x, target.y };

    std::map<point, unsigned long long> geologic_indexes;
    std::map<point, region_type> area_map;

    unsigned long long risk = 0;

    std::set<point> regions;
    std::string output;
    for (int y = 0; y <= max.y; ++y)
    {
        for (int x = 0; x <= max.x; ++x)
        {
            point p = point{ x, y };
            auto index = geologic_index(p, target, depth, geologic_indexes);
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

            geologic_indexes[p] = index;
            area_map[p] = r;

            regions.emplace(p);

            output.push_back(char_from_region(r));
        }
        output.push_back('\n');
    }

    //printf("%s", output.c_str());
    printf("%d Risk\n", risk);

    

    std::map<point, distance_and_tool> min_distances;
    std::map<point, point> previous;

    point current = point{ 0, 0 };
    min_distances[current] = distance_and_tool{ 0, tool::Torch };

    std::map<point, int> points_and_distances;
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

        if (current == target)
        {
            break;
        }

        points_and_distances.erase(current);

        auto r = area_map[current];
        auto d = min_distances[current];

        std::vector<point> adjs = {
            point{ current.x - 1, current.y },
            point{ current.x + 1, current.y },
            point{ current.x, current.y - 1 },
            point{ current.x, current.y + 1 }
        };

        for (auto adj : adjs)
        {
            if (area_map.count(adj))
            {
                auto adj_ds = costs(r, area_map[adj], d.t);
                for (auto adj_d : adj_ds)
                {
                    adj_d.distance += d.distance;

                    if (min_distances.count(adj) > 0)
                    {
                        auto existing_distance = min_distances[adj];

                        if (adj_d.distance < existing_distance.distance)
                        {
                            min_distances[adj] = adj_d;
                            previous[adj] = current;

                            points_and_distances[adj] = adj_d.distance;
                        }
                    }
                    else
                    {
                        min_distances[adj] = adj_d;
                        previous[adj] = current;

                        points_and_distances[adj] = adj_d.distance;
                    }
                }
            }
        }
    }

    /*
    point p = target;
    tool t = min_distances[p].t;

    std::vector<point> path;
    std::vector<tool> tools;

    while (p.x != 0 || p.y != 0)
    {
        path.push_back(p);
        tools.push_back(t);

        p = previous[p];
        t = min_distances[p].t;
    }
    path.push_back(p);
    tools.push_back(t);

    for (int i = path.size() - 1; i >= 0; --i)
    {
        if (t != tools[i])
        {
            printf("Switch from %d to %d\n", t, tools[i]);
        }

        p = path[i];
        t = tools[i];

        printf("(%d, %d) - %d [%c]\n", p.x, p.y, tools[i], char_from_region(area_map[p]));
    }
    */

    printf("Minimum travel to target: %d\n", points_and_distances[target]);

    std::getc(stdin);
}
