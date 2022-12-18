#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <algorithm>

enum valveColor { white, grey, black };

struct Valve 
{
  int flowRate;
  std::string name;
  bool open;
  int color;
  std::vector<Valve*> adjacentValves;
};

struct WeightedValve
{
    int score;
    Valve *valve;
};

struct compare
{
  bool operator()(const WeightedValve* a, const WeightedValve* b)
  {
    return a->score > b->score;
  }
};

bool compareWeighedValve(WeightedValve a, WeightedValve b)
{
    return a.score > b.score;
}

bool compareValve(Valve a, Valve b)
{
    return a.flowRate > b.flowRate;
}

void printValve(Valve v)
{
    std::cout << "Valve " << v.name << " is open:" << v.open;
    std::cout << " Adjacent Valves: ";

    for (auto& adjacent : v.adjacentValves)
    {
        std::cout << adjacent->name << ",";
    }
    std::cout << std::endl;
}

Valve parseValveInput(std::string &line, std::vector<std::string> &neighbours)
{
    std::stringstream ss(line);
    std::string word;
    Valve valve = { 0, "AA", true, white };

    std::getline(ss, word, ' ');
    std::getline(ss, word, ' ');

    valve.name = word;

    std::getline(ss, word, ' ');
    std::getline(ss, word, ' ');
    std::getline(ss, word, ' ');

    word.pop_back();
    valve.flowRate = stoi(word.substr(5, word.size() - 5));

    std::getline(ss, word, ' ');
    std::getline(ss, word, ' ');
    std::getline(ss, word, ' ');
    std::getline(ss, word, ' ');

    while(std::getline(ss, word, ' '))
    {
        neighbours.push_back(word.substr(0, 2));
    }

    return valve;
}

void readInput(std::vector<Valve> &valves, std::string &filename)
{
    std::ifstream file(filename);
    std::string line;
    std::vector<std::vector<std::string>> valveNeighbours;

    while(std::getline(file, line))
    {
        std::vector<std::string> neighbours;
        Valve vl = parseValveInput(line, neighbours);

        valves.push_back(vl);
        valveNeighbours.push_back(neighbours);
    }

    for (int i = 0; i < valves.size(); i++)
    {
        for (auto& neighbour : valveNeighbours[i])
        {
            for(auto& valve : valves)
            {
                if (valve.name == neighbour)
                {
                    valves[i].adjacentValves.push_back(&valve);
                    break;
                }
            }
        }
    }
}

void buildDistanceMap (std::map<std::string, int> &map, Valve &valve, const Valve &originalValve, int count)
{   
    for (auto &neighbour : valve.adjacentValves)
    {
        if ( map.count(originalValve.name + neighbour->name) <= 0 )
        {
            map.insert({ originalValve.name + neighbour->name, count });
            map.insert({ neighbour->name + originalValve.name, count });


            if (neighbour->name != originalValve.name)
            {
                buildDistanceMap(map, *neighbour, originalValve, count + 1);
            }

        }
        else if (map.at(originalValve.name + neighbour->name) > count)
        {
            map[originalValve.name + neighbour->name] = count;
            map[neighbour->name + originalValve.name] = count;
        }
        else if (map.at(originalValve.name + neighbour->name) == count)
        {
            buildDistanceMap(map, *neighbour, originalValve, count + 1);
        }
    }
}

std::map<std::string, int> calculateValveDistances(std::vector<Valve> &valves)
{
    std::map<std::string, int> map;

    for(auto& valve : valves)
    {
        buildDistanceMap(map, valve, valve, 1);
    }

    return map;
}

int visitValve (Valve &valve, int time)
{
    time++; //Tunnel traversal time
    valve.color = grey;

    return 0;
}


// WeightedValve setValveWeights(Valve &valve, std::map<std::string, int> &valveDistance, std::vector<WeightedValve> &sortedWeightedValves)
// {

//     for (auto & weightedValve : sortedWeightedValves)
//     {

//     }
// }
// float proximityScore (Valve &valve, std::map<std::string, int> &valveDistances, std::vector<WeightedValve> &weightedValves)
// {
//     float proximityScore = 0;
//     // std::cout << valve.name << " options with weights: ";
//     for (int i = 0; i < weightedValves.size(); i++)
//     {
//         if (weightedValves[i].valve->name != valve.name && weightedValves[i].valve->open)
//         {
//             // std::cout << valve.name + weightedValves[i].valve->name << " ";
//             if (valveDistances.count(valve.name + weightedValves[i].valve->name) > 0)
//             {
//                 proximityScore += weightedValves[i].valve->flowRate / valveDistances.at(valve.name + weightedValves[i].valve->name);
//                 // std::cout << weightedValves[i].valve->name << " flow:" <<weightedValves[i].valve->flowRate << " dist:" << valveDistances.at(valve.name + weightedValves[i].valve->name) << ",  ";
//             }
//         }
//     }
//     // std::cout << " prox: " << proximityScore << std::endl;

//     return proximityScore;
// }
std::vector<bool> buildOpenValveList (std::vector<Valve> &workingValves)
{
    std::vector<bool> openValves;

    for(auto& valve: workingValves)
    {
        openValves.push_back(valve.open);
    }

    return openValves;
}

int findValve(Valve searchValve, const std::vector<Valve> &valves)
{
    for (int i = 0; i < valves.size(); i++)
    {
        if (valves[i].name == searchValve.name)
        {
            return i;
        }
    }

    return 0;
}

