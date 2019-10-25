/*
2_1. Жадина

Ограничение времени	1 секунда
Ограничение памяти	64Mb
Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt

Вовочка ест фрукты из бабушкиной корзины.
В корзине лежат фрукты разной массы. Вовочка может поднять не более K грамм. 
Каждый фрукт весит не более K грамм. За раз он выбирает несколько самых тяжелых фруктов, 
которые может поднять одновременно, откусывает от каждого половину и кладет огрызки обратно 
в корзину. Если фрукт весит нечетное число грамм, он откусывает большую половину. Фрукт массы 
1гр он съедает полностью.
Определить за сколько подходов Вовочка съест все фрукты в корзине.

Формат ввода
Вначале вводится n - количество фруктов и n строк с массами фруктов. n ≤ 50000.
Затем K - "грузоподъемность". K ≤ 1000.

Формат вывода
Неотрицательное число - количество подходов к корзине.
*/

#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

class FruitHeap
{
    public:
    FruitHeap();
    ~FruitHeap();

    size_t size();
    void push(int);
    int pop();
    int getMaxMass();
    bool isEmpty();

    private:
    vector<int> heap;
    int getParent(int);
    int getLeftChild(int);
    int getRightChild(int);
    void heapifyDown(int);
    void heapifyUp(int);
};

FruitHeap::FruitHeap()
{
	heap.resize(0);
}

FruitHeap::~FruitHeap()
{
	heap.clear();
}

size_t FruitHeap::size()
{
    return heap.size();
}

bool FruitHeap::isEmpty()
{
    return heap.empty();
}

void FruitHeap::push(int key)
{
    heap.push_back(key);
    heapifyUp(size() - 1);
}

int FruitHeap::pop()
{
    int result;

    if (size() == 0)
        throw out_of_range("Heap is empty, nothing to pop");
    
    result = getMaxMass();

    if (size() > 1)
    {
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
        return result;
    }
    if (size() == 1)
    {
        heap.pop_back();
        return result;
    }
}

int FruitHeap::getMaxMass()
{
    if (isEmpty())
        throw out_of_range("Heap is empty");
    
    return heap.at(0);
}

int FruitHeap::getParent(int idx)
{
    if (idx == 0)
        return 0;
    return (idx - 1) / 2;
    
}

int FruitHeap::getLeftChild(int idx)
{
    return (2*idx + 1);
}

int FruitHeap::getRightChild(int idx)
{
    return (2*idx + 2);
}

void FruitHeap::heapifyDown(int idx)
{
	int left = getLeftChild(idx);
	int right = getRightChild(idx);

	int largest = idx;

	if (left < size() and heap[left] > heap[idx])
		largest = left;

	if (right < size() and heap[right] > heap[largest])
		largest = right;

	if (largest != idx) {
		swap(heap[idx], heap[largest]);
		heapifyDown(largest);
	}
}

void FruitHeap::heapifyUp(int idx)
{
	if (idx > 0 and heap[getParent(idx)] < heap[idx]) 
	{
		swap(heap[idx], heap[getParent(idx)]);
		heapifyUp(getParent(idx));
	}
}

int bitenFruitMass(int mass)
{
    if (mass == 1)
        return 0;
    int offset = mass % 2;
    return (mass - offset) / 2;
}


int main()
{
    size_t sizeHeap;
    cin >> sizeHeap;
    
    // основная корзина с фруктами
    FruitHeap basket;

    int buffer;
    for(size_t i = 0; i < sizeHeap; ++i)
    {
        cin >> buffer;
        basket.push(buffer);
    }
    
    int maxMass;
    cin >> maxMass;

    // промежуточная корзина, в которой хранятся "огрызки" до того
    // как вернуться в основную корзину
    vector<int> basketToReturn;
    int massTaken = 0;
    int step = 0;

    while (!basket.isEmpty())
    {
        while(!basket.isEmpty() and (basket.getMaxMass() + massTaken <= maxMass))
        {
            //если масса "огрызка" ненулевая, добавим "огрызок" в промежуточную
            // корзину
            if (bitenFruitMass(basket.getMaxMass()) > 0)
                basketToReturn.push_back(bitenFruitMass(basket.getMaxMass()));
            massTaken += basket.pop();
        }

        // возвращаем "огрызки" в основную корзину
        for(size_t i = 0; i < basketToReturn.size(); ++i)
            basket.push(basketToReturn.at(i));

        // обнуляем промежуточную корзину и массу взятых данном подходе
        // фруктов
        basketToReturn.clear();
        step += 1;
        massTaken = 0;
    }

    cout << step;

    return 0;    
}
