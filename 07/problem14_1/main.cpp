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
*/

#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>

int primMST(std::vector< std::vector< std::pair<int, int> >>& graphWeighted)
{
    // вес остового дерева
    int result(0);
    int numberOfVertexes = (int) graphWeighted.size();

    // минимальное расстояние от вершины до остовного дерева
    std::vector<int>  minDist(graphWeighted.size(), INT_MAX);
    // предок вершины в остовном дереве
    std::vector<int>  parent(graphWeighted.size(), -1);
    // проверка, лежит ли вершина в очереди
    std::vector<bool>  isInQueue(graphWeighted.size(), true);

    // начинаем с 0-ой вершины
    minDist[0] = 0;

    // приоритетная очередь, упорядоченная по расстоянию вершины до 
    // остовного дерева
    // в очереди хранятся пары (расстояние-номер вершины)
    std::set < std::pair<int, int> > pq;

    // заполняем очередь
    for(int i = 0; i < numberOfVertexes; ++i)
        pq.insert(std::make_pair(minDist[i], i));
    
    // выбираем 0-ую вершину
    int from = pq.begin()->second;
    isInQueue[from] = false;
    pq.erase(pq.begin());


    while (!pq.empty())
    {
        // просматриваем вершины, смежные очередной вершине остового дерева 
        for(int i = 0; i < graphWeighted[from].size(); ++i)
        {
            int to = graphWeighted[from][i].first;
            int distFromToI = graphWeighted[from][i].second;
            
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
    int n, m;

    std::cin >> n >> m;

    int v1, v2, w;
    std::vector< std::vector< std::pair<int, int> >> graphWeighted(n);
    
    for(int i = 0; i < m; ++i)
    {
        std::cin >> v1 >> v2 >> w;
        graphWeighted[v1 - 1].push_back(std::make_pair(v2 - 1, w));
        graphWeighted[v2 - 1].push_back(std::make_pair(v1 - 1, w));
    }

    std::cout << primMST(graphWeighted) << std::endl;
    
    return 0;
}
