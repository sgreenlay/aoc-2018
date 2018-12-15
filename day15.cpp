#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <optional>
#include <queue>
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

struct character
{
    int id;
    point p;
    char team;

    int health = 200;
    bool dead = false;

    void hit(int dmg)
    {
        health -= dmg;
        if (health <= 0)
        {
            dead = true;
        }
    }

    bool operator< (const character& c) const
    {
        return (this->p < c.p);
    }

    bool operator== (const character& c) const
    {
        return (c.id == this->id);
    }

    bool operator!= (const character& c) const
    {
        return !(c == *this);
    }
};

struct scenario
{
    std::vector<std::string> map;
    std::vector<character> characters;

    character * getCharacter(point p)
    {
        for (int i = 0; i < characters.size(); ++i)
        {
            if (!characters[i].dead && characters[i].p == p)
            {
                return &(characters[i]);
            }
        }
        return nullptr;
    }
};

scenario scenarioFromFile(const char * path)
{
    scenario s;

    std::ifstream input;
    input.open(path);

    std::string line;

    int id = 0;
    int y = 0;
    while (std::getline(input, line))
    {
        for (int x = 0; x < line.length(); ++x)
        {
            char ch = line[x];
            if (ch == 'E' || ch == 'G')
            {
                s.characters.push_back(character{ id++, point{ x, y }, ch });
                line[x] = '.';
            }
        }
        s.map.push_back(line);
        y++;
    }
    input.close();

    return s;
}

void printMap(std::vector<std::string> map)
{
    for (auto && line : map)
    {
        printf("%s\n", line.c_str());
    }
}

void printScenario(scenario & s)
{
    for (int y = 0; y < s.map.size(); ++y)
    {
        auto line = s.map[y];
        
        std::vector<character> characters;
        
        for (int x = 0; x < line.length(); ++x)
        {
            auto p = point{ x, y };
            auto c = s.getCharacter(p);

            if (c)
            {
                characters.push_back(*c);

                auto character = c->team;
                printf("%c", character);
            }
            else
            {
                printf("%c", line[x]);
            }
        }

        printf(" ");
        for (auto & c : characters)
        {
            printf("%c(%d) ", c.team, c.health);
        }

        printf("\n");
    }
}

