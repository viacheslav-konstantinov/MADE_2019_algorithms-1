
// Алгоритм Прима поиска веса минимального остовного дерева

#include <set>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
#include <limits>
#include <stdexcept>
#include <cmath>
#include <stack>

typedef std::pair<size_t, double> TEdge;
typedef std::pair<std::pair<double, double> /*coordinates*/, size_t /*idx*/> TVertex;

class Graph
{
    public:
        explicit Graph(const size_t numberOfVertexes);
        bool addEdge(const TVertex vStart, const TVertex vEnd);
        void makeCompleteGraph(const std::vector<TVertex>& listOfVertexes);
        void reset();
        void print() const;
        std::vector <TEdge> getEdgesOfVertex(const size_t idx) const
        {
            try
            {
                return vertexes_.at(idx).second;
            }
            catch (const std::out_of_range& e)
            {
                std::cerr << "Requested index is out of range! Index must be in the range = 1.." << numberOfVertexes_ << std::endl;
            }
        }
        size_t getNumberOfVertexes() const
        {
            return numberOfVertexes_;
        }
        std::pair<double, std::vector<size_t>> primMST() const;
        std::pair<std::pair<double, double>, std::vector<size_t>> getMinPathDFS() const;
    
    private:
        size_t numberOfVertexes_;
        std::vector< std::pair<TVertex /*coordinates*/, std::vector <TEdge> /*edges*/>> vertexes_;
};

void Graph::reset()
{
    numberOfVertexes_ = 0;
    vertexes_.clear(); 
}

Graph::Graph(const size_t numberOfVertexes)
{
    numberOfVertexes_ = numberOfVertexes;
    vertexes_.resize(numberOfVertexes);
}

//добавление ребра, кратных рёбер нет
bool Graph::addEdge(const TVertex vStart, const TVertex vEnd)
{
    if(vStart.second >=0 && vEnd.second >= 0)
    {
        vertexes_[vStart.second ].first = vStart;
        vertexes_[vEnd.second].first = vEnd;
        double edgeWeight = std::sqrt(std::pow( (vStart.first.first - vEnd.first.first), 2) + std::pow( (vStart.first.second - vEnd.first.second), 2));
        // edgeWeight = ((x1-x2)^2 +(y1 - y2)^2 )^(1/2)
        vertexes_[vStart.second].second.push_back(std::pair<size_t, double>(vEnd.second,    edgeWeight));
        vertexes_[vEnd.second  ].second.push_back(std::pair<size_t, double>(vStart.second,  edgeWeight));

        return true;
    }
    else
        return false;
}

void Graph::print() const
{
    for(size_t i = 0; i < numberOfVertexes_; ++i)
    {
        std::cout << "i = " << i + 1 << ", ";
        std::cout << "coordinates: x =  " << vertexes_.at(i).first.first.first << ", y =  " << vertexes_.at(i).first.first.second << std::endl;
        std::cout << "edges: \n";
        for(auto edge : vertexes_.at(i).second)
            std::cout << " to = " << (edge.first + 1) << ", weight = " << edge.second <<std::endl;
        std::cout << " " << std::endl;
    }
}


