var fs = require('fs');

var values = fs.readFileSync('day2.txt', 'utf8').split('\n');

function calculateChecksum(values)
{
    var twoCount = 0;
    var threeCount = 0;

    values.forEach(value => {
        var counts = new Object();

        for (var i = 0; i < value.length; i++) {
            if (counts[value.charAt(i)] == null)
            {
                counts[value.charAt(i)] = 0;
            }
            counts[value.charAt(i)]++;
        }

        var hasTwoCount = false;
        var hasThreeCount = false;
        Object.keys(counts).forEach(char => {
            if (counts[char] == 2)
            {
                hasTwoCount = true;
            }
            else if (counts[char] == 3)
            {
                hasThreeCount = true;
            }
        });

        if (hasTwoCount)
        {
            twoCount++;
        }
        if (hasThreeCount)
        {
            threeCount++;
        }
    });

    return twoCount * threeCount;
}

console.log(calculateChecksum(values));