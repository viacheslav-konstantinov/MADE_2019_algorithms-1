#include <iostream>
#include <vector>
#include <iomanip>
#include "boxMuellerGenerator.hpp"
#include "primMST.hpp"
#include "accuracyScore.hpp"

int main()
{
    std::cout << "Enter min and max number of vertexes to generate >> ";
    size_t minNumber, maxNumber;
    std::cin >> minNumber >> maxNumber;

    std::cout << std::setprecision(5);
    std::cout << "--------------------------------------------------------------------------------------\n";
    std::cout << " shortest calculated path = l_cal, MST length = l_min, ";
    std::cout << "l_cal/l_min = r\n";
    std::cout << " N\tmean(l_cal)\tstdVar(l_cal)\tmean(l_min)\tstdVar(l_min)\tmean(r)\t\tstdVar(r) \n";
    std::cout << "--------------------------------------------------------------------------------------\n";
    for(size_t numberOfPintsToGenerate = minNumber; numberOfPintsToGenerate <= maxNumber; ++numberOfPintsToGenerate)
    {
        std::vector<double> minPathLengths;
        std::vector<double> MSTlengths;
        std::vector<double> ratios;

        for(size_t i = 0; i < 100; ++i)
        {
            std::vector<std::pair<double, double>> randomPoints = boxMuellerGenerator(numberOfPintsToGenerate);

            Graph completeGraph(numberOfPintsToGenerate);
            std::vector<TVertex> listOfVertexes;
            for(size_t i = 0; i < numberOfPintsToGenerate; ++i)
                listOfVertexes.push_back(std::make_pair(std::make_pair(randomPoints.at(i).first, randomPoints.at(i).second), i));
            
            completeGraph.makeCompleteGraph(listOfVertexes);
            
            std::pair<std::pair<double /*primMST*/, double /*lengthOfThePath*/>, std::vector<size_t> /*path*/> resultMinPathDFS = completeGraph.getMinPathDFS();

            minPathLengths.push_back(resultMinPathDFS.first.second);
            MSTlengths.push_back(resultMinPathDFS.first.first);
            ratios.push_back(resultMinPathDFS.first.second / resultMinPathDFS.first.first);

        }

        std::pair<double, double> minPathStats = accuracyScore(minPathLengths);
        std::pair<double, double> MSTlengthsStats = accuracyScore(MSTlengths);
        std::pair<double, double> ratiosStats = accuracyScore(ratios);

        std::cout << numberOfPintsToGenerate << "\t";
        std::cout << minPathStats.first <<  "\t\t"; 
        std::cout << minPathStats.second << "\t";
        std::cout << MSTlengthsStats.first << "\t\t";
        std::cout << MSTlengthsStats.second << "\t";
        std::cout << ratiosStats.first << "\t\t"; 
        std::cout << ratiosStats.second << std::endl;
    }
    return 0;
}