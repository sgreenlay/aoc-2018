
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <set>
#include <algorithm>

enum class team
{
    none,
    infection,
    immune_system
};

struct group
{
    int id;

    long long count;
    long long hp;
    std::set<std::string> weak;
    std::set<std::string> immune;
    long long attack;
    std::string attack_type;
    long long initiative;

    void init()
    {
        _units = count;
    }

    int remaining_units() const
    {
        return _units;
    }

    long long effective_power() const
    {
        return remaining_units() * attack;
    }

    bool alive() const
    {
        return remaining_units() > 0;
    }

    long long damage_to(group opponent)
    {
        // By default, an attacking group would deal damage equal to its effective power 
        // to the defending group. However, if the defending group is immune to the 
        // attacking group's attack type, the defending group instead takes no damage; 
        // if the defending group is weak to the attacking group's attack type, the 
        // defending group instead takes double damage.
        
        if (opponent.immune.count(attack_type) > 0)
        {
            return 0;
        }

        long long damage = effective_power();

        if (opponent.weak.count(attack_type) > 0)
        {
            damage *= 2;
        }

        return damage;
    }

    long long deal_damage(long long damage)
    {
        long long killed = (int)std::floor(damage / hp);

        if (killed > _units)
        {
            killed = _units;
        }

        _units -= killed;
        return killed;
    }

private:
    long long _units;
};

group groupFromString(std::string string)
{
    group g;

    // 17 units each with 5390 hit points(weak to radiation, bludgeoning) with an attack that does 4507 fire damage at initiative 2

    auto start = 0;
    auto end = string.find(" units");

    g.count = std::stoll(string.substr(start, end - start));

    start = string.find("with ", end) + 5;
    end = string.find(" hit points", start);

    g.hp = std::stoll(string.substr(start, end - start));

    auto weaknesses = string.find("weak to ", end);
    auto immunities = string.find("immune to ", end);

    if (weaknesses != std::string::npos)
    {
        auto weaknesses_end = string.find(";", weaknesses);
        if (weaknesses_end == std::string::npos)
        {
            weaknesses_end = string.find(")", weaknesses);
        }

        start = weaknesses + 8;

        while (start < weaknesses_end)
        {
            end = string.find(", ", start);
            if (end == std::string::npos || end > weaknesses_end)
            {
                end = weaknesses_end;
            }

            g.weak.emplace(string.substr(start, end - start));
            start = end + 2;
        }
    }

    if (immunities != std::string::npos)
    {
        auto immunities_end = string.find(";", immunities);
        if (immunities_end == std::string::npos)
        {
            immunities_end = string.find(")", immunities);
        }

        start = immunities + 10;

        while (start < immunities_end)
        {
            end = string.find(", ", start);
            if (end == std::string::npos || end > immunities_end)
            {
                end = immunities_end;
            }

            g.immune.emplace(string.substr(start, end - start));
            start = end + 2;
        }
    }

    start = string.find("an attack that does ") + 20;;
    end = string.find(" ", start);

    g.attack = std::stoll(string.substr(start, end - start));

    start = end + 1;
    end = string.find(" damage", start);

    g.attack_type = string.substr(start, end - start);

    start = string.find("initiative ", end) + 11;

    g.initiative = std::stoll(string.substr(start));

    return g;
}

struct scenario
{
    std::vector<group> immune;
    std::vector<group> infection;
};

scenario scenarioFromFile(const char * path)
{
    scenario s;

    std::ifstream input;
    input.open(path);

    std::string line;

    std::getline(input, line);
    if (line != "Immune System:")
    {
        abort();
    }

    int id = 0;
    while (std::getline(input, line) && (line.length() > 0))
    {
        auto g = groupFromString(line);

        g.id = ++id;
        g.init();

        s.immune.push_back(g);
    }

    std::getline(input, line);
    if (line != "Infection:")
    {
        abort();
    }

    id = 0;
    while (std::getline(input, line) && (line.length() > 0))
    {
        auto g = groupFromString(line);

        g.id = ++id;
        g.init();

        s.infection.push_back(g);
    }

    input.close();

    return s;
}

std::vector<int> target_choice_order(
    const std::vector<group>& attackers)
{
    std::vector<int> attacker_idxs;
    for (int i = 0; i < attackers.size(); ++i)
    {
        attacker_idxs.push_back(i);
    }

    std::sort(attacker_idxs.begin(), attacker_idxs.end(), 
        [attackers](const int& idx_a, const int& idx_b)
        {
            group a = attackers[idx_a];
            group b = attackers[idx_b];

            auto a_effective = a.effective_power();
            auto b_effective = b.effective_power();

            if (a_effective == b_effective)
            {
                return a.initiative > b.initiative;
            }
            return a_effective > b_effective;
        });

    return attacker_idxs;
}