bool run_simulation(scenario scenario, int elf_attack, bool stop_on_elf_death)
{
    //printScenario(scenario);
    //printf("\n");

    int round = 0;
    char winner = ' ';

    while (winner == ' ')
    {
        for (auto && currentCharacter : scenario.characters)
        {
            // If the character is dead, it can't act
            if (currentCharacter.dead)
            {
                continue;
            }

            // If there are no more opponents, the character's team wins
            auto opponent_count = std::count_if(
                scenario.characters.begin(),
                scenario.characters.end(),
                [currentCharacter](auto character) {
                    return !character.dead && (character.team != currentCharacter.team);
                });

            if (opponent_count == 0)
            {
                winner = currentCharacter.team;
                break;
            }

            // Populate a map to perform pathfinding
            std::vector<std::string> map = scenario.map;
            for (auto & character : scenario.characters)
            {
                if (character.dead)
                {
                    continue;
                }

                if (character.team != currentCharacter.team)
                {
                    map[character.p.y][character.p.x] = 'X';
                }
                else
                {
                    map[character.p.y][character.p.x] = '#';
                }
            }
            map[currentCharacter.p.y][currentCharacter.p.x] = 'O';

            // Create a distance map to the closest opponent(s)
            std::map<point, int> distances;

            character * closest_target = nullptr;
            std::optional<int> closest_target_distance = {};

            std::queue<std::pair<point, int>> potentials;
            potentials.push({ currentCharacter.p, 0 });
            while (potentials.size() > 0)
            {
                auto p = potentials.front();

                if (closest_target_distance &&
                    closest_target_distance.value() < p.second)
                {
                    potentials.pop();
                    continue;
                }

                distances[p.first] = p.second;
                map[p.first.y][p.first.x] = 'v';

                point directions[] = {
                    point{ p.first.x, p.first.y - 1 },
                    point{ p.first.x - 1, p.first.y },
                    point{ p.first.x + 1, p.first.y },
                    point{ p.first.x, p.first.y + 1 }
                };

                for (auto & point : directions)
                {
                    if (distances.count(point))
                    {
                        continue;
                    }

                    int distance = p.second + 1;

                    if (point.y >= 0 && point.y < map.size() &&
                        point.x >= 0 && point.x < map[0].length())
                    {
                        switch (map[point.y][point.x])
                        {
                        case 'X':
                        {
                            auto target = scenario.getCharacter(point);
                            if (!closest_target ||
                                (distance == closest_target_distance.value() && point < closest_target->p))
                            {
                                closest_target = target;
                                closest_target_distance = distance;
                            }
                            distances[point] = distance;
                            break;
                        }
                        case '.':
                            potentials.push({ point, distance });
                            distances[point] = distance;
                            break;
                        case '#':
                            break;
                        default:
                            abort();
                            break;
                        }
                    }
                }

                potentials.pop();
            }

            if (!closest_target)
            {
                continue;
            }

            // Calculate the path to the closest target
            std::vector<point> path;
            point pt = closest_target->p;
            for (int i = closest_target_distance.value(); i > 1; --i)
            {
                point directions[] = {
                    point{ pt.x, pt.y - 1 },
                    point{ pt.x - 1, pt.y },
                    point{ pt.x + 1, pt.y },
                    point{ pt.x, pt.y + 1 }
                };

                std::optional<point> next_pt = {};
                for (auto & point : directions)
                {
                    if (distances.count(point))
                    {
                        if (distances[point] == i - 1)
                        {
                            map[point.y][point.x] = 'o';
                            next_pt = point;
                            break;
                        }
                    }
                }

                if (!next_pt)
                {
                    abort();
                }

                pt = next_pt.value();
                path.push_back(pt);
            }

            //printMap(map);
            //printf("\n");

            // Move the character one step along the path
            if (path.size() > 0)
            {
                currentCharacter.p = *(path.end() - 1);
            }

            if (path.size() > 1)
            {
                continue;
            }

            // Find the adjacent target with the lowest HP and hit it
            {
                point pt = currentCharacter.p;

                point directions[] = {
                    point{ pt.x, pt.y - 1 },
                    point{ pt.x - 1, pt.y },
                    point{ pt.x + 1, pt.y },
                    point{ pt.x, pt.y + 1 }
                };

                character * target = nullptr;
                for (auto & point : directions)
                {
                    auto potential_target = scenario.getCharacter(point);
                    if (potential_target && potential_target->team != currentCharacter.team)
                    {
                        if (!target)
                        {
                            target = potential_target;
                        }
                        else if (potential_target->health < target->health)
                        {
                            target = potential_target;
                        }
                        else if ((potential_target->health == target->health) &&
                            (potential_target->p < target->p))
                        {
                            target = potential_target;
                        }
                    }
                }

                if (!target)
                {
                    abort();
                }

                if (currentCharacter.team == 'E')
                {
                    target->hit(elf_attack);
                }
                else
                {
                    target->hit(3);
                }

                if (stop_on_elf_death && target->team == 'E' && target->dead)
                {
                    //printf("An elf died on round %d\n", round);
                    return false;
                }
            }
        }

        scenario.characters.erase(std::remove_if(
            scenario.characters.begin(),
            scenario.characters.end(),
            [](auto character) {
                return character.dead;
            }),
            scenario.characters.end());

        if (winner != ' ')
        {
            break;
        }

        std::sort(
            scenario.characters.begin(),
            scenario.characters.end());

        round++;

        //printf("Round %d\n", round);
        //printScenario(scenario);
        //printf("\n");
    }

    //printScenario(scenario);
    //printf("\n");

    int score = 0;
    for (auto && c : scenario.characters)
    {
        score += c.health;
    }
    printf("Game ends with %c victory after %d rounds\n", winner, round);
    printf("Score: %d\n", score * round);

    return true;
}

int main()
{
    auto scenario = scenarioFromFile("data/day15.txt");

    // Part 1
    run_simulation(scenario, 3, false);

    // Part 2
    int elf_power = 3;
    bool elves_won = false;
    while (!elves_won)
    {
        elf_power++;
        elves_won = run_simulation(scenario, elf_power, true);
    }
    printf("Elves won with an attack power of %d\n", elf_power);

    std::getc(stdin);
}
