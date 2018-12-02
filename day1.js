var fs = require('fs');

var values = fs.readFileSync('day1.txt', 'utf8').split('\n').map(line => {
    return Number(line.replace(/[^0-9.-]+/g,""));
});

console.log("Part 1");

function calculateFrequency(values)
{
    return values.reduce((accumulator, value) => {
        return accumulator + value;
    });
}

console.log(calculateFrequency(values));

console.log("Part 2");

function findFirstRepeatFrequency(values)
{
    var sum = 0;
    var seen = [sum];

    var i = 0;
    while (true)
    {
        if (i == values.length)
        {
            i = 0;
        }

        sum += values[i];
        if (seen.includes(sum))
        {
            return sum;
        }
        seen.push(sum);

        i++;
    }
}

console.log(findFirstRepeatFrequency(values));
