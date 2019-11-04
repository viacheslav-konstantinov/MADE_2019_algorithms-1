/*
1_1. Очередь с динамическим буфером

Ограничение времени	1 секунда
Ограничение памяти	10Mb
Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt
Реализовать очередь с динамическим зацикленным буфером.

Обрабатывать команды push back и pop front/*
1_1. Очередь с динамическим буфером
 Ограничение времени	1 секунда
Ограничение памяти	10Mb
Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt
Реализовать очередь с динамическим зацикленным буфером.
 Обрабатывать команды push back и pop front.
 Формат ввода
В первой строке количество команд n. n ≤ 1000000.
 Каждая команда задаётся как 2 целых числа: a b.
 a = 2 - pop front
a = 3 - push back
 Если дана команда pop front, то число b - ожидаемое значение. 
Если команда pop front вызвана для пустой структуры данных, то ожидается “-1”.
 Формат вывода
Требуется напечатать YES - если все ожидаемые значения совпали. 
Иначе, если хотя бы одно ожидание не оправдалось, то напечатать NO.
*/

 #define SIZE_STEP 1024
#include <iostream>
#include <bits/stdc++.h>

 using namespace std;

 class Queue {
public:
    Queue();
    ~Queue();

     void Push(int key);
    int Pop();

     bool IsEmpty();
    bool IsFull();
    void PrintQueue();

 private:
    void AllocMemory(int);
    int * buffer_;
    int idxPop_;
    int idxPush_;
    int size_;
    int currentSize_;
};

 Queue::Queue()
{
    buffer_ = new int[SIZE_STEP];
    idxPop_ = 0;
    idxPush_ = 0;
    size_ = SIZE_STEP;
    currentSize_ = 0;
}

 Queue::~Queue()
{
   delete [] buffer_;
}

 void Queue::Push(int key)
{
    // если в буфере есть свободное место, то добавляем
    // число, сдвигаем индекс idxPush_ и увеличиваем размер на 1 
    if(!IsFull())
    {
        *(buffer_ + idxPush_) = key;
        idxPush_ += 1;
        if (idxPush_ > size_ - 1)
           idxPush_ = 0; 
        currentSize_ += 1;
    }
    // Аллоцируем место, если буфер заполнен
    else
        AllocMemory(key);

 }

 void Queue::AllocMemory(int key)
{
    size_ += SIZE_STEP;
    // указатель на расширенную очередь
    int * newBuffer = new int[size_];

     // Если idxPush_ = 0 и буфер заполнен, значит "хвост"
    // очереди в самом конце и при перезаписи данных 
    // индексы уже записанных чисел не изменятся
    if (idxPush_ == 0) 
    {
        idxPush_ += SIZE_STEP;
        for(int i = 0; i < size_ - SIZE_STEP; i++)
            *(newBuffer + i) = *(buffer_ + i);

         *(newBuffer + SIZE_STEP) = key;
    }
    else
    {
        // данные с индексами до idxPush_ сохранят
        // свои индексы
        for(int i = 0; i < idxPush_; i++)
            *(newBuffer + i) = *(buffer_ + i);

         *(newBuffer + idxPush_) = key;

         // данные "голова", с индексами до idxPush_ сохранят индексы
        // idxPush_<=idxPop_ значит, что массив имеет струкутру "голова""хвост"
        // и в новой очереди элементы хвоста и idxPop_ будут иметь индексы больше
        // на SIZE_STEP 
        if (idxPush_<=idxPop_)
        {
            for(int i = idxPop_; i < size_ - SIZE_STEP; i++)
                *(newBuffer + SIZE_STEP + i) = *(buffer_ + i);
            idxPop_ += SIZE_STEP;
        }
    }
    delete [] buffer_;
    buffer_ = newBuffer;
    idxPush_ += 1;
    currentSize_ += 1;
}

 int Queue::Pop()
{
    if(!IsEmpty()) 
    {
        int result = *(buffer_ + idxPop_);
        idxPop_ += 1;
        if (idxPop_ > size_ - 1)
           idxPop_ = 0; 
        currentSize_ -= 1;
        return result;
    }
    else
        return -1;
}

 bool Queue::IsFull()
{
    return currentSize_ == size_;
}

 bool Queue::IsEmpty()
{
    return currentSize_ == 0;
}

 // метод для печати очереди
// используется для отладки
void Queue::PrintQueue()
{
    if (idxPop_ != idxPush_)
    {
        if(idxPop_ < idxPush_)
        {
            for(int i = 0; i < size_; i++)
            {
                if((i < idxPop_) or (i >= idxPush_))
                    cout << "_ ";
                else
                    cout << *(buffer_ + i) << " ";
            }
        }
        else
        {
            for(int i = 0; i < size_; i++)
            {
                if((i < idxPop_) and (i >= idxPush_))
                    cout << "_ ";
                else
                    cout << *(buffer_ + i) << " ";
            }            
        }
        cout << endl;   
    } 
    else
    {
        if(IsFull())
        {
            for (int i = 0; i < size_; i++)
                std::cout << *(buffer_ + i) << " ";

             cout << endl;
        } 
        else
        {
            for (int i = 0; i < size_; i++)
                cout << "_ ";

             cout << endl;
        }
    }
}

 int main()
{
    int numberOfStrings;
    cin >> numberOfStrings;
    Queue myQueue;

     int operation;
    int numberToOperate;

     for(int i = 0; i < numberOfStrings; i++)
    {
        cin >> operation;
        cin >> numberToOperate;

         if(operation == 2)
        {
            if(myQueue.Pop()!=numberToOperate)
            {
                cout << "NO";
                return 0;
            }
        }
        else if(operation == 3)
        {
            myQueue.Push(numberToOperate);
        }
    }

     cout << "YES";

     return 0;
}
.

