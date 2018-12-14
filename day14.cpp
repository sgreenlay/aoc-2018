
#include <string>

#include <vector>
#include <map>

int main()
{
    std::vector<int> recipies = { 3, 7 };

    int e1 = 0;
    int e2 = 1;

    int recipe_count = 190221;

    while (recipies.size() < recipe_count + 10)
    {
        int v1 = recipies[e1];
        int v2 = recipies[e2];

        int sum = v1 + v2;

        if (sum >= 10)
        {
            recipies.push_back(sum / 10);
            recipies.push_back(sum % 10);
        }
        else
        {
            recipies.push_back(sum);
        }

        e1 = (e1 + v1 + 1) % recipies.size();
        e2 = (e2 + v2 + 1) % recipies.size();

        /*
        for (int i = 0; i < recipies.size(); ++i)
        {
            if (i == e1)
            {
                printf("(%d) ", recipies[i]);
            }
            else if (i == e2)
            {
                printf("[%d] ", recipies[i]);
            }
            else
            {
                printf("%d ", recipies[i]);
            }
        }
        printf("\n");
        */
    }

    for (int i = 0; i < 10; ++i)
    {
        printf("%d", recipies[recipe_count + i]);
    }
    printf("\n");

    //

    std::getc(stdin);
}
