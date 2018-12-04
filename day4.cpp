
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <algorithm>
#include <map>

enum class operation
{
    Unknown,
    ShiftChange,
    Sleep,
    Wake
};

struct log_entry
{
    int y;
    int m;
    int d;
    int hh;
    int mm;
    operation op;
    int id;

    bool operator< (const log_entry& entry) const
    {
        if (entry.y != this->y) return (entry.y > this->y);
        if (entry.m != this->m) return (entry.m > this->m);
        if (entry.d != this->d) return (entry.d > this->d);
        if (entry.hh != this->hh) return (entry.hh > this->hh);
        return (entry.mm > this->mm);
    }
};

log_entry logEntryFromString(const std::string string)
{
    // [1518-11-01 00:00] Guard #10 begins shift
    // [1518-11-01 00:05] falls asleep
    // [1518-11-01 00:25] wakes up

    auto start = string.find("[") + 1;
    auto end = string.find("-");

    auto y = string.substr(start, end - start);

    start = end + 1;
    end = string.find("-", start);

    auto m = string.substr(start, end - start);

    start = end + 1;
    end = string.find(" ", start);

    auto d = string.substr(start, end - start);

    start = end + 1;
    end = string.find(":", start);

    auto hh = string.substr(start, end - start);

    start = end + 1;
    end = string.find("]", start);

    auto mm = string.substr(start, end - start);

    if (string.find("#", end) != std::string::npos)
    {
        start = string.find("#") + 1;
        end = string.find(" begins shift");
        auto id = string.substr(start, end - start);

        return log_entry{
            std::stoi(y),
            std::stoi(m),
            std::stoi(d),
            std::stoi(hh),
            std::stoi(mm),
            operation::ShiftChange,
            std::stoi(id)
        };
    }
    else if (string.find("falls asleep", end) != std::string::npos)
    {
        return log_entry{
            std::stoi(y),
            std::stoi(m),
            std::stoi(d),
            std::stoi(hh),
            std::stoi(mm),
            operation::Sleep,
            0
        };
    }
    else if (string.find("wakes up", end) != std::string::npos)
    {
        return log_entry{
            std::stoi(y),
            std::stoi(m),
            std::stoi(d),
            std::stoi(hh),
            std::stoi(mm),
            operation::Wake,
            0
        };
    }

    abort();
}

std::vector<log_entry> logFromFile(const char * path)
{
    std::vector<log_entry> logEntries;

    std::ifstream input;
    input.open(path);

    std::string line;
    while (std::getline(input, line))
    {
        logEntries.push_back(logEntryFromString(line));
    }
    input.close();

    std::sort(logEntries.begin(), logEntries.end());
    return logEntries;
}

int main()
{
    auto log = logFromFile("data/day4.txt");
    
    std::map<int, std::pair<int[60], int>> sleepLogs;
    {
        int lastTime = 0;
        operation lastState = operation::Unknown;
        int lastId = 0;

        for (auto && entry : log)
        {
            if (entry.op == operation::ShiftChange)
            {
                if ((lastState != operation::Unknown) &&
                    (lastState != operation::ShiftChange) &&
                    (lastState != operation::Wake))
                {
                    abort();
                }

                lastId = entry.id;
            }
            else if (entry.op == operation::Sleep)
            {
                if ((lastState != operation::ShiftChange) &&
                    (lastState != operation::Wake))
                {
                    abort();
                }
            }
            else if (entry.op == operation::Wake)
            {
                if (lastState != operation::Sleep)
                {
                    abort();
                }

                for (int t = lastTime; t < entry.mm; ++t)
                {
                    sleepLogs[lastId].first[t]++;
                    sleepLogs[lastId].second++;
                }
            }

            lastState = entry.op;
            lastTime = entry.mm;
        }
    }

    {
        int id = 0;
        int totalSleep = 0;

        for (auto && sleepLog : sleepLogs)
        {
            if (sleepLog.second.second > totalSleep)
            {
                id = sleepLog.first;
                totalSleep = sleepLog.second.second;
            }
        }

        {
            auto sleepLog = sleepLogs[id];

            int sleepCount = 0;
            int time = 0;

            for (int t = 0; t < 60; ++t)
            {
                if (sleepLog.first[t] > sleepCount)
                {
                    time = t;
                    sleepCount = sleepLog.first[t];
                }
            }

            printf("Guard %d slept the most during 00:%02d (Answer: %d))\n", id, time, id * time);
        }
    }

    {
        int id = 0;
        int sleepCount = 0;
        int time = 0;

        for (auto && sleepLog : sleepLogs)
        {
            for (int t = 0; t < 60; ++t)
            {
                if (sleepLog.second.first[t] > sleepCount)
                {
                    id = sleepLog.first;
                    time = t;
                    sleepCount = sleepLog.second.first[t];
                }
            }
        }

        printf("Guard %d slept the most during 00:%02d (Answer: %d))\n", id, time, id * time);
    }

    std::getc(stdin);
}