Формат ввода
В первой строке количество команд n. n ≤ 1000000.

Каждая команда задаётся как 2 целых числа: a b.

a = 2 - pop front
a = 3 - push back

Если дана команда pop front, то число b - ожидаемое значение. 
Если команда pop front вызвана для пустой структуры данных, то ожидается “-1”.

Формат вывода
Требуется напечатать YES - если все ожидаемые значения совпали. 
Иначе, если хотя бы одно ожидание не оправдалось, то напечатать NO.
*/

#define SIZE_STEP 1024
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class Queue {
public:
    Queue();
    Queue(const Queue&) = delete;
    Queue(const Queue&&) = delete;
    Queue& operator= (const Queue&) = delete;
    Queue& operator= (const Queue&&) = delete;
    ~Queue();
 
    void Push(int key);
    int Pop();
 
    bool IsEmpty();
    bool IsFull();
    void PrintQueue();
 
private:
    void AllocMemory(int);
    int * buffer_;
    int idxPop_;
    int idxPush_;
    int size_;
    int currentSize_;
};

Queue::Queue()
{
    buffer_ = new int[SIZE_STEP];
    idxPop_ = 0;
    idxPush_ = 0;
    size_ = SIZE_STEP;
    currentSize_ = 0;
}
 
Queue::~Queue()
{
   delete [] buffer_;
}
 
void Queue::Push(int key)
{
    // если в буфере есть свободное место, то добавляем
    // число, сдвигаем индекс idxPush_ и увеличиваем размер на 1 
    if(!IsFull())
    {
        *(buffer_ + idxPush_) = key;
        idxPush_ += 1;
        if (idxPush_ > size_ - 1)
           idxPush_ = 0; 
        currentSize_ += 1;
    }
    // Аллоцируем место, если буфер заполнен
    else
        AllocMemory(key);
    
}

void Queue::AllocMemory(int key)
{
    int * newBuffer = new int[2*size_];

    // Если idxPush_ = 0 и буфер заполнен, значит "хвост"
    // очереди в самом конце и при перезаписи данных 
    // индексы уже записанных чисел не изменятся
    if (idxPush_ == 0) 
    {
        idxPush_ += size_;
        for(int i = 0; i < size_; ++i)
            *(newBuffer + i) = *(buffer_ + i);
        
        *(newBuffer + size_) = key;
    }
    else
    {
        // данные с индексами до idxPush_ сохранят
        // свои индексы
        for(int i = 0; i < idxPush_; i++)
            *(newBuffer + i) = *(buffer_ + i);
    
        *(newBuffer + idxPush_) = key;
        
        // данные "голова", с индексами до idxPush_ сохранят индексы
        // idxPush_<=idxPop_ значит, что массив имеет струкутру "голова""хвост"
        // и в новой очереди элементы хвоста и idxPop_ будут иметь индексы больше
        // на SIZE_STEP 
        if (idxPush_ <= idxPop_)
        {
            for(int i = idxPop_; i < size_; i++)
                *(newBuffer + size_ + i) = *(buffer_ + i);
            idxPop_ += size_;
        }
    }
    delete [] buffer_;
    buffer_ = newBuffer;
    idxPush_ += 1;
    currentSize_ += 1;
    size_ *= 2;
}

int Queue::Pop()
{
    if(!IsEmpty()) 
    {
        int result = *(buffer_ + idxPop_);
        idxPop_ += 1;
        if (idxPop_ > size_ - 1)
           idxPop_ = 0; 
        currentSize_ -= 1;
        return result;
    }
    else
        return -1;
}

bool Queue::IsFull()
{
    return currentSize_ == size_;
}

bool Queue::IsEmpty()
{
    return currentSize_ == 0;
}

// метод для печати очереди
// используется для отладки
void Queue::PrintQueue()
{
    if (idxPop_ != idxPush_)
    {
        if(idxPop_ < idxPush_)
        {
            for(int i = 0; i < size_; i++)
            {
                if((i < idxPop_) or (i >= idxPush_))
                    cout << "_ ";
                else
                    cout << *(buffer_ + i) << " ";
            }
        }
        else
        {
            for(int i = 0; i < size_; i++)
            {
                if((i < idxPop_) and (i >= idxPush_))
                    cout << "_ ";
                else
                    cout << *(buffer_ + i) << " ";
            }            
        }
        cout << endl;   
    } 
    else
    {
        if(IsFull())
        {
            for (int i = 0; i < size_; i++)
                std::cout << *(buffer_ + i) << " ";

            cout << endl;
        } 
        else
        {
            for (int i = 0; i < size_; i++)
                cout << "_ ";
            
            cout << endl;
        }
    }
}

int main()
{
    int numberOfStrings;
    cin >> numberOfStrings;
    Queue myQueue;

    int operation;
    int numberToOperate;

    for(int i = 0; i < numberOfStrings; i++)
    {
        cin >> operation;
        if(operation == 2)
        {
            cin >> numberToOperate;
            //cout << "Popping is called\n";
            if(myQueue.Pop()!=numberToOperate)
            {
                cout << "NO";
                return 0;
            }
        }
        if(operation == 3)
        {
            cin >> numberToOperate;
            //cout << "Pushing is called\n";
            myQueue.Push(numberToOperate);
        }    
        if(operation == 1)
        {
            //cout << "Printing is called\n";
            myQueue.PrintQueue();
        }
    }
    
    cout << "YES";
    
    return 0;
}
