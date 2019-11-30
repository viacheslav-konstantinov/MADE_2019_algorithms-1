/*
4_1 Порядковая статистика

Ограничение времени	1 секунда
Ограничение памяти	16Mb

Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt

Даны неотрицательные целые числа n, k и массив целых чисел из диапазона [0..109] размера n.
Требуется найти k-ю порядковую статистику. т.е. напечатать число, которое бы стояло на позиции с 
индексом k ∈[0..n-1] в отсортированном массиве.

Напишите нерекурсивный алгоритм.

Требования к дополнительной памяти: O(n).
Требуемое среднее время работы: O(n).

Функцию Partition следует реализовывать методом прохода двумя итераторами в одном направлении.

Описание для случая прохода от начала массива к концу:
Выбирается опорный элемент.
Опорный элемент меняется с последним элементом массива.
Во время работы Partition в начале массива содержатся элементы, не бОльшие опорного. 
Затем располагаются элементы, строго бОльшие опорного. В конце массива лежат нерассмотренные элементы. 
Последним элементом лежит опорный.
Итератор (индекс) i указывает на начало группы элементов, строго бОльших опорного.
Итератор j больше i, итератор j указывает на первый нерассмотренный элемент.
Шаг алгоритма. Рассматривается элемент, на который указывает j. Если он больше опорного, то сдвигаем j.
Если он не больше опорного, то меняем a[i] и a[j] местами, сдвигаем i и сдвигаем j.
В конце работы алгоритма меняем опорный и элемент, на который указывает итератор i.

Ссылка на Яндекс.Контест
https://contest.yandex.ru/contest/14768/run-report/23723542/
*/

#include <iostream>
#include <stdio.h>
#include <bits/stdc++.h>
#include <cstdlib>
#include <algorithm>

template <typename T>
bool TLess(const T& t1, const T& t2)
{
    return (t1 < t2);
}


template <typename T>
T med(T a, T b, T c)
{
    if (TLess(b, a)) 
    {
        if (TLess(a, c))
            return a;
        return (TLess(c, b)) ? b : c;
    }   

    if (TLess(b, c))  
        return b;
    return (TLess(c, a)) ? a : c;
}


template <typename T>
size_t getPivot(T* inputArray, size_t len)
{
    if (len > 1)
    {
        T a(inputArray[0]);
        T b(inputArray[(len - 1) / 2]);
        T c(inputArray[ len - 1]);

        T median( med(a, b, c));
    
        if( median == a)
            return (size_t) 0;
        else if(median == b)
            return (size_t) (len - 1) / 2;
        else
            return (size_t) len - 1;
    }
    else
        return (size_t) 0;
}

template <typename T>
size_t doPartition(T * inputArray, size_t len)
{
    if (len <= 1)
        return 0;

    size_t pivotIdx = getPivot(inputArray, len);
    T pivot(inputArray[pivotIdx]);

    std::swap(inputArray[pivotIdx], inputArray[len-1]);
    size_t i(0), j(0);
    
    while(j < len-1)
    {
        if (TLess(pivot, inputArray[j]))
            ++j;    
        else
        {
            std::swap(inputArray[i], inputArray[j]);
            ++i;
            ++j;
        }
    }
    std::swap(inputArray[i], inputArray[len-1]);
    return i;
}

template <typename T>
T calculateKStatistics(T * inputArray, size_t len, size_t k)
{
    size_t p;
    while(true)
    {
        p = doPartition(inputArray, len);
        if (p == k)
            return inputArray[p];
        if (p > k)
            len = p ;
        else
        {
            inputArray = inputArray + p + 1 ;
            len = len - p - 1;
            k = k - p - 1;
        }            
    }
}

int main()
{
    size_t arraySize(0), k(0);
    std::cin >> arraySize >> k;

    int * myArray = new int[arraySize]();
    
    for(int i = 0; i < arraySize; ++i)
        std::cin >> myArray[i];

    std::cout << calculateKStatistics(myArray, arraySize, k);
    
    delete [] myArray;
    return 0;
}
