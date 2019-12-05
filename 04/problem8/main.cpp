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
#include <array>
#include <memory>
#include <type_traits>
#include <iomanip>

#define HASH_TABLE_DEFAULT_BUFFER_SIZE 8
#define REHASH_RATIO 0.75
#define HASH_P 31


using byte = unsigned char;

//сериализация объекта произвольного типа T
template <typename T> 
std::array< byte, sizeof(T)>  convertToBytes( const T& object )
{
    std::array<byte, sizeof(T)> bytes ;

    const byte* begin = reinterpret_cast< const byte* >( std::addressof(object) ) ;
    const byte* end = begin + sizeof(T) ;
    std::copy(begin, end, std::begin(bytes)) ;

    return bytes;
}

template <typename T> 
size_t hashFunc1(const T &key, size_t size) 
{
    size_t hash = 0;
    std::array<byte, sizeof(T)>keySerialized = convertToBytes(key);

    for (auto item = keySerialized.begin(); item != keySerialized.end(); ++item)
        hash = (HASH_P * hash) + (size_t)*item;
    return (hash & 0x7fffffff) % size;
}

template <typename T>
size_t hashFunc2(const T &key) 
{
    std::array<byte, sizeof(T)>keySerialized = convertToBytes(key);

    return (size_t)keySerialized.at(0)*2 + 1;
}

template <typename T>
class HashTable 
{
    public:
        HashTable()
             : itemCounter_ (0), table_(HASH_TABLE_DEFAULT_BUFFER_SIZE, HashTableNode()) {}

        bool Has(const T& key) const;
        bool Add(const T& key);
        bool Remove(const T& key);
        void Print();

    private:
        struct HashTableNode 
        {
            T key;
            bool isDeleted;
            bool isEmpty;
            explicit HashTableNode(const T& key_) 
            : key(key_), isDeleted(false), isEmpty(false) {}
            HashTableNode() 
            : isDeleted(false), isEmpty(true) {}

            bool operator==(const HashTableNode& other) const
            {
                return (key == other.key && isDeleted == other.isDeleted && isEmpty == other.isEmpty);
            }
        };

        

        std::vector<HashTableNode> table_;
        size_t tableSize_ = HASH_TABLE_DEFAULT_BUFFER_SIZE;
        size_t itemCounter_ ;

        void Rehash_();
};

template <typename T>
void HashTable<T>::Print()
{
    for (size_t i = 0; i < tableSize_ ; ++i)
    {
        if (!table_[i].isEmpty)
            std::cout << "idx = " << i << ", key = " << table_[i].key << std::endl;
    }
        
}

template <typename T>
void HashTable<T>::Rehash_()
{
    size_t newSize = tableSize_ * 2;
    std::vector<HashTableNode> newTable(newSize, HashTableNode());

    for(size_t i = 0; i < tableSize_; ++i) 
    {
        if(!table_[i].isEmpty && !table_[i].isDeleted) 
        {
            size_t  h1 = hashFunc1(table_[i].key, newSize),
                    h2 = hashFunc2(table_[i].key);

                size_t j = 1;
                size_t currentHash = h1;
                while(!newTable[currentHash].isEmpty && j < newSize) 
                {
                    currentHash = (h1 + j*h2) % newSize;
                    j++;
                }
                newTable[currentHash] = HashTableNode(table_[i].key);
        }
        
        table_[i].isEmpty = false;
    }
    table_ = newTable;
    tableSize_ = newSize;
}

template <typename T>
bool HashTable<T>::Add(const T& key) 
{
    if( ((double) itemCounter_  / (double) tableSize_) >= REHASH_RATIO)
        HashTable<T>::Rehash_();

    int h1 = hashFunc1(key, tableSize_), 
        h2 = hashFunc2(key);

    int i = 1, idxFirstDeleted = -1, currentHash = h1;

    while(!table_[currentHash].isEmpty && i < tableSize_) 
    {
        if(table_[currentHash].key == key && !table_[currentHash].isDeleted)
            return false;

        if(table_[currentHash].isDeleted && idxFirstDeleted < 0)
            idxFirstDeleted = currentHash;

        currentHash = (h1 + i*h2) % tableSize_;
        i++;
    }

    if(idxFirstDeleted < 0)
        table_[currentHash] = HashTableNode(key);
    else
    {
        table_[idxFirstDeleted].key = key;
        table_[idxFirstDeleted].isDeleted = false;
    }

    itemCounter_++;
    
    return true;
}

template <typename T>
bool HashTable<T>::Has(const T& key) const 
{   
    int h1 = hashFunc1(key, tableSize_), 
        h2 = hashFunc2(key);
    
    int i = 1, currentHash = h1;
    
    while(!table_[currentHash].isEmpty && i < tableSize_) 
    {
        if(table_[currentHash].key == key && !table_[currentHash].isDeleted) 
            return true;

        currentHash = (h1 + i*h2) % tableSize_;
        i++;
    }

    return false;
}

template <typename T>
bool HashTable<T>::Remove(const T& key) 
{
    int h1 = hashFunc1(key, tableSize_), 
        h2 = hashFunc2(key);
    
    int i = 1, currentHash = h1;
    
    while(!table_[currentHash].isEmpty && i < tableSize_) 
    {
        if(table_[currentHash].key == key && !table_[currentHash].isDeleted)
        {
            itemCounter_--;
            table_[currentHash].isDeleted = true;
                
            return true;
        }

        currentHash = (h1 + i*h2) % tableSize_;
        i++;
    }
    
    return false;
}

int main() 
{
    HashTable<std::string> myTable;

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
