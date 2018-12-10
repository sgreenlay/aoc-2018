
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <map>
#include <set>

struct point {
    int x;
    int y;
    int vx;
    int vy;
};

point pointFromString(std::string string)
{
    auto start = string.find("position=<") + 10;
    auto end = string.find(", ", start);

    auto x = string.substr(start, end - start);

    start = end + 2;
    end = string.find("> velocity=<", start);

    auto y = string.substr(start, end - start);

    start = end + 12;
    end = string.find(", ", start);

    auto vx = string.substr(start, end - start);

    start = end + 2;
    end = string.find(">", start);

    auto vy = string.substr(start, end - start);

    return point{
        std::stoi(x),
        std::stoi(y),
        std::stoi(vx),
        std::stoi(vy)
    };
}

std::vector<point> pointsFromFile(const char * path)
{
    std::vector<point> points;

    std::ifstream input;
    input.open(path);

    int label = 0;
    std::string line;
    while (std::getline(input, line))
    {
        points.push_back(pointFromString(line));
    }
    input.close();

    return points;
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

struct rect
{
    int xMin;
    int yMin;
    int xMax;
    int yMax;

    int size()
    {
        return xMax - xMin + yMax - yMin;
    }
};

rect calculateBoundingRect(const std::vector<point> points)
{
    int xMin = points[0].x;
    int yMin = points[0].y;

    int xMax = points[0].x;
    int yMax = points[0].y;

    for (auto && point : points)
    {
        xMin = min(xMin, point.x);
        yMin = min(yMin, point.y);
        xMax = max(xMax, point.x);
        yMax = max(yMax, point.y);
    }

    return rect{
        xMin,
        yMin,
        xMax,
        yMax
    };
}

int main()
{
    auto points = pointsFromFile("data/day10.txt");

    int iteration;
    rect boundingRect = calculateBoundingRect(points);
    for (iteration = 1; ; ++iteration)
    {
        for (auto & point : points)
        {
            point.x += point.vx;
            point.y += point.vy;
        }

        rect newBoundingRect = calculateBoundingRect(points);

        if (newBoundingRect.size() >= boundingRect.size())
        {
            for (auto & point : points)
            {
                point.x -= point.vx;
                point.y -= point.vy;
            }

            iteration--;

            break;
        }

        boundingRect = newBoundingRect;
    }

    std::map<int, std::set<int>> point_map;

    for (auto && point : points)
    {
        if (point_map.count(point.y) == 0)
        {
            point_map[point.y] = std::set<int>{};
        }
        point_map[point.y].emplace(point.x);
    }

    std::string message;
    for (int y = boundingRect.yMin; y <= boundingRect.yMax; ++y)
    {
        for (int x = boundingRect.xMin; x <= boundingRect.xMax; ++x)
        {
            if ((point_map.count(y) > 0) && (point_map[y].count(x) > 0))
            {
                message.append("#");
            }
            else
            {
                message.append(".");
            }
        }
        message.append("\n");
    }

    printf("%s\n", message.c_str());
    printf("%d seconds\n", iteration);

    std::getc(stdin);
}
