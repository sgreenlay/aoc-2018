
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <map>

struct coordinate
{
    int label;
    int x;
    int y;
};

coordinate coordinateFromString(int label, const std::string string)
{
    auto start = 0;
    auto end = string.find(", ");

    auto x = string.substr(start, end - start);

    start = end + 2;
    auto y = string.substr(start);

    return coordinate{
        label,
        std::stoi(x),
        std::stoi(y)
    };
}

std::vector<coordinate> coordinatesFromFile(const char * path)
{
    std::vector<coordinate> coordinates;

    std::ifstream input;
    input.open(path);

    int label = 0;
    std::string line;
    while (std::getline(input, line))
    {
        coordinates.push_back(coordinateFromString(++label, line));
    }
    input.close();

    return coordinates;
}

int main()
{
    auto coordinates = coordinatesFromFile("data/day6.txt");
    int maxTotalDistance = 10000;

    int max = 0;
    for (auto && coordinate : coordinates)
    {
        if (coordinate.x > max)
        {
            max = coordinate.x;
        }
        if (coordinate.y > max)
        {
            max = coordinate.y;
        }
    }

    std::map<int, std::pair<int, bool>> counts;
    for (auto && coordinate : coordinates)
    {
        counts[coordinate.label] = std::pair<int, bool>{ 0 , true };
    }

    std::map<int, std::map<int, int>> grid;
    int maxTotalDistanceRegion = 0;
    for (int y = 0; y <= max; ++y)
    {
        grid[y] = std::map<int, int>{};
        for (int x = 0; x <= max; ++x)
        {
            int label = 0;
            int distance = 2 * max;
            int totalDistance = 0;

            for (auto && coordinate : coordinates)
            {
                if (x == coordinate.x && y == coordinate.y)
                {
                    distance = 0;
                    label = coordinate.label;
                    continue;
                }
                
                int manhattenDistance = abs(coordinate.x - x) + abs(coordinate.y - y);
                totalDistance += manhattenDistance;

                if (manhattenDistance < distance)
                {
                    distance = manhattenDistance;
                    label = coordinate.label;
                }
                else if (manhattenDistance == distance)
                {
                    label = 0;
                }
            }

            grid[y][x] = label;
            
            if (label != 0)
            {
                counts[label].first++;

                if (x == 0 || y == 0 || x == max || y == max)
                {
                    counts[label].second = false;
                }
            }

            if (totalDistance < maxTotalDistance)
            {
                maxTotalDistanceRegion++;
                //printf("#");
            }
            else
            {
                //printf("%c", (grid[y][x] == 0) ? '.' : 'a' + grid[y][x]);
            }
        }
        //printf("\n");
    }
    //printf("\n");

    int label = 0;
    int maxCount = 0;
    for (auto && count : counts)
    {
        if (count.second.second && (count.second.first > maxCount))
        {
            label = count.first;
            maxCount = count.second.first;
        }
    }

    printf("Max count of %d for %d\n", maxCount, label);
    printf("Max region to total distanct < %d is %d\n", maxTotalDistance, maxTotalDistanceRegion);

    std::getc(stdin);
}