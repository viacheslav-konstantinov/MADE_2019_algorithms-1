/*
12. Мосты
Ограничение времени	2 секунды
Ограничение памяти	256Mb
Ввод	bridges.in
Вывод	bridges.out
Ребро неориентированного графа называется мостом, 
если удаление этого ребра из графа увеличивает число компонент связности.

Дан неориентированный граф, требуется найти в нем все мосты.

Формат ввода
Первая строка входного файла содержит два целых числа n и m — количество 
вершин и ребер графа соответственно (1 ≤ n ≤ 20000, 1 ≤ m ≤ 200000).

Следующие m строк содержат описание ребер по одному на строке. Ребро номер i 
описывается двумя натуральными числами bi, ei — номерами концов ребра (1 ≤ bi, ei ≤ n).

Формат вывода
Первая строка выходного файла должна содержать одно натуральное число b — 
количество мостов в заданном графе. На следующей строке выведите b целых чисел — 
номера ребер, которые являются мостами, в возрастающем порядке. Ребра нумеруются 
с единицы в том порядке, в котором они заданы во входном файле.

Отчёт из Яндекс.Контеста:
https://contest.yandex.ru/contest/15957/run-report/27985681/

*/

#include <vector>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h> 
#include <utility>

// функция спуска в глубину
void DFS(std::vector<int>& bridges, int& timer, std::vector<std::vector< std::pair<int, int> >>& graph, 
                    std::vector<bool>& isVisited, std::vector<int>& tin, std::vector<int>& fup, int vv, int pp = -1)
{
    
    // отмечаем вершину посещённой
    isVisited[vv] = true;
    // записываем время входа
    tin[vv] = fup[vv] = timer++;

    //просматриваем смежные вершины
    for (int i = 0; i < graph[vv].size(); ++i)
    {
        int to = graph[vv][i].first;
        if (to == pp)
            continue;
        if (isVisited[to])
            // если смежная вершина посещена, пересчитаем fup в родителе
            fup[vv] = std::min(fup[vv], tin[to]);
        else
        {
            // если смежная вершина еще не посещена, то запустим рекурсивно спуск из потомка в родителя
            DFS(bridges, timer, graph, isVisited, tin, fup, to, vv);
            fup[vv] = std::min(fup[vv], fup[to]);

            // проверяем, не мост ли ребро (to, vv)
            if(fup[to] > tin[vv])
            {
                // проверяем, нет ли кратных ребер (to, vv)
                bool isMultipleEdge = false;
                for (int j = 0; j < graph[vv].size() && !isMultipleEdge; ++j)
                    isMultipleEdge = (j != i && graph[vv][i].first == graph[vv][j].first);

                // если кратных рёбер нет, записываем номер ребра
                if(!isMultipleEdge)
                    bridges.push_back(graph[vv][i].second);
            }
                
        }
    }
}

std::vector<int> findBridge(std::vector<std::vector< std::pair<int, int> >>& graph)
{
    // массив мостов - номеров рёбер
    std::vector<int>bridges;
    //количество вершин
    const int vertexCount(graph.size());
    //таймер
    int timer(0);

    // массивы флагов посещённости вершин и времен
    std::vector<bool> isVisited(vertexCount, false);
    std::vector<int> tin(vertexCount, INT_MAX);
    std::vector<int> fup(vertexCount, INT_MAX);

    // запускаем поиск мостов
    for(int i = 0; i < vertexCount; ++i)
        if (!isVisited[i])
            DFS(bridges, timer, graph, isVisited, tin, fup, i);

    return bridges;
}

int main()
{
    // входной и выходной файлы
    std::ifstream file("./bridges.in");
    std::ofstream fileOut("./bridges.out");
    
    int n, m;

    file >> n >> m;

    // граф
    std::vector<std::vector< std::pair<int, int> >> graph(n);

    int v1, v2;
    for(int i = 0; i < m; ++i)
    {
        file >> v1 >> v2;
        // уменьшаем на 1, поскольку входная нумерация рёбер и вершин - с единицы, а в рабочем массиве - с 0
        v1 --;
        v2 --;
        
        graph[v1].push_back(std::make_pair(v2, i));
        graph[v2].push_back(std::make_pair(v1, i));
    }

    std::vector<int> result = findBridge(graph);
    
    //сортируем по мосты по номеру
    std::sort(result.begin(), result.end());

    //записываем количество мостов
    fileOut << result.size() <<std::endl;

    // записываем номера рбер-мостов
    // номер из result увеличиваем на 1, поскольку входная нумерация рёбер и вершин - с единицы, а в рабочем массиве - с 0
    for(int i = 0; i< result.size(); ++i)
        fileOut << result[i] + 1 <<std::endl;
    
    // закрываем файлы
    file.close();
    fileOut.close();
    return 0;
}
