
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <set>

int manhatten_distance(
    int X1, int Y1, int Z1, int N1,
    int X2 = 0, int Y2 = 0, int Z2 = 0, int N2 = 0)
{
    int dx = std::abs(X2 - X1);
    int dy = std::abs(Y2 - Y1);
    int dz = std::abs(Z2 - Z1);
    int dn = std::abs(N2 - N1);

    return dx + dy + dz + dn;
}

std::vector<std::tuple<int, int, int, int>> pointsFromFile(const char * path)
{
    std::vector<std::tuple<int, int, int, int>> points;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        auto start = 0;
        auto end = line.find(",", start);

        int a = std::stoi(line.substr(start, end - start));

        start = end + 1;
        end = line.find(",", start);

        int b = std::stoi(line.substr(start, end - start));

        start = end + 1;
        end = line.find(",", start);

        int c = std::stoi(line.substr(start, end - start));

        start = end + 1;

        int d = std::stoi(line.substr(start));

        points.push_back({ a, b, c, d });
    }
    input.close();

    return points;
}

int main()
{
    auto points = pointsFromFile("data/day25.txt");

    std::vector<std::set<int>> constellations;
    for (int i = 0; i < points.size(); ++i)
    {
        auto a = points[i];

        std::set<int> in_constellation;
        in_constellation.emplace(i);

        for (int j = 0; j < points.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }

            auto b = points[j];
            int distance = manhatten_distance(
                std::get<0>(a),
                std::get<1>(a),
                std::get<2>(a),
                std::get<3>(a),
                std::get<0>(b),
                std::get<1>(b),
                std::get<2>(b),
                std::get<3>(b));

            if (distance <= 3)
            {
                in_constellation.emplace(j);
            }
        }

        std::set<int> existing_constellations;
        for (int c = 0; c < constellations.size(); ++c)
        {
            for (int k : in_constellation)
            {
                if (constellations[c].count(k) > 0)
                {
                    existing_constellations.emplace(c);
                    break;
                }
            }
        }

        if (existing_constellations.size() == 0)
        {
            constellations.push_back(in_constellation);
        }
        else
        {
            std::vector<std::set<int>> new_constellations;
            for (int c = 0; c < constellations.size(); ++c)
            {
                if (existing_constellations.count(c) > 0)
                {
                    for (auto k : constellations[c])
                    {
                        in_constellation.emplace(k);
                    }
                }
                else
                {
                    new_constellations.push_back(constellations[c]);
                }
            }

            new_constellations.push_back(in_constellation);
            constellations = new_constellations;
        }
    }

    printf("%d constellations\n", constellations.size());

    std::getc(stdin);
}
