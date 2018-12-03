
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <map>

struct point
{
    int x;
    int y;

    bool operator< (const point& p) const
    {
        if (p.x == this->x) return (p.y < this->y);
        return (p.x < this->x);
    }
};

struct patch
{
    int id;
    int x;
    int y;
    int w;
    int h;
};

patch patchFromString(const std::string string)
{
    // #A @ B,C: DxE

    auto start = 1;
    auto end = string.find(" @ ");
    auto a = string.substr(start, end - start);

    start = end + 3;
    end = string.find(",");
    auto b = string.substr(start, end - start);

    start = end + 1;
    end = string.find(": ");
    auto c = string.substr(start, end - start);

    start = end + 2;
    end = string.find("x");
    auto d = string.substr(start, end - start);

    start = end + 1;
    auto e = string.substr(start);

    return patch{
        std::stoi(a),
        std::stoi(b),
        std::stoi(c),
        std::stoi(d),
        std::stoi(e)
    };
}

std::vector<patch> patchesFromFile(const char * path)
{
    std::vector<patch> patches;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        patches.push_back(patchFromString(line));
    }
    input.close();

    return patches;
}

int main()
{
    auto patches = patchesFromFile("data/day3.txt");

    std::map<point, int> patch_area;
    for (auto && patch : patches)
    {
        for (int x = 0; x < patch.w; ++x)
        {
            for (int y = 0; y < patch.h; ++y)
            {
                auto p = point{ patch.x + x, patch.y + y };

                if (patch_area.count(p) > 0)
                {
                    patch_area[p] = 0;
                }
                else
                {
                    patch_area[p] = patch.id;
                }
            }
        }
    }

    {
        int overlap = 0;
        for (auto && inch : patch_area)
        {
            if (inch.second == 0)
            {
                overlap++;
            }
        }

        printf("Overlap: %d\n", overlap);
    }

    {
        int nonOverlappingId = 0;
        for (auto && patch : patches)
        {
            bool hasOverlap = false;
            for (int x = 0; x < patch.w; ++x)
            {
                for (int y = 0; y < patch.h; ++y)
                {
                    auto p = point{ patch.x + x, patch.y + y };

                    if (patch_area[p] != patch.id)
                    {
                        hasOverlap = true;
                    }
                }
            }

            if (!hasOverlap)
            {
                nonOverlappingId = patch.id;
                break;
            }
        }

        printf("Non-overlapping Patch: %d\n", nonOverlappingId);
    }

    std::getc(stdin);
}