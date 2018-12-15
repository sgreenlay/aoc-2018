
#include <fstream>
#include <sstream>
#include <string>

#include <list>
#include <map>

struct parameters
{
    int players;
    int last_marble;
};

parameters parametersFromFile(const char * path)
{
    std::ifstream input;
    input.open(path);

    std::string string;
    std::getline(input, string);
    input.close();

    int start = 0;
    int end = string.find(" ", start);

    int players = std::stoi(string.substr(start, end - start));

    start = string.find("worth ", start) + 6;
    end = string.find(" ", start);

    int last_marble = std::stoi(string.substr(start, end - start));

    return parameters{
        players,
        last_marble
    };
}

int main()
{
    auto game = parametersFromFile("data/day9.txt");

    // Part 2
    game.last_marble *= 100;

    int score = 0;

    int player = 0;
    std::list<int> marbles;
    std::map<int, unsigned int> scores;

    marbles.push_back(0);
    auto pos = marbles.begin();

    for (int marble = 1; marble <= game.last_marble; ++marble)
    {
        if (marble % 23 != 0)
        {
            // Each Elf takes a turn placing the lowest - numbered remaining 
            // marble into the circle between the marbles that are 1 and 2 
            // marbles clockwise of the current marble.

            for (int i = 0; i < 1; ++i)
            {
                pos++;

                if (pos == marbles.end())
                {
                    pos = marbles.begin();
                }
            }
            if (pos != marbles.end()) {
                pos++;
            }
            pos = marbles.insert(pos, marble);
        }
        else
        {
            // If the marble that is about to be placed has a number which is 
            // a multiple of 23, something entirely different happens. First, 
            // the current player keeps the marble they would have placed,
            // adding it to their score. In addition, the marble 7 marbles 
            // counter-clockwise from the current marble is removed from the 
            // circle and also added to the current player's score. The marble 
            // located immediately clockwise of the marble that was removed 
            // becomes the new current marble.

            score = marble;

            for (int i = 0; i < 7; ++i)
            {
                if (pos == marbles.begin())
                {
                    pos = marbles.end();
                }
                pos--;
            }

            auto marble_to_remove = pos;
            pos++;

            score += *marble_to_remove;
            marbles.erase(marble_to_remove);

            if (scores.count(player) == 0)
            {
                scores[player] = score;
            }
            else
            {
                scores[player] += score;
            }
        }

        /*
        printf("[%d] ", player + 1);
        for (auto && m : marbles)
        {
            if (m == *pos)
            {
                printf("(%d) ", m);
            }
            else
            {
                printf("%d ", m);
            }
        }
        printf("\n");
        */

        player = (player + 1) % game.players;
    }

    unsigned int high_score = 0;
    for (auto && score : scores)
    {
        if (score.second > high_score)
        {
            high_score = score.second;
        }
    }

    printf("High score: %u\n", high_score);

    std::getc(stdin);
}
