/*
8. Хеш-таблица

Ограничение времени	0.2 секунды
Ограничение памяти	15Mb

Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt

Реализуйте структуру данных типа “множество строк” на основе 
динамической хеш-таблицы с открытой адресацией. Хранимые строки 
непустые и состоят из строчных латинских букв. Хеш-функция строки 
должна быть реализована с помощью вычисления значения многочлена методом 
Горнера. Начальный размер таблицы должен быть равным 8-ми. Перехеширование 
выполняйте при добавлении элементов в случае, когда коэффициент заполнения 
таблицы достигает 3/4. Структура данных должна поддерживать операции добавления 
строки в множество, удаления строки из множества и проверки принадлежности 
данной строки множеству. 

1_2. Для разрешения коллизий используйте двойное хеширование.

Формат ввода
Каждая строка входных данных задает одну операцию над множеством. 
Запись операции состоит из типа операции и следующей за ним через пробел строки, 
над которой проводится операция. Тип операции – один из трех символов: 
+ означает добавление данной строки в множество; 
- означает удаление строки из множества; 
? означает проверку принадлежности данной строки множеству. 

При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует в этом 
множестве. При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует 
в этом множестве.

Формат вывода

Программа должна вывести для каждой операции одну из двух строк OK или FAIL.
Для операции '?': OK, если элемент присутствует во множестве. FAIL иначе.
Для операции '+': FAIL, если добавляемый элемент уже присутствует во множестве и 
потому не может быть добавлен. OK иначе.
Для операции '-': OK, если элемент присутствовал во множестве и успешно удален. 
FAIL иначе.

*/



#include <iostream>
#include <string>
#include <vector>

#define HASH_TABLE_DEFAULT_BUFFER_SIZE 8
#define REHASH_RATIO 0.75
#define HASH_P 31

int hashFunc1(const std::string &key, int size) 
{
    int hash = 0;
    for (int i = 0; i < key.size(); i++)
        hash = (HASH_P * hash) + key[i];
    return (hash & 0x7fffffff) % size;
}

int hashFunc2(const std::string &key) 
{
    return (int)key[0]*2 + 1;
}

class HashTable 
{
    public:
        HashTable()
             : table_(HASH_TABLE_DEFAULT_BUFFER_SIZE, nullptr), 
                itemCounter_ (0), 
                    tableSize_(HASH_TABLE_DEFAULT_BUFFER_SIZE) {}
        ~HashTable();
        HashTable(const HashTable&) = delete;
        HashTable(HashTable&&) = delete;
        HashTable& operator=(const HashTable&) = delete;
        HashTable& operator=(HashTable&&) = delete;

        bool Has(const std::string& key) const;
        bool Add(const std::string& key);
        bool Remove(const std::string& key);
        void Print();

    private:
        struct HashTableNode 
        {
            std::string key;
            bool isDeleted;
            explicit HashTableNode(const std::string& key_) 
            : key(key_), isDeleted(false) {}
        };
        std::vector<HashTableNode*> table_;

        int itemCounter_ ;
        int tableSize_;
        void Rehash_();
};

HashTable::~HashTable() 
{
    for (HashTableNode* head: table_)
        delete head;
}

void HashTable::Print()
{
    for (int i = 0; i < tableSize_ ; ++i)
    {
        if (table_[i] && !table_[i]->isDeleted)
            std::cout << "idx = " << i << ", key = " << table_[i]->key << std::endl;
    }
}
void HashTable::Rehash_()
{
    int newSize = tableSize_ * 2;
    std::vector<HashTableNode*> newTable(newSize, nullptr);

    for(int i = 0; i < tableSize_; i++) 
    {
        if(table_[i] != nullptr && !table_[i]->isDeleted) 
        {
            int h1 = hashFunc1(table_[i]->key, newSize),
                h2 = hashFunc2(table_[i]->key);

                int j = 1;
                int currentHash = h1;
                while(newTable[currentHash] != nullptr && j < newSize) 
                {
                    currentHash = (h1 + j*h2) % newSize;
                    j++;
                }
                newTable[currentHash] = table_[i];
        }
        else
            delete table_[i];
        
        table_[i] = nullptr;
    }
    table_ = newTable;
    tableSize_ = newSize;
}

bool HashTable::Add(const std::string& key) 
{
    if( ((double) itemCounter_  / (double) tableSize_) >= REHASH_RATIO)
        Rehash_();

    int h1 = hashFunc1(key, tableSize_), 
        h2 = hashFunc2(key);

    int i = 1, idxFirstDeleted = -1, currentHash = h1;

    while(table_[currentHash] != nullptr && i < tableSize_) 
    {
        if(table_[currentHash]->key == key && !table_[currentHash]->isDeleted)
            return false;

        if(table_[currentHash]->isDeleted && idxFirstDeleted < 0)
            idxFirstDeleted = currentHash;

        currentHash = (h1 + i*h2) % tableSize_;
        i++;
    }

    if(idxFirstDeleted < 0)
        table_[currentHash] = new HashTableNode(key);
    else
    {
        table_[idxFirstDeleted]->key = key;
        table_[idxFirstDeleted]->isDeleted = false;
    }

    itemCounter_++;
    
    return true;
}

bool HashTable::Has(const std::string& key) const 
{   
    int h1 = hashFunc1(key, tableSize_), 
        h2 = hashFunc2(key);
    
    int i = 1, currentHash = h1;
    
    while(table_[currentHash] != nullptr && i < tableSize_) 
    {
        if(table_[currentHash]->key == key && !table_[currentHash]->isDeleted) 
            return true;

        currentHash = (h1 + i*h2) % tableSize_;
        i++;
    }

    return false;
}

bool HashTable::Remove(const std::string& key) 
{
    int h1 = hashFunc1(key, tableSize_), 
        h2 = hashFunc2(key);
    
    int i = 1, currentHash = h1;
    
    while(table_[currentHash] != nullptr  && i < tableSize_) 
    {
        if(table_[currentHash]->key == key && !table_[currentHash]->isDeleted)
        {
            itemCounter_--;
            table_[currentHash]->isDeleted = true;
                
            return true;
        }

        currentHash = (h1 + i*h2) % tableSize_;
        i++;
    }
    
    return false;
}

int main() 
{
    HashTable myTable;

    char command = ' ';
    std::string key("");

    bool result = false;
    while(std::cin >> command >> key) 
    {
        switch(command) 
        {
            case '+':
                result = myTable.Add(key);
                break;
            case '?':
                result = myTable.Has(key);
                break;
            case '-':
                result = myTable.Remove(key);
                break;
            case 'p':
                myTable.Print();
            default:
                continue;
        }
        std::cout << (result ? "OK" : "FAIL") << std::endl;
    }
}
