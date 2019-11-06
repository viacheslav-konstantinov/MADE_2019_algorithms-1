/*
4. Порядковая статистика

Ограничение времени	1 секунда
Ограничение памяти	16Mb

Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt

Даны неотрицательные целые числа n, k и массив целых чисел из диапазона [0..109] размера n.
Требуется найти k-ю порядковую статистику. т.е. напечатать число, которое бы стояло на позиции с индексом k ∈[0..n-1] в отсортированном массиве.

Напишите нерекурсивный алгоритм.

Требования к дополнительной памяти: O(n).
Требуемое среднее время работы: O(n).

Функцию Partition следует реализовывать методом прохода двумя итераторами в одном направлении.

Описание для случая прохода от начала массива к концу:
Выбирается опорный элемент.
Опорный элемент меняется с последним элементом массива.
Во время работы Partition в начале массива содержатся элементы, не бОльшие опорного. Затем располагаются элементы, строго бОльшие опорного. В конце массива лежат нерассмотренные элементы. Последним элементом лежит опорный.
Итератор (индекс) i указывает на начало группы элементов, строго бОльших опорного.
Итератор j больше i, итератор j указывает на первый нерассмотренный элемент.
Шаг алгоритма. Рассматривается элемент, на который указывает j. Если он больше опорного, то сдвигаем j. Если он не больше опорного, то меняем a[i] и a[j] местами, сдвигаем i и сдвигаем j.
В конце работы алгоритма меняем опорный и элемент, на который указывает итератор i.
*/

#include <iostream>
#include <stdio.h>
#include <bits/stdc++.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

size_t getPivot(int * inputArray, size_t len)
{
    int randomIndices[3] {0, 0, 0};
    
    if (len <= 1)
        return 0;
    else
    {
        int divider = RAND_MAX/(len - 1);
        for (int i = 0; i < 3; ++i)
            randomIndices[i] = rand() / divider;
    
        array<int, 3> elements{inputArray[randomIndices[0]], inputArray[randomIndices[1]], inputArray[randomIndices[2]]};
        array<int, 3> elementsSorted(elements);

        sort(elementsSorted.begin(), elementsSorted.end());
        size_t idx = distance(elements.begin(), find(elements.begin(), elements.end(), elementsSorted[1]));
    
        return (size_t)randomIndices[idx];
    }
}

size_t doPartition(int * inputArray, size_t len)
{
    size_t pivotIdx = getPivot(inputArray, len);
    int pivot(inputArray[pivotIdx]);

    swap(inputArray[pivotIdx], inputArray[len-1]);
    size_t i(0), j(0);
    
    while(j < len-1)
    {
        if (inputArray[j] > pivot)
            ++j;    
        else
        {
            swap(inputArray[i], inputArray[j]);
            ++i;
            ++j;
        }
    }
    swap(inputArray[i], inputArray[len-1]);
    return i;
}

int calculateKStatistics(int * inputArray, size_t len, size_t k)
{
    size_t p;
    while(true)
    {
        p = doPartition(inputArray, len);
        if (p == k)
            return (int) inputArray[p];
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
    cin >> arraySize >> k;

    int * myArray = new int[arraySize]();
    
    for(int i = 0; i < arraySize; ++i)
        cin >> myArray[i];

    cout << calculateKStatistics(myArray, arraySize, k);
    
    delete [] myArray;
    return 0;
}