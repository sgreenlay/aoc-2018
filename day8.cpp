
#include <fstream>
#include <sstream>
#include <string>

#include <vector>

struct node
{
    int child_count;
    int metadata_count;
    std::vector<node> children;
    std::vector<int> metadata;
};

node nodeFromString(const std::string string, int & start)
{
    node n;

    auto end = string.find(" ", start);

    n.child_count = std::stoi(string.substr(start, end - start));

    start = end + 1;
    end = string.find(" ", start);

    n.metadata_count = std::stoi(string.substr(start, end - start));

    start = end + 1;

    for (int c = 0; c < n.child_count; ++c)
    {
        n.children.push_back(nodeFromString(string, start));
    }

    for (int m = 0; m < n.metadata_count; ++m)
    {
        end = string.find(" ", start);

        n.metadata.push_back(std::stoi(string.substr(start, end - start)));

        start = end + 1;
    }

    return n;
}

node treeFromFile(const char * path)
{
    std::ifstream input;
    input.open(path);

    std::string line;
    std::getline(input, line);
    input.close();

    int start = 0;
    return nodeFromString(line, start);
}

int metadataSumForNode(const node n)
{
    int sum = 0;

    for (auto && child : n.children)
    {
        sum += metadataSumForNode(child);
    }

    for (auto && metadata : n.metadata)
    {
        sum += metadata;
    }

    return sum;
}

int valueForNode(const node n)
{
    int value = 0;

    if (n.child_count == 0)
    {
        for (auto && metadata : n.metadata)
        {
            value += metadata;
        }
    }
    else
    {
        for (auto && metadata : n.metadata)
        {
            if (metadata == 0)
            {
                continue;
            }

            int idx = metadata - 1;
            if (idx < n.child_count)
            {
                value += valueForNode(n.children[idx]);
            }
        }
    }

    return value;
}

int main()
{
    auto n = treeFromFile("data/day8.txt");

    {
        int sum = metadataSumForNode(n);
        printf("Metadata sum: %d\n", sum);
    }

    {
        int value = valueForNode(n);
        printf("Value of root: %d\n", value);
    }

    std::getc(stdin);
}