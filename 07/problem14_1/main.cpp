/*
Задача 14, вариант 1 Алгоритм Прима

Минимальное остовное дерево 2018
Ограничение времени	0.2 секунды
Ограничение памяти	10Mb
Ввод	стандартный ввод или kruskal.in
Вывод	стандартный вывод или kruskal.out
Дан неориентированный связный граф. 

Требуется найти вес минимального остовного дерева в этом графе. 
Ссылка на Яндекс.Контест:
https://contest.yandex.ru/contest/16109/run-report/28116175/
https://contest.yandex.ru/contest/16109/run-report/28727955/
*/

#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
#include <limits>
#include <stdexcept>


template </*class TData,*/ class TWeight>
class Graph
{
    typedef std::pair<size_t, TWeight> TEdge;
    //typedef std::pair<size_t, TData> TVertex;
    public:
        explicit Graph(const size_t numberOfVertexes);
        bool addEdge(const size_t vStart, const size_t vEnd, const TWeight edgeWeight);
        void print() const;
        std::vector <TEdge> getEdgesOfVertex(const size_t idx) const
        {
            try
            {
                return vertexes_.at(idx - 1);
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
    
    private:
        size_t numberOfVertexes_;
        std::vector< std::vector <TEdge>> vertexes_;
};

template </*class TData, */class TWeight>
Graph<TWeight>::Graph(const size_t numberOfVertexes)
{
    numberOfVertexes_ = numberOfVertexes;
    vertexes_.resize(numberOfVertexes);
}

//добавление ребра
//нумерация начинается с 1-цы, кратных рёбер нет
template </*class TData, */class TWeight>
bool Graph<TWeight>::addEdge(const size_t vStart, const size_t vEnd, const TWeight edgeWeight)
{
    if(vStart >=1 && vEnd >= 1)
    {
        vertexes_[vStart - 1].push_back(std::pair<size_t, TWeight>(vEnd - 1,  edgeWeight));
        vertexes_[vEnd - 1].push_back(std::pair<size_t, TWeight>(vStart - 1,  edgeWeight));
        return true;
    }
    else
        return false;
}

template </*class TData, */class TWeight>
void Graph<TWeight>::print() const
{
    for(size_t i = 0; i < numberOfVertexes_; ++i)
    {
        std::cout << "i = " << i + 1 << ":";
        for(auto edge : vertexes_[i])
            std::cout << " to = " << (edge.first + 1) << ", weight = " << edge.second;
        std::cout << " " << std::endl;
    }
}

template </*class TData, */class TWeight>
TWeight primMST(Graph<TWeight>& graphWeighted)
{
    // вес остового дерева
    TWeight result(0);
    size_t numberOfVertexes = graphWeighted.getNumberOfVertexes();

    // минимальное расстояние от вершины до остовного дерева
    std::vector<TWeight>  minDist(numberOfVertexes, std::numeric_limits<TWeight>::max());
    // предок вершины в остовном дереве
    std::vector<size_t>  parent(numberOfVertexes, std::numeric_limits<size_t>::min());
    // проверка, лежит ли вершина в очереди
    std::vector<bool> isInQueue(numberOfVertexes, true);

    // начинаем с 0-ой вершины
    minDist[0] = TWeight(0);

    // приоритетная очередь, упорядоченная по расстоянию вершины до 
    // остовного дерева
    // в очереди хранятся пары (расстояние-номер вершины)
    //typedef std::pair<TVertex, TWeight> TEdge;
    std::set<std::pair<TWeight, size_t>> pq;

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
        for(auto edge : graphWeighted.getEdgesOfVertex(from + 1))
        {
            size_t to = edge.first;
            TWeight distFromToI = edge.second;
            
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
    return result;
}

int main()
{
    size_t n;
    int m;

    std::cin >> n >> m;

    int v1, v2, w;
    Graph<int> graphWeighted(n);
    
    for(int i = 0; i < m; ++i)
    {
        std::cin >> v1 >> v2 >> w;
        graphWeighted.addEdge(v1, v2, w);
    }

    //graphWeighted.print();
    std::cout << primMST(graphWeighted) << std::endl;
    
    return 0;
}
