
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
    Narrow = 2
};

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

int main()
{
    int depth = 5616;
    point target = point{ 10, 785 };

    std::map<point, unsigned long long> geologic_indexes;

    std::string output;
    unsigned long long risk = 0;

    for (int y = 0; y <= target.y; ++y)
    {
        for (int x = 0; x <= target.x; ++x)
        {
            point p = point{ x, y };
            auto index = geologic_index(p, target, depth, geologic_indexes);
            auto erosion = erosion_level(index, depth);
            
            auto r = region(erosion);
            switch (r)
            {
            case region_type::Rocky:
                output.push_back('.');
                break;
            case region_type::Wet:
                output.push_back('=');
                break;
            case region_type::Narrow:
                output.push_back('|');
                break;
            }

            risk += (int)r;

            geologic_indexes[p] = index;
        }
        output.push_back('\n');
    }

    //printf("%s", output.c_str());
    printf("%d Risk\n", risk);

    std::getc(stdin);
}
