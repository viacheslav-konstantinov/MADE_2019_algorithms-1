/*
Задача 13 Пятнашки
Ограничение времени	6 секунд
Ограничение памяти	1Gb
Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt
Написать алгоритм для решения игры в “пятнашки”. 

Решением задачи является приведение к виду: 
[ 1 2 3 4 ] 
[ 5 6 7 8 ] 
[ 9 10 11 12] 
[ 13 14 15 0 ], где 0 задает пустую ячейку. 

Достаточно найти хотя бы какое-то решение. 
Число перемещений костяшек не обязано быть минимальным.

Формат ввода
Начальная расстановка.

Формат вывода
Если вам удалось найти решение, то в первой строке файла выведите 
число перемещений, которое требуется сделать в вашем решении. А во 
второй строке выведите соответствующую последовательность ходов: 
L означает, что в результате перемещения костяшки пустая ячейка 
сдвинулась влево, R – вправо, U – вверх, D – вниз. Если же выигрышная 
конфигурация недостижима, то выведите в выходной файл одно число −1.

Отчёт из Яндекс.Контеста
https://contest.yandex.ru/contest/16109/run-report/28683657/

*/

#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <chrono>
#include <set>
#include <bits/stdc++.h> 
#include <cmath>


// структура, описывающее текущее расположение фишек - узел графа
struct position 
{
    unsigned char chips[16]; // положение фишек
    char zero_place; // положение нуля
    int fScore; // оценка расстояния до решения
    
    position() //инициализация по умолчанию
    {
        for (int i = 0; i < 16; ++ i)
            chips[i] = i + 1;
      
        zero_place = 15;
        fScore = INT_MAX;
    }
    
    position(const position& other); // инициализация другим узлом
    position(char inputPositions[16], char zeroPos); // инициализация массивом и положением 0
    
    position& operator=(const position& other); // копирование 

    bool operator==(const position& other) const;
    bool operator!=(const position& other) const { return !operator==(other); }
    
    //fScore = значение приоритета в очереди, но чем меньше 
    //fScore, тем выше приоритет, поэтому return (this->fScore > other.fScore);, а не наоборот
    bool operator<(const position& other) const {return (this->fScore > other.fScore);} 

    //проверка, не является ли узел искомым
    bool isGoal() const;
    std::vector<position>  getNeighborhoods() const;

    //расчёт эвристики для узла = "Манхэттеннское расстояние" + "Последний ход"
    int getEstimation() const;

};

position::position(const position& other) 
{
    for (int i = 0; i < 16; ++ i)
        chips[i] = other.chips[i];
    zero_place = other.zero_place;
    fScore = other.fScore;
}

position::position(char inputPositions[16], char zeroPos)
{ 
    for(int i = 0; i < 16; ++i)
        chips[i] = inputPositions[i];
    zero_place = zeroPos;
}

//проверяем только расположение фишек
//fScore не проверяется
bool position::operator==(const position& other) const 
{
    for (size_t i = 0; i < 16; ++i ) 
    {
      if (chips[i] != other.chips[i]) return false;
    }
    return true;
}

position& position::operator=(const position& other)
{
    for (int i = 0; i < 16; ++ i)
        chips[i] = other.chips[i];
    
    zero_place = other.zero_place;
    fScore = other.fScore;     
    
    retrun *this;
}


