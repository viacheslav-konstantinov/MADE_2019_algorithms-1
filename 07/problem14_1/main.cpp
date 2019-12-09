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


template <class TVertex, class TWeight>
class Graph
{
    typedef std::pair<TVertex, TWeight> TEdge;
    public:
        Graph(const TVertex numberOfVertexes);
        void addEdge(const TVertex, const TVertex, const TWeight);
        void print();
        std::vector <TEdge> getEdgesOfVertex(const TVertex idx) const
        {
            return vertexes_[static_cast<size_t>(idx)];
        }
        TVertex getNumberOfVertexes() const
        {
            return numberOfVertexes_;
        }
    
    private:
        TVertex numberOfVertexes_;
        std::vector< std::vector <TEdge>> vertexes_;
};

template <class TVertex, class TWeight>
Graph<TVertex, TWeight>::Graph(const TVertex numberOfVertexes)
{
    numberOfVertexes_ = numberOfVertexes;
    vertexes_.resize(static_cast<size_t>(numberOfVertexes));
}

template <class TVertex, class TWeight>
void Graph<TVertex, TWeight>::addEdge(const TVertex vStart, const TVertex vEnd, const TWeight weight)
{
    vertexes_[static_cast<size_t>(vStart) - 1].push_back(std::pair<TVertex, TWeight>(vEnd - TVertex(1),  weight));
    vertexes_[static_cast<size_t>(vEnd) - 1].push_back(std::pair<TVertex, TWeight>(vStart - TVertex(1),  weight));
}

template <class TVertex, class TWeight>
void Graph<TVertex, TWeight>::print()
{
    for(size_t i = 0; i < static_cast<size_t>(numberOfVertexes_); ++i)
    {
        std::cout << "i = " << i + 1 << ":";
        for(auto edge : vertexes_[i])
            std::cout << " to = " << edge.first + TVertex(1) << ", weight = " << edge.second;
        std::cout << " " << std::endl;
    }
}

template <class TVertex, class TWeight>
TWeight primMST(Graph<TVertex, TWeight>& graphWeighted)
{
    // вес остового дерева
    TWeight result(0);
    TVertex numberOfVertexes = graphWeighted.getNumberOfVertexes();

    // минимальное расстояние от вершины до остовного дерева
    std::vector<TWeight>  minDist(static_cast<size_t>(numberOfVertexes), std::numeric_limits<TWeight>::max());
    // предок вершины в остовном дереве
    std::vector<TVertex>  parent(static_cast<size_t>(numberOfVertexes), std::numeric_limits<TVertex>::min());
    // проверка, лежит ли вершина в очереди
    std::vector<bool> isInQueue(static_cast<size_t>(numberOfVertexes), true);

    // начинаем с 0-ой вершины
    minDist[0] = TWeight(0);

    // приоритетная очередь, упорядоченная по расстоянию вершины до 
    // остовного дерева
    // в очереди хранятся пары (расстояние-номер вершины)
    //typedef std::pair<TVertex, TWeight> TEdge;
    std::set<std::pair<TWeight, TVertex>> pq;

    // заполняем очередь
    for(TVertex i = 0; i < numberOfVertexes; ++i)
        pq.insert(std::make_pair(minDist[i], i));
    
    // выбираем 0-ую вершину
    TVertex from = pq.begin()->second;
    isInQueue[static_cast<size_t>(from)] = false;
    pq.erase(pq.begin());

    while (!pq.empty())
    {
        // просматриваем вершины, смежные очередной вершине остового дерева 
        for(auto edge : graphWeighted.getEdgesOfVertex(from))
        {
            TVertex to = edge.first;
            TWeight distFromToI = edge.second;
            
            // если нашли такого сына to, что его расстояние до дерева
            // distFromToI меньше расстояния до дерева в minDist[to],
            // обновляем minDist[to], записываем предка parent[to] и
            // обновляем расстояние minDist[to] в очереди
            if (isInQueue[static_cast<size_t>(to)] && distFromToI < minDist[static_cast<size_t>(to)])
            {
                pq.erase(std::make_pair(minDist[to], to));
                minDist[static_cast<size_t>(to)] = distFromToI;
                parent[static_cast<size_t>(to)] = from;
                pq.insert(std::make_pair(minDist[to], to));
                
            }
        }
        // выбираем очередную вершину о остовом дереве
        from = pq.begin()->second;
        isInQueue[static_cast<size_t>(from)] = false;
        pq.erase(pq.begin());
        // обновляем вес остового дерева
        result += minDist[static_cast<size_t>(from)];
    }
    return result;
}

int main()
{
    int n, m;

    std::cin >> n >> m;

    int v1, v2, w;
    Graph<int, int> graphWeighted(n);
    
    for(int i = 0; i < m; ++i)
    {
        std::cin >> v1 >> v2 >> w;
        graphWeighted.addEdge(v1, v2, w);
    }

    //graphWeighted.print();
    std::cout << primMST(graphWeighted) << std::endl;
    
    return 0;
}
