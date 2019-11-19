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

using namespace std;

//склеивает массивы и считает инверсии
void mergeAndCount(int* inputArray1, int lenFirst, int* inputArray2,  int lenSecond, int* result, int64_t& counter)
{
    int ii(0), jj(0);
    for(int i = 0; i < lenFirst + lenSecond; ++i)
    {
        if(ii < lenFirst and jj < lenSecond)
        {
            if(inputArray1[ii] > inputArray2[jj])
            {
                result[i] = inputArray2[jj];
                counter = counter + lenFirst - ii;
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
}

//сортирует и считает инверсии с помощью sortAndCount
void sortAndCount(int32_t*inputArray, int len, int64_t& counter)
{
    if(len <= 1)
    {
        return;
    }
    else
    {
        int lenFirst  = len / 2;
        int lenSecond = len - lenFirst;
        
        int* bufferArray = new int[len];

        sortAndCount(inputArray, lenFirst, counter);
        sortAndCount(inputArray+lenFirst, lenSecond, counter);

        mergeAndCount(inputArray, lenFirst, inputArray+lenFirst, lenSecond, bufferArray, counter);
        copy(bufferArray, bufferArray + len, inputArray);

        delete [] bufferArray;
    }
}

int main()
{
    int sizeOfArray;
    
    vector<int> arrayToSortVector;
    
    int bufer;
    
    while(cin >> bufer)
        arrayToSortVector.push_back(bufer);    

    sizeOfArray = arrayToSortVector.size();
    
    int * arrayToSort;
    arrayToSort = &arrayToSortVector[0];

    int64_t counter(0);
    sortAndCount(arrayToSort, sizeOfArray, counter);

    cout << counter;
}
