/*
3_3. Количество инверсий

Ограничение времени	1 секунда
Ограничение памяти	64Mb

Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt

Дана последовательность целых чисел из диапазона (-1000000000 .. 1000000000). 
Длина последовательности не больше 1000000. Числа записаны по одному в строке. 
Количество чисел не указано. Пусть количество элементов n, и числа записаны в массиве a = a[i]: i из [0..n-1]. 
Требуется напечатать количество таких пар индексов (i,j) из [0..n-1], что (i < j и a[i] > a[j]). 
Указание: количество инверсий может быть больше 4*1000000000 - используйте int64_t.

Отчёт в Яндекс.Контесте
https://contest.yandex.ru/contest/14768/run-report/23683623/
*/


#include <iostream>
#include <stdio.h>
#include <bits/stdc++.h>

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

int main()
{
    size_t sizeOfArray;
    
    std::vector<int> arrayToSortVector;
    
    int bufer;
    
    while(std::cin >> bufer)
        arrayToSortVector.push_back(bufer);    

    sizeOfArray = arrayToSortVector.size();
    
    int * arrayToSort;
    arrayToSort = &arrayToSortVector[0];

    int64_t counter(0);
    counter = sortAndCount(arrayToSort, sizeOfArray, 0, compareFunc);

    std::cout << counter;
}