char finalPositions[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
const position FinalPosition(finalPositions, (char) 15);
bool position::isGoal() const 
{
    return operator==(FinalPosition);
}


//расчёт эвристики для узла = "Манхэттеннское расстояние" + "Последний ход"
int position::getEstimation() const
{
    int distance = 0;

    char expectedX, 
        expectedY,
        realX,
        realY;
  
    //"Манхэттеннское расстояние"
    for(char i = 0; i < 16 ; ++i)
    {
        if(this->chips[i])
        {
            expectedX = (this->chips[i] - 1) / 4;
            expectedY = (this->chips[i] - 1) % 4;
            realX = i / 4;
            realY = i % 4;

            distance += std::abs(expectedX - realX);
            distance += std::abs(expectedY - realY);
        }
    }

    //добавление "Последнего хода"
    if (this->chips[15] != 12 && this->chips[15] != 15) 
        return distance + 2;
    else 
        return distance;
}


//получение соседних узлов
std::vector<position> position::getNeighborhoods() const 
{
    std::vector<position> result;
    if (zero_place < 12) 
    {
      position sibling(*this);
      std::swap(sibling.chips[zero_place], sibling.chips[zero_place+4]);
      sibling.zero_place = zero_place + 4;
      result.emplace_back(sibling);
    }
    if (zero_place >= 4) 
    {
      position sibling(*this);
      std::swap(sibling.chips[zero_place], sibling.chips[zero_place-4]);
      sibling.zero_place = zero_place - 4;
      result.emplace_back(sibling);
    }
    if (zero_place % 4 != 0)
    {
      position sibling(*this);
      std::swap(sibling.chips[zero_place], sibling.chips[zero_place-1]);
      sibling.zero_place = zero_place - 1;
      result.emplace_back(sibling);
    }
    if (zero_place % 4 != 3) 
    {
      position sibling(*this);
      std::swap(sibling.chips[zero_place], sibling.chips[zero_place+1]);
      sibling.zero_place = zero_place + 1;
      result.emplace_back(sibling);
    }
    return result;
}

//хэш-функция для position
//понадобится для хранения position в unordered_set
const size_t HashParameter = 31;
namespace std {
    template <>
    struct hash<position> 
    {
        size_t operator()(const position& key) const 
        {
            size_t value = 0;
            for (auto chip : key.chips)
                value = HashParameter * value + static_cast<size_t>(chip);
            
            return (value & 0x7fffffff);
        }
    };
}


bool A_star(position& start, std::unordered_map<position, position>& parents) 
{
    if(start.isGoal())
        return true;

    position current;

    // приоритетная очередь с расположениями фишек, упорядоченная по ВОЗРАСТАНИЮ fScore
    std::priority_queue<position> positionsQueue;

    // списиок открытых и рассмотренных расположений
    std::unordered_set<position>  closedNodes;

    // списиок открытых и не рассмотренных расположений
    std::unordered_set<position>  openedNodes;

    //расстояния до начальной вершины = количество передвинутых фишек
    std::map<position, int> distToStart;
    
    //задание начального положения
    distToStart[start] = 0;
    start.fScore = distToStart[start] + start.getEstimation();
    positionsQueue.push(start);
    openedNodes.insert(start);
    
    while(!positionsQueue.empty())
    {
        current = positionsQueue.top();
        positionsQueue.pop();
        
        openedNodes.erase(current);
        closedNodes.insert(current);

        for(auto& sibling : current.getNeighborhoods())
        {
            // проверяем всех соседей узла
            if(closedNodes.count(sibling))
                continue;
            
            bool isDistShorter;
            //расстояния до current = 1
            int estimatedDistToCurrent = distToStart[current] + 1;
                
            //если узел не открыт, добавим его
            if (openedNodes.count(sibling) == 0)
            {
                sibling.fScore = estimatedDistToCurrent + sibling.getEstimation();
                positionsQueue.push(sibling);
                openedNodes.insert(sibling);
                isDistShorter = true;
            }
            else
            {
                isDistShorter = (estimatedDistToCurrent < distToStart[sibling]);
            }

            if (isDistShorter)
            {
                parents[sibling] = current;
                distToStart[sibling] = estimatedDistToCurrent;
            }
                
            // если дошли до конца, завершаем выполнение функции и возвращаем true
            if(sibling.isGoal())
                return true; 
        }
    }
    
    return false;
}


//получит символ, куда была подвинута фишка
char GetMoveSymbol(const position& from, const position& to) 
{
    char zero_diff = to.zero_place - from.zero_place;
    switch (zero_diff) 
    {
        case 1:
            return 'L'; // Ноль вправо -> фишка влево
        case -1:
            return 'R';
        case 4:
            return 'U';
        case -4:
            return 'D';
        default:
            assert(false);
    }
    return 0;
}


//получить путь решения 
std::vector<char> GetPath(const position& start, const std::unordered_map<position, position>& parents) 
{
    std::vector<char> result;
    
    if(!start.isGoal())
    {
        position current = FinalPosition;
        
        while (current != start) 
        {
            const position parent = parents.at(current);
            result.push_back(GetMoveSymbol(parent, current));
            current = parent;
        }
    }
    // Развернем result, так как собирали его с конца.
    std::reverse(result.begin(), result.end());
    return result;
}

std::pair<bool, std::vector<char>> SolvePuzzle15(position& start) 
{
    // Запустим A_star, запоминая предков всех пройденных позиций.
    std::unordered_map<position, position> parents;
    if (!A_star(start, parents))
        return std::make_pair(false, std::vector<char>());

    return std::make_pair(true, GetPath(start, parents));
}



// mergeAndCount и sortAndCount - функции, которые рассчитывают инверсии в начальном расположении фишек
template <typename T>
using Comparator = bool (*)(const T&, const T&);

//склеивает массивы и считает инверсии
template <typename T>
int64_t mergeAndCount(T* inputArray1, size_t lenFirst,  T* inputArray2, 
            size_t lenSecond, T* result, int64_t currentCounter, Comparator<T> Tless)
{
    int64_t newCounter(currentCounter);
    size_t ii(0), jj(0);
    for(size_t i = 0; i < lenFirst + lenSecond; ++i)
    {
        if(ii < lenFirst and jj < lenSecond)
        {
            if(Tless(inputArray2[jj], inputArray1[ii]))
            {
                result[i] = inputArray2[jj];
                newCounter = newCounter + lenFirst - ii;
                jj += 1;
            }
            else
            {
                result[i] = inputArray1[ii];
                ii += 1;
            }
            
        }
        else if (jj < lenSecond)
        {
            result[i] = inputArray2[jj];
            jj += 1;
        }
        else
        {
            result[i] = inputArray1[ii];
            ii += 1;
        }
    }
    return newCounter;
}

//сортирует и считает инверсии с помощью sortAndCount
template <typename T>
int64_t sortAndCount(T* inputArray, size_t len, int64_t currentCounter, Comparator<T> Tless)
{
    if(len <= 1)
    {

        return currentCounter;
    }
    else
    {
        int64_t newCounter(currentCounter);
        size_t lenFirst  = len / 2;
        size_t lenSecond = len - lenFirst;
        
        T* bufferArray = new T[len];

        newCounter = sortAndCount(inputArray, lenFirst, newCounter, Tless);
        newCounter = sortAndCount(inputArray+lenFirst, lenSecond, newCounter, Tless);

        newCounter = mergeAndCount(inputArray, lenFirst, inputArray+lenFirst, 
                                            lenSecond, bufferArray, newCounter, Tless);
        std::copy(bufferArray, bufferArray + len, inputArray);

        delete [] bufferArray;
        return newCounter;
    }
}

template <typename T>
bool compareFunc(const T& T1, const T& T2)
{
    return (T1 < T2);
}

//проверят наличие решение
bool hasSolution15(char start[16])
{
    std::vector<char> arrayToSort;
    int lineIdx;
    
    for (int i = 0; i < 16; ++i)
    {
        if(start[i])
            arrayToSort.push_back(start[i]);
        else
            lineIdx = i;
    }

    //строка, в которой находится нуль, где 1 - самая верхняя строка
    lineIdx = (int) lineIdx / 4 + 1;

    int invertion(0);

    // количество перестановок в поле
    invertion = sortAndCount(&arrayToSort[0], arrayToSort.size(), 0, compareFunc);

    invertion += lineIdx;

    //провека на чётность суммы перестановок и номера строки lineIdx
    return !(bool)( invertion % 2);
}



int main() {
    
    //auto startTime = std::chrono::high_resolution_clock::now();
    
    char initialPositions[16];
    char zero_pos;

    int buffer;

    for(int i = 0; i < 16; ++i)
      {
          std::cin >> buffer;
          initialPositions[i] = buffer;
          if(buffer == 0)
            zero_pos = i;
      }

    
    position start(initialPositions, zero_pos);
    
    if (hasSolution15(initialPositions))
    {
        const auto result = SolvePuzzle15(start);
      
        std::cout << result.second.size() << std::endl;
      
        if(result.second.size()>0)
        {
            for (const auto move : result.second)
                std::cout << move;
        }

        std::cout << std::endl;
    }
    else
    {
        std::cout << -1 << std::endl;
    }
    
    //auto stopTime = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime);
    //std::cout << "Execution time = " << duration.count() << " ms" << std::endl;

    return 0; 
}