int findBestFlow (Valve &startValve, std::vector<Valve> workingValves, const std::map<std::string, int> valveDistances, 
    std::map<std::tuple<std::string, int, std::vector<bool>>, int> &stateMap, const int minutesRemaining)
{
    std::vector<bool> openValves = buildOpenValveList(workingValves);

    // if theres already a value saved then return that.
    if (stateMap.count({ startValve.name, minutesRemaining, openValves}) > 0)
    {
        std::cout << startValve.name << " at " << minutesRemaining <<  " is " << stateMap.at({ startValve.name, minutesRemaining, openValves}) << std::endl;
        return stateMap.at({ startValve.name, minutesRemaining, openValves});
    }

    // If no time remaining, return 0;
    if (minutesRemaining - 1 <= 0)
    {
        stateMap.insert({{ startValve.name, minutesRemaining, openValves}, 0});
        return 0;
    }

    int maxValue = 0;
    int minutes = minutesRemaining - 1; // Time needed to open the valve

    int startIndex = findValve(startValve, workingValves);
    workingValves[startIndex].open = false;
    openValves[startIndex] = false;

    for (auto &valve : workingValves)
    {
        if (valveDistances.count(startValve.name + valve.name) <= 0)
        {
            continue;
        }
        int val = 0;
        int distance = valveDistances.at(startValve.name + valve.name);

        if (valve.open && minutes - distance > 0)
        {
            std::cout << startValve.name << " -> " << valve.name << std::endl;
            val = findBestFlow(valve, workingValves, valveDistances, stateMap, minutes - distance);

            if (val > maxValue)
            {
                maxValue = val;
            }
        }
    }

    int total = startValve.flowRate * minutes + maxValue;
    stateMap.insert({{ startValve.name, minutesRemaining, openValves}, total});

    return total;
}

void maxFlow(Valve &startValve, std::vector<Valve> &valves, int minutes)
{
    std::map<std::string, int> valveDistances = calculateValveDistances(valves);
    std::vector<Valve> workingValves;
    std::map<std::tuple<std::string, int, std::vector<bool>>, int> stateMap;
    int nonZeroFlowValves = 0;

    for (auto& valve : valves)
    {
        if (valve.flowRate != 0)
        {
            // weighedValves.push_back({ valve.flowRate, &valve });
            workingValves.push_back(valve);
            nonZeroFlowValves++;
        }
    }
    std::sort(workingValves.begin(), workingValves.end(), compareValve);


    // for(const auto& elem : valveDistances)
    // {
    // std::cout << elem.first << " " << elem.second << "\n";
    // }
    // for( auto& wv : workingValves)
    // {
    //     std::cout << wv.name << " flow: " << wv.flowRate << " distance:" << valveDistances.at(startValve.name + wv.name) << std::endl;
    // }

    int val = findBestFlow(startValve, workingValves, valveDistances, stateMap, minutes);
    std::cout << val << std::endl;
    

    //More of an estimate instead of actual answer
    // int currentMinutes = minutes;
    // int totalFlow = 0;
    // Valve *currentValve = &startValve;

    // while (currentMinutes > 0 && nonZeroFlowValves > 0)
    // {
    //     float bestScore = 0;
    //     int bestScoreIndex = -1;
    //     int bestMinuteReduction = 0;

    //     for (int i = 0; i < weighedValves.size(); i++)
    //     {
    //         if (!weighedValves[i].valve->open || valveDistances.count(currentValve->name + weighedValves[i].valve->name) <= 0)
    //         {
    //             continue;
    //         }

    //         int minsUp = currentMinutes - valveDistances.at(currentValve->name + weighedValves[i].valve->name) - 1;
    //         if (minsUp <= 0)
    //         {
    //             continue;
    //         }
    //         float score = ((weighedValves[i].valve->flowRate * minsUp) / valveDistances.at(currentValve->name + weighedValves[i].valve->name)) 
    //             +  proximityScore(*weighedValves[i].valve, valveDistances, weighedValves);

    //         std::cout << weighedValves[i].valve->name << " " << score<< std::endl;

    //         if (score > bestScore)
    //         {
    //             bestScore = score;
    //             bestScoreIndex = i;
    //             bestMinuteReduction = valveDistances.at(currentValve->name + weighedValves[i].valve->name) + 1;
    //         }
    //     }

    //     if (bestScoreIndex == -1)
    //     {
    //         break;
    //     }

    //     currentValve = weighedValves[bestScoreIndex].valve;
    //     weighedValves[bestScoreIndex].valve->open = false;
    //     currentMinutes -= bestMinuteReduction;
    //     std::cout << weighedValves[bestScoreIndex].valve->name << " chosen at " << currentMinutes << "mins with flow rate: " << weighedValves[bestScoreIndex].valve->flowRate << std::endl;
    //     nonZeroFlowValves--;
    //     totalFlow += weighedValves[bestScoreIndex].valve->flowRate * currentMinutes;
    // }
    // std::cout << totalFlow << std::endl;
}

int main (int argc, char** argv)
{
    std::string filename(argv[1]);
    std::vector<Valve> valves;
    readInput(valves, filename);

    // for (auto& valve : valves)
    // {
    //     printValve(valve);
    // }
    maxFlow(valves[0], valves, 30);
}