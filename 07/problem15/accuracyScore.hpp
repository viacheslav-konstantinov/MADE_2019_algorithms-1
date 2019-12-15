#include <cmath>
#include <vector>

typedef std::pair<double, double> coordinates;
std::pair<coordinates /*for X*/, coordinates /*for Y*/> accuracyScore(std::vector<coordinates>& input)
{
    double meanX(0), meanY(0);
    for(size_t i = 0; i < input.size(); ++i)
    {
        meanX += input.at(i).first;
        meanY += input.at(i).second;
    }
    meanX /= input.size();
    meanY /= input.size();

    double varX(0), varY(0);
    for(size_t i = 0; i < input.size(); ++i)
    {
        varX += (input.at(i).first - meanX)*(input.at(i).first - meanX);
        varY += (input.at(i).second - meanY)*(input.at(i).second - meanY);
    }
    varX /= input.size();
    varY /= input.size(); 

    varX = std::sqrt(varX);
    varY = std::sqrt(varY);

    return std::make_pair(std::make_pair(meanX, varX), std::make_pair(meanY, varY));
}

std::pair<double, double> accuracyScore(std::vector<double>& input)
{
    double meanX(0);
    for(size_t i = 0; i < input.size(); ++i)
    {
        meanX += input.at(i);
    }
    meanX /= input.size();

    double varX(0);
    for(size_t i = 0; i < input.size(); ++i)
        varX += (input.at(i) - meanX)*(input.at(i) - meanX);
    
    varX /= input.size(); 
    varX = std::sqrt(varX);

    return std::make_pair(meanX, varX);
}