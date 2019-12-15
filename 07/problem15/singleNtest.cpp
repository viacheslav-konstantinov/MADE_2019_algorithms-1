#include <iostream>
#include <vector>
#include "boxMuellerGenerator.hpp"
#include "primMST.hpp"
#include "accuracyScore.hpp"

int main()
{
    std::cout << "Enter number of vertexes to generate >> ";
    size_t numberOfPintsToGenerate = 0;

    std::cin >> numberOfPintsToGenerate;

    std::vector<double> minPathLengths;
    std::vector<double> MSTlengths;
    std::vector<double> ratios;

    for(size_t i = 0; i < 100; ++i)
    {
        std::cout << "Iteration #" << i + 1 << std::endl;

        std::cout << "Generating array of (x, y) points...\n";
        std::vector<std::pair<double, double>> randomPoints = boxMuellerGenerator(numberOfPintsToGenerate);

        std::cout << "Generating a complete graph with the generated random points...\n";
        Graph completeGraph(numberOfPintsToGenerate);
        std::vector<TVertex> listOfVertexes;
        for(size_t i = 0; i < numberOfPintsToGenerate; ++i)
            listOfVertexes.push_back(std::make_pair(std::make_pair(randomPoints.at(i).first, randomPoints.at(i).second), i));

        
        completeGraph.makeCompleteGraph(listOfVertexes);
        std::cout << "Generation is complete!\n";
        
        std::pair<std::pair<double /*primMST*/, double /*lengthOfThePath*/>, std::vector<size_t> /*path*/> resultMinPathDFS = completeGraph.getMinPathDFS();

        std::cout << "Min path route length: ";
        std::cout << resultMinPathDFS.first.second << std::endl;

        std::cout << "Sum of the distances of the MST: ";
        std::cout << resultMinPathDFS.first.first << std::endl;

        std::cout << "(Min path route length) /(Sum of the distances of the MST) ratio:" << std::endl;
        std::cout << resultMinPathDFS.first.second / resultMinPathDFS.first.first << std::endl;

        minPathLengths.push_back(resultMinPathDFS.first.second);
        MSTlengths.push_back(resultMinPathDFS.first.first);
        ratios.push_back(resultMinPathDFS.first.second / resultMinPathDFS.first.first);
        std::cout << "-------------------------------------------------------------------------------\n";

    }

    std::pair<double, double> minPathStats = accuracyScore(minPathLengths);
    std::pair<double, double> MSTlengthsStats = accuracyScore(MSTlengths);
    std::pair<double, double> ratiosStats = accuracyScore(ratios);

    std::cout << "Statistics on shortest path, MST length, and their ratio:\n";
    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "Shortest path: mean = " << minPathStats.first << ", standart deviation = " << minPathStats.second << std::endl;
    std::cout << "MST length: mean = " << MSTlengthsStats.first << ", standart deviation = " << MSTlengthsStats.second << std::endl;
    std::cout << "(Shortest path) / (MST length): mean = " << ratiosStats.first << ", standart deviation = " << ratiosStats.second << std::endl;
    std::cout << "-------------------------------------------------------------------------------\n";

    return 0;
}