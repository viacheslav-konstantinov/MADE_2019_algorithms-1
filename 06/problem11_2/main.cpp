/*
11_2. Количество различных путей

Ограничение времени	0.2 секунды
Ограничение памяти	10Mb
Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt

Дан невзвешенный неориентированный граф. В графе может быть несколько кратчайших путей 
между какими-то вершинами. Найдите количество различных кратчайших путей между заданными вершинами. 
Требуемая сложность O(V+E).

Формат ввода
v: кол-во вершин (макс. 50000),
n: кол-во ребер(макс. 200000),
n пар реберных вершин,
пара вершин (u, w) для запроса.

Формат вывода
Количество кратчайших путей от v к w.

Отчёт из Яндекс.Контеста:
https://contest.yandex.ru/contest/15957/run-report/27775664/
*/


#include <queue>
#include <vector>
#include <iostream>
#include<bits/stdc++.h> 

int BFS_pathCounts(int start, int end, std::vector<std::vector<int>>& edges )
{
    std::vector<int>  shortestUniquePathCounts (edges.size(), 0);
    std::vector<bool> isVisited (edges.size(), false);
    std::vector<int>  dist(edges.size(), INT_MAX);
    
    std::queue<int> q;

    // Инициализация начальной вершины
    q.push(start);
    isVisited[start] = true;
    shortestUniquePathCounts[start] = 1;
    dist[start] = 0;

    // Главный цикл - пока есть нераскрытые вершины
    while (!q.empty())
    {
        // Берем первую из них
        int v = q.front();
        q.pop();
        // Пробегаемся по всем её соседям
        for (int i = 0; i < (int) edges[v].size(); ++i)
        {
            // Если соседняя вершина еще не посещена
            if (isVisited[edges[v][i]] == false)
            {
                // Отмечаем её посещённой
                isVisited[edges[v][i]] = true;
                // Находим расстояние до start
                dist[edges[v][i]] = dist[v] + 1;

                // Находим количество различных путей кратчайшей длины в
                // в текущую вершину edges[v][i]
                for (int j = 0; j < (int) edges[edges[v][i]].size(); ++j)
                {
                    // проходимся по всем соседним вершинам и складываем pathCounts тех,
                    // что уже раскрыты и расстояние от которых до start меньше кратчайшего
                    // расстояния между start и edges[v][i]

                    // здесь проверяем, чтобы добавлять только кратчайшие пути
                    if(dist[edges[edges[v][i]][j]] < dist[edges[v][i]]) 
                        // можно не проверять на то, посещен ли узел в isVisited или нет,
                        // т.к. у нераскрытых вершин pathCounts равен 0
                        shortestUniquePathCounts[edges[v][i]] += (int) shortestUniquePathCounts[edges[edges[v][i]][j]];
                }

                q.push(edges[v][i]);
                //Если достигли заданной конечной вершины, выходим из цикла
                if (edges[v][i] == end)
                    break;
            }
        }
    }

    return shortestUniquePathCounts[end];
}

int main()
{
    int n, m; // количество вершин и рёбер
    std::cin >> n >> m;
    
    std::vector<std::vector<int>> edges(m); // таблица смежности
    
    // заполнение таблицы смежности
    int a1, a2;
    for(int i = 0; i < m; ++i)
    {
        std::cin >> a1 >> a2;
        edges[a1].push_back(a2);
        edges[a2].push_back(a1);
    }

/*
    for (int i = 0; i < m; i++)
    {
        std::cout << i << " : ";
        for (int j = 0; j < (int) edges[i].size(); j++)
            std::cout << edges[i][j] << " ";
        std::cout << std::endl;
    }
    */
    
    // ввод начальной и конечной вершин
    int start, end;
    std::cin >> start >> end;

    std::cout << BFS_pathCounts(start, end, edges);

    return 0;
}
