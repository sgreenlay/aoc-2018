
#include <string>

#include <vector>
#include <map>

int powerLevel(int x, int y, int sn)
{
    // Find the fuel cell's rack ID, which is its X coordinate plus 10.
    int rack_id = x + 10;

    // Begin with a power level of the rack ID times the Y coordinate.
    int power = rack_id * y;

    // Increase the power level by the value of the grid serial number.
    power += sn;

    // Set the power level to itself multiplied by the rack ID.
    power *= rack_id;

    // Keep only the hundreds digit of the power level.
    power = (int)((power % 1000) / 100);

    // Subtract 5 from the power level.
    power -= 5;

    return power;
}

int main()
{
    int sn = 9810;
    int cells[300][300] = { 0 };

    for (int y = 1; y <= 300; ++y)
    {
        for (int x = 1; x <= 300; ++x)
        {
            int ix = x - 1;
            int iy = y - 1;

            cells[iy][ix] = powerLevel(x, y, sn);
        }
    }

    int xMax = 0;
    int yMax = 0;
    int max = 0;

    for (int y = 1; y < 299; ++y)
    {
        for (int x = 1; x < 299; ++x)
        {
            int sum =
                cells[y - 1][x - 1] +
                cells[y - 1][x] +
                cells[y - 1][x + 1] +
                cells[y][x - 1] +
                cells[y][x] +
                cells[y][x + 1] +
                cells[y + 1][x - 1] +
                cells[y + 1][x] +
                cells[y + 1][x + 1];

            if (sum > max)
            {
                xMax = x;
                yMax = y;
                max = sum;
            }
        }
    }

    printf("Max power of %d starting at (%d, %d)\n", max, xMax, yMax);

    std::getc(stdin);
}