std::pair<double, std::vector<size_t>> Graph::primMST() const
{
    // вес остового дерева
    double result(0);
    size_t numberOfVertexes = this->getNumberOfVertexes();

    // минимальное расстояние от вершины до остовного дерева
    std::vector<double>  minDist(numberOfVertexes, std::numeric_limits<double>::max());
    // предок вершины в остовном дереве
    std::vector<size_t>  parent(numberOfVertexes, std::numeric_limits<size_t>::min());
    // проверка, лежит ли вершина в очереди
    std::vector<bool> isInQueue(numberOfVertexes, true);

    // начинаем с vStart
    minDist[0] = double(0);

    // приоритетная очередь, упорядоченная по расстоянию вершины до 
    // остовного дерева
    // в очереди хранятся пары (расстояние-номер вершины)
    std::set<std::pair<double, size_t>> pq;

    // заполняем очередь
    for(size_t i = 0; i < numberOfVertexes; ++i)
        pq.insert(std::make_pair(minDist[i], i));
    
    // выбираем 0-ую вершину
    size_t from = pq.begin()->second;
    isInQueue[from] = false;
    pq.erase(pq.begin());

    while (!pq.empty())
    {
        // просматриваем вершины, смежные очередной вершине остового дерева 
        for(auto edge : this->getEdgesOfVertex(from))
        {
            size_t to = edge.first;
            double distFromToI = edge.second;
            
            // если нашли такого сына to, что его расстояние до дерева
            // distFromToI меньше расстояния до дерева в minDist[to],
            // обновляем minDist[to], записываем предка parent[to] и
            // обновляем расстояние minDist[to] в очереди
            if (isInQueue[to] && distFromToI < minDist[to])
            {
                pq.erase(std::make_pair(minDist[to], to));
                minDist[to] = distFromToI;
                parent[to] = from;
                pq.insert(std::make_pair(minDist[to], to));
            }
        }
        // выбираем очередную вершину о остовом дереве
        from = pq.begin()->second;
        isInQueue[from] = false;
        pq.erase(pq.begin());
        // обновляем вес остового дерева
        result += minDist[from];
    }
    return std::make_pair(result, parent);
}

void Graph::makeCompleteGraph(const std::vector<TVertex>& listOfVertexes)
{
    for(size_t i = 0; i < numberOfVertexes_; ++i)
    {
        for(size_t j = i + 1; j < numberOfVertexes_; ++j)
            addEdge(listOfVertexes.at(i), listOfVertexes.at(j));
    }

}

//решение задачи коммивояжёра
std::pair<std::pair<double /*primMST*/, double /*lengthOfThePath*/>, std::vector<size_t> /*path*/> Graph::getMinPathDFS() const
{
    //длина минимального остового дерева и его рёбра
    std::pair<double, std::vector<size_t>> primMSTresult = this->primMST();

    //граф, составленный из рёбер минимального остового дерева
    std::vector<std::vector<size_t>> MSTgraph(numberOfVertexes_);
    
    double primMST = primMSTresult.first;

    for(size_t to = 1; to < numberOfVertexes_; ++to)
    {   
        size_t from = primMSTresult.second.at(to);
        MSTgraph.at(from).push_back(to);
        MSTgraph.at(to).push_back(from);
    }

    //обход в глубину графа, составленного из рёбер минимального остовного дерева
    //и расчёт длины этого обхода
    
    //путь
    std::vector<size_t> path;
    std::vector<bool> visited(numberOfVertexes_, false);
    std::stack<size_t> vertexStack;
    
    //длина пути
    double lengthOfThePath = 0;

    //номер предыдущей открытой вершины
    size_t previousVertex = 0;

    vertexStack.push(0);
    visited[0] = true;

    while (!vertexStack.empty())
    {
        size_t from = vertexStack.top();
        path.push_back(from);
        vertexStack.pop();

        // получение координат открытой вершины и обновление
        // пройденного пути
        double x_prev, y_prev, x_current, y_current;
        x_current = vertexes_[from].first.first.first;
        y_current = vertexes_[from].first.first.second;

        x_prev = vertexes_[previousVertex].first.first.first;
        y_prev = vertexes_[previousVertex].first.first.second;

        lengthOfThePath += std::sqrt(std::pow(x_current - x_prev, 2) + std::pow(y_current - y_prev, 2));
        
        for (size_t i = 0; i < MSTgraph[from].size(); ++i)
        {
            size_t to = MSTgraph[from][i];
            if (visited[to] == false)
            {
                vertexStack.push(to);
                visited[to] = true;
            }
        }
        previousVertex = from;
    }

    //закольцовываем маршрут
    path.push_back(path.at(0));

    return std::make_pair(std::make_pair(primMST, lengthOfThePath), path);
}