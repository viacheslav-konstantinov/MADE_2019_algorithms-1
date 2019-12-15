#include <iostream>
#include <vector>
#include "boxMuellerGenerator.hpp"
#include "primMST.hpp"
#include "accuracyScore.hpp"


int main()
{
    
    std::cout << "Enter number of points to generate >> ";
    size_t numberOfPintsToGenerate = 0;

    std::cin >> numberOfPintsToGenerate;
    std::cout << "Generating array of (x, y) points...\n";
    std::vector<std::pair<double, double>> randomPoints = boxMuellerGenerator(numberOfPintsToGenerate);

    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "Generated array of points:\n";
    for(size_t i = 0; i < numberOfPintsToGenerate; ++i)
        std::cout << "i = " << i + 1 << ", x = " << randomPoints.at(i).first << ", y = " << randomPoints.at(i).second << std::endl;

    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "Calculating mean and standart deviation of x and y\n";
    std::pair<coordinates, coordinates> randomPointsStat =  accuracyScore(randomPoints);

    std::cout << "Mean and standart deviation of x and y:\n";
    std::cout << "meanX = " << randomPointsStat.first.first  << ", stdVarX = " << randomPointsStat.first.second  << std::endl;
    std::cout << "meanY = " << randomPointsStat.second.first << ", stdVarY = " << randomPointsStat.second.second << std::endl;

    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "Generating a complete graph with the generated random points...\n";
    Graph completeGraph(numberOfPintsToGenerate);
    std::vector<TVertex> listOfVertexes;
    for(size_t i = 0; i < numberOfPintsToGenerate; ++i)
        listOfVertexes.push_back(std::make_pair(std::make_pair(randomPoints.at(i).first, randomPoints.at(i).second), i));

    
    completeGraph.makeCompleteGraph(listOfVertexes);
    //completeGraph.print();
    std::cout << "Generation is complete!\n";
    
    std::pair<std::pair<double /*primMST*/, double /*lengthOfThePath*/>, std::vector<size_t> /*path*/> resultMinPathDFS = completeGraph.getMinPathDFS();

    std::cout << "Shortest path route: \n";
    for(size_t i = 0; i < resultMinPathDFS.second.size(); ++i)
    {
        std::cout << resultMinPathDFS.second.at(i) + 1;
        if(i != resultMinPathDFS.second.size() - 1)
            std::cout << "->";
        else
            std::cout << std::endl;
    }

    std::cout << "Shortest path route length: ";
    std::cout << resultMinPathDFS.first.second << std::endl;

    std::cout << "Sum of the distances of the MST: ";
    std::cout << resultMinPathDFS.first.first << std::endl;

    return 0;
}