std::vector<std::tuple<long long, long long, long long>> targets(
    const team t,
    const std::vector<int>& attacker_idxs,
    const std::vector<group>& attackers,
    const std::vector<group>& defenders)
{
    std::vector<std::tuple<long long, long long, long long>> targets;
    std::set<int> chosen_targets;

    for (int i = 0; i < attacker_idxs.size(); ++i)
    {
        group attacker = attackers[attacker_idxs[i]];

        bool target_selected = false;
        int selected_target = 0;
        int selected_target_damage = 0;

        // The attacking group chooses to target the group in the enemy army to which 
        // it would deal the most damage (after accounting for weaknesses and immunities,
        // but not accounting for whether the defending group has enough units to
        // actually receive all of that damage).

        for (int j = 0; j < defenders.size(); ++j)
        {
            // Defending groups can only be chosen as a target by one attacking group.

            if (chosen_targets.count(j) > 0)
            {
                continue;
            }

            group defender = defenders[j];
            int damage = attacker.damage_to(defender);

            if (damage == 0)
            {
                continue;
            }

            bool replace_as_target = false;
            if (!target_selected)
            {
                replace_as_target = true;
            }
            else if (damage > selected_target_damage)
            {
                replace_as_target = true;
            }
            else if (damage == selected_target_damage)
            {
                // If an attacking group is considering two defending groups to which 
                // it would deal equal damage, it chooses to target the defending group 
                // with the largest effective power;

                int power = defender.effective_power();
                int selected_target_power = defenders[selected_target].effective_power();

                if (power > selected_target_power)
                {
                    replace_as_target = true;
                }

                // if there is still a tie, it chooses the defending group with the 
                // highest initiative. 

                else if (power == selected_target_power)
                {
                    int initiative = defender.initiative;
                    int selected_target_initiative = defenders[selected_target].initiative;

                    if (initiative > selected_target_initiative)
                    {
                        replace_as_target = true;
                    }
                }
            }

            if (replace_as_target)
            {
                selected_target = j;
                selected_target_damage = damage;
                target_selected = true;
            }
        }

        if (target_selected)
        {
            printf("%s group %d would deal defending group %d %d damage\n",
                t == team::immune_system ? "Immune System" : "Infection",
                attacker.id, defenders[selected_target].id, attacker.damage_to(defenders[selected_target]));

            targets.push_back({ i, selected_target, selected_target_damage });
            chosen_targets.emplace(selected_target);
        }
    }

    return targets;
}

int main()
{
    auto s = scenarioFromFile("data/day24.txt");

    while ((s.immune.size() > 0) && (s.infection.size() > 0))
    {
        // Each fight consists of two phases: target selection and attacking.

        // During the target selection phase, each group attempts to choose one target.
        // In decreasing order of effective power, groups choose their targets; in a tie, 
        // the group with the higher initiative chooses first.

        auto infection = target_choice_order(s.infection);
        auto immune = target_choice_order(s.immune);

        auto infection_targets = targets(
            team::infection, infection, s.infection, s.immune);
        auto immune_targets = targets(
            team::immune_system, immune, s.immune, s.infection);

        printf("************ \n");

        // During the attacking phase, each group deals damage to the target it selected, 
        // if any. Groups attack in decreasing order of initiative, regardless of whether 
        // they are part of the infection or the immune system. (If a group contains no units, 
        // it cannot attack.)

        std::vector<std::tuple<team, long long, long long>> attacks;
        for (auto t : infection_targets)
        {
            attacks.push_back({ team::infection, std::get<0>(t), std::get<1>(t) });
        }
        for (auto t : immune_targets)
        {
            attacks.push_back({ team::immune_system, std::get<0>(t), std::get<1>(t) });
        }

        auto get_attacker_group = [&s](
            std::tuple<team, long long, long long>& attack)
        {
            auto group = std::get<0>(attack);
            auto idx = std::get<1>(attack);

            if (group == team::immune_system)
            {
                return &s.immune[idx];
            }
            else if (group == team::infection)
            {
                return &s.infection[idx];
            }
            abort();
        };

        auto get_defender_group = [&s](
            std::tuple<team, long long, long long>& attack)
        {
            auto enemy_group = std::get<0>(attack);
            auto idx = std::get<2>(attack);

            if (enemy_group == team::immune_system)
            {
                return &s.infection[idx];
            }
            else if (enemy_group == team::infection)
            {
                return &s.immune[idx];
            }
            abort();
        };

        std::sort(attacks.begin(), attacks.end(),
            [s, get_attacker_group](
                std::tuple<team, long long, long long>& attack_a,
                std::tuple<team, long long, long long>& attack_b)
            {
                auto a = get_attacker_group(attack_a);
                auto b = get_attacker_group(attack_b);

                return a->initiative > b->initiative;
            });

        for (auto attack : attacks)
        {
            auto attacker = get_attacker_group(attack);

            if (!attacker->alive())
            {
                continue;
            }

            auto defender = get_defender_group(attack);
            auto damage = attacker->damage_to(*defender);

            auto t = std::get<0>(attack);
            auto killed = defender->deal_damage(damage);

            printf("%s group %d attacks defending group %d, killing %lld units\n",
                t == team::immune_system ? "Immune System" : "Infection",
                attacker->id, defender->id, killed);
        }
        printf("============ \n");

        s.immune.erase(std::remove_if(s.immune.begin(), s.immune.end(), [](group g) {
            return !g.alive();
        }), s.immune.end());
        s.infection.erase(std::remove_if(s.infection.begin(), s.infection.end(), [](group g) {
            return !g.alive();
        }), s.infection.end());
    }

    if (s.infection.size() == 0)
    {
        long long sum = 0;

        printf("Immune System:\n");
        for (auto g : s.immune)
        {
            sum += g.remaining_units();
            printf("Group %d contains %lld units\n", g.id, g.remaining_units());
        }
        printf("Sum: %lld\n", sum);
    }
    else if (s.immune.size() == 0)
    {
        long long sum = 0;

        printf("Infection:\n");
        for (auto g : s.infection)
        {
            sum += g.remaining_units();
            printf("Group %d contains %lld units\n", g.id, g.remaining_units());
        }
        printf("Sum: %lld\n", sum);
    }
    else
    {
        abort();
    }

    std::getc(stdin);
}
