
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
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

    int score = 0;

    int player = 0;
    std::vector<int> marbles;
    std::map<int, int> scores;

    int idx = 0;
    marbles.push_back(0);

    for (int marble = 1; marble <= game.last_marble; ++marble)
    {
        if (marble % 23 != 0)
        {
            // Each Elf takes a turn placing the lowest - numbered remaining 
            // marble into the circle between the marbles that are 1 and 2 
            // marbles clockwise of the current marble.

            int pos = (idx + 1) % marbles.size();
            marbles.insert(marbles.begin() + pos + 1, marble);
            idx = pos + 1;
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

            if (idx < 7)
            {
                idx = marbles.size() + idx - 7;
            }
            else
            {
                idx = idx - 7;
            }

            score += marbles[idx];
            marbles.erase(marbles.begin() + idx);

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
        for (int i = 0; i < marbles.size(); ++i)
        {
            if (i == idx)
            {
                printf("(%d) ", marbles[i]);
            }
            else
            {
                printf("%d ", marbles[i]);
            }
        }
        printf("\n");
        */

        player = (player + 1) % game.players;
    }

    int high_score = 0;
    for (auto && score : scores)
    {
        if (score.second > high_score)
        {
            high_score = score.second;
        }
    }

    printf("High score: %d\n", high_score);

    std::getc(stdin);
}
