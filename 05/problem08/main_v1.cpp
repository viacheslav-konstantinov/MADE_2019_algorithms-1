/*
Задача 9. Алгоритм сжатия данных Хаффмана
Реализуйте алгоритм построения оптимального префиксного кода Хаффмана. 
При помощи алгоритма реализуйте две функции для создания архива из одного файла и 
извлечения файла из архива.

// Метод архивирует данные из потока original
void Encode(IInputStream& original, IOutputStream& compressed);
// Метод восстанавливает оригинальные данные
void Decode(IInputStream& compressed, IOutputStream& original);
 где:
typedef unsigned char byte;

struct IInputStream {
	// Возвращает false, если поток закончился
	bool Read(byte& value) = 0;
};

struct IOutputStream {
	void Write(byte value) = 0;
};

В контест необходимо отправить .cpp файл содержащий функции Encode, Decode, а 
также включающий файл Huffman.h. Тестирующая программа выводит скоринг зависящий 
от соотношения размера сжатого файла и исходного.

*/


#include <iostream>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <stack>
#include <vector>
#include <bitset>
#include <functional>
#include "Huffman.h"


// класс объектов для побитной записи
class BitsWriter 
{
    public:
        void writeBit(bool bit);
        void writeByte(unsigned char byte);
        void clean();

        std::vector<unsigned char> GetResult();

    private:
        std::vector<unsigned char> buffer_;
        unsigned char accumulator_ = 0;
        int bits_count_ = 0;
};

void BitsWriter::writeBit(bool bit) 
{
    // Ставим бит в аккумулятор на нужное место
    accumulator_ |= static_cast<unsigned char>(bit) << bits_count_;
    ++bits_count_;
    if (bits_count_ == 8) 
    {
        bits_count_ = 0;
        buffer_.push_back(accumulator_);
        accumulator_ = 0;
    }
}

void BitsWriter::writeByte(unsigned char byte) 
{
    if (bits_count_ == 0) 
    {
        buffer_.push_back(byte);
    } 
    else 
    {
        accumulator_ |= byte << bits_count_;
        buffer_.push_back(accumulator_);
        accumulator_ = byte >> (8 - bits_count_);
    }
}

std::vector<unsigned char> BitsWriter::GetResult() 
{
    if (bits_count_ != 0) 
    {
    // Добавляем в буфер аккумулятор, если в нем что-то есть.
    buffer_.push_back(accumulator_);
    }

    buffer_.push_back(static_cast<unsigned char>(bits_count_));
    return std::move(buffer_);
}

void BitsWriter::clean()
{
    buffer_.clear();
    accumulator_ = 0;
    bits_count_ = 0;
}

//класс словарей "симовол" - частота и функции для сравнения подобных пар
using Comparator = std::function<bool(std::pair<std::string, int>, std::pair<std::string, int>)>;
using charFrequenciesDict = std::map<std::string, int>;

class HuffmanEncoder
{   
    public:
        explicit HuffmanEncoder(const std::string & inputString);
        HuffmanEncoder()
            : root_(nullptr), stringToCompress_(""), isBuilt(false) {}
        ~HuffmanEncoder();

        HuffmanEncoder(const HuffmanEncoder&) = delete;
        HuffmanEncoder(HuffmanEncoder&& other) = delete;
        HuffmanEncoder& operator=(const HuffmanEncoder&) = delete;
        HuffmanEncoder& operator=(HuffmanEncoder&&) = delete;

        //добавление символа
        void addChar(unsigned char ch);
        //положить строку для кодирования
        void setString(const std::string & inputString);
        // построить кодирующее дерево
        void buildTree();
        // построить кодирующее дерево
        // вершина - слева, листья - справа
        void printTree();
        // напечатать таблицу кодировки
        void printCharTable();
        // стереть все данные
        void reset();

        bool isTreeBuilt();
        //получить сжатые данные в виде строки
        std::string getStringCompressed(char spacer);
        // получить сжатые данные в виде массива символов
        std::vector<unsigned char> getStringCompressedVector();
        // получить сжатую таблицу префиксов в виде массива символов
        std::vector<unsigned char> getPrefixTable();

        //получение глубины дерева
        int getTreeDepth();
        
        // получить таблицу префиксов и строку в зашифрованном виде
        // в начале печатается количество уникальных символов в строке
        // Схема кодировки
        // (количество уникальных символов)(глубина_дерева)(код_символа_в_дереве,8_битный_код_символа)...
        // ...(код_символа_в_дереве,8_битный_код_символа)(зашифрованная_строка)(количество_использованных_разрядов_в_предыдущем_байте)  
        std::vector<unsigned char> getStringAndTableCopmressed();

    private:
        struct Node
        {
            int key;
            std::string chars;
            Node* left;
            Node* right;
            Node()
                : key(0), chars(""), left(nullptr), right(nullptr) {} ;
            Node(const int key, const std::string& chars, const Node* leftRoot, const Node* rightRoot)
                : key(key), chars(chars), 
                    left(const_cast<Node*>(leftRoot)), right(const_cast<Node*>(rightRoot)) {};
            Node(const std::pair<std::string, int>& charPair)
                : key(charPair.second), chars(charPair.first), left(nullptr), right(nullptr) {};
            
            bool operator==(const Node& other) const
            {
                return (key == other.key && left == other.left && right == other.right && chars == other.chars );
            }
        };
        
        Node* root_;

        // данные для сжатия
        std::string stringToCompress_;
        // словарь частот символов
        charFrequenciesDict charFrequencies_;
        // кодировщик
        BitsWriter bitsWriter_;
        // построено ли кодирующее дерево
        bool isBuilt;

        // doPostOrder_ используется в деструкторе
        using NodeCallBack = void (*)(Node*);
        void doPostOrder_(NodeCallBack visitNode);

        // doPreOrder_ используется для вычисления пути к листу с символом 
        // 1 - лист, 0 - не лист       
        using NodeCallBackPreOrder = void (*)(Node*, BitsWriter&);
        void doPreOrder_(NodeCallBackPreOrder visitNode);

        // печать дерева
        void printTree_(Node* node, int level);

        // метод для получения кода символа 
        // 0 - налево, 1 - направо
        std::string getCharCode_(std::string str);

        //метод получения глубины префиксного дерева
        int getMaxDepthOfTree_(Node* node);
        
};

HuffmanEncoder::HuffmanEncoder(const std::string & inputString)
{
    root_ = nullptr;
    stringToCompress_ = std::string(inputString);
}

HuffmanEncoder::~HuffmanEncoder()
{
    //обойдем дерево и удалим каждый из узлов
    doPostOrder_([](Node* node)->void{delete node; });
}


//метод обхода
void HuffmanEncoder::doPostOrder_(NodeCallBack doWithNodes)
{
    std::stack<Node*> nodeStack;
    Node* currentNode;
    currentNode = root_;
    Node* lastVisitedNode = nullptr;
    Node* topOfStack = nullptr;
 
    while ((nodeStack.size() > 0) or currentNode)
    {
        if (currentNode) 
        {
            nodeStack.push(currentNode);
            currentNode = currentNode->left;
        } 
        else
        {
            topOfStack = nodeStack.top();
            if (topOfStack->right and lastVisitedNode != topOfStack->right)
                currentNode = topOfStack->right;                
            else 
            {
                nodeStack.pop();
                lastVisitedNode = topOfStack;
                doWithNodes(topOfStack);
            }
        }
    }
}


// метод прямого обхода
void HuffmanEncoder::doPreOrder_(NodeCallBackPreOrder visitNode) 
{
    std::stack<Node*> nodeStack;
    Node* currentNode;
    currentNode = root_;

    while (nodeStack.size() != 0 || currentNode != nullptr) 
    {
        if (currentNode != nullptr) 
        {
            visitNode(currentNode, bitsWriter_);
            if (currentNode->right)
                nodeStack.push(currentNode->right);
            currentNode = currentNode->left;
        } else 
        {
            currentNode = nodeStack.top();
            nodeStack.pop();
        }
    }
}


void HuffmanEncoder::addChar(unsigned char ch)
{
    if(!isBuilt)
        stringToCompress_ += std::string(1, ch);
}

void HuffmanEncoder::setString(const std::string & inputString)
{
    if(!isBuilt)
        stringToCompress_ = std::string(inputString);
        
}

void HuffmanEncoder::reset()
{
    if(isBuilt)
    {
        doPostOrder_([](Node* node)->void{delete node; });
        stringToCompress_ = "";
        charFrequencies_.clear();
        bitsWriter_.clean();
        isBuilt = false;
    }
    else
    {
        stringToCompress_ = "";
    }
    
}

bool HuffmanEncoder::isTreeBuilt()
{
    return isBuilt;
}

void HuffmanEncoder::buildTree()
{
    if(!isBuilt)
    {
        charFrequencies_.clear();

        //расчёт частот символов
        for(char& c : stringToCompress_)
        {
            std::string c_as_string(1, c);
            if(charFrequencies_.find(c_as_string) != charFrequencies_.end())
                charFrequencies_[c_as_string] += 1;
            else
                charFrequencies_.insert(std::pair<std::string, int>(c_as_string, 1));
        }
        
        if(charFrequencies_.size()>1)
            {
                //тип функций для сравнения двух узлов
                using ComparatorNodes = std::function<bool(Node*, Node*)>;
                // класс упорядоченных по ключу - частоте чимволов в узлах словарей
                using NodesDictSorted =  std::set<Node*, ComparatorNodes>;

                //функция сравнения узлов
                ComparatorNodes nodeCompare = [](Node* node1 ,Node* node2)
                {
                    return node1->key <= node2->key;
                };

                // словарь с отсортированных по возрастанию ключа узлов
                NodesDictSorted setOfNodes(nodeCompare);

                //заполнение словаря
                //maxKey - суммарное количество символов
                int maxKey = 0;
                for(auto i = charFrequencies_.begin(); i != charFrequencies_.end(); ++i)
                {
                    maxKey += i->second;
                    setOfNodes.insert(new Node(*i));
                }

                //выбираем два узла с минимальными частотами 
                Node * node1 = new Node(**setOfNodes.begin());
                setOfNodes.erase(setOfNodes.begin());

                Node * node2 = new Node(**setOfNodes.begin());
                setOfNodes.erase(setOfNodes.begin());

                //объединяем в третий узел
                //левый ребенок имеет ключ не больше, чем правый
                Node * currentRoot = new Node();
                currentRoot->key = node1->key + node2->key;
                
                if(node1->key <= node2->key)
                {
                    currentRoot->chars = node1->chars + node2->chars;
                    currentRoot->left =  node1;
                    currentRoot->right = node2;
                }
                else
                {
                    currentRoot->chars = node2->chars + node1->chars;
                    currentRoot->left  = node2;
                    currentRoot->right = node1;            
                }

                //повторяем процедуру, пока в текущем узле ключ не станет
                //равным maxKey
                while(currentRoot-> key < maxKey && setOfNodes.size() > 0)
                {
                    //кладем новый узел в отсортированный словарь
                    setOfNodes.insert(currentRoot);
                    
                    //повторяем процедуру
                    node1 = new Node(**setOfNodes.begin());
                    setOfNodes.erase(setOfNodes.begin());

                    node2 = new Node(**setOfNodes.begin());
                    setOfNodes.erase(setOfNodes.begin());

                    currentRoot = new Node();
                    currentRoot->key = node1->key + node2->key;

                    if(node1->key <= node2->key)
                    {
                        currentRoot->chars = node1->chars + node2->chars;
                        currentRoot->left = node1;
                        currentRoot->right = node2;
                    }
                    else
                    {
                        currentRoot->chars = node2->chars + node1->chars;
                        currentRoot->left  = node2;
                        currentRoot->right = node1;            
                    }

                }
                // в корень записываем узел с ключом maxKey
                root_ = currentRoot;
            }
            // если строка = посторение 1-го символа
            // создаём дерево с одним корнем и одним левый потомком = листом
            else if (charFrequencies_.size() == 1)
            {
                root_ = new Node();

                root_->chars = charFrequencies_.begin()->first;;
                root_->chars = root_->chars + charFrequencies_.begin()->first; ;
                root_->key = charFrequencies_.begin()->second;


                root_->left = new Node();
                root_->left->key = charFrequencies_.begin()->second;
                root_->left->chars = charFrequencies_.begin()->first;
            }
    isBuilt = true;
    }
            
}

void HuffmanEncoder::printTree()
{
    if(isBuilt)
        printTree_(root_, 0);
}


void HuffmanEncoder::printTree_(Node * node, int level)
{
    if (!node)
        return;
    
    level += 5;
    printTree_(node->right, level);
    std::cout << std::endl;

        
    for(int i = 5; i < level; ++i)
        std::cout << "  ";
        
    std::cout << node->key << " " << node->chars << std::endl;
    printTree_(node->left, level);
}

void HuffmanEncoder::printCharTable()
{
    if(isBuilt)
    {
        std::string buffer;
        std::cout << " Char - frequency - code in decoding tree - 8-bit code (inverted)\n";
        for(auto i = charFrequencies_.begin(); i != charFrequencies_.end(); ++i)
        {
            std::cout << "Char: " << i->first << ", freq: " << i->second << ", ";
            buffer = getCharCode_(i->first);
            std::reverse(buffer.begin(), buffer.end());
            std::cout << "code: " << buffer << ", ";
            std::cout << "binary code: " << std::bitset<8>(i->first[0]) << std::endl;

        }
        std::cout << "In total: " << charFrequencies_.size() << " different symbols in string.\n";
    }

}

// получить код символа в дереве
// 1 - направо, 0 - налево
std::string HuffmanEncoder::getCharCode_(std:: string str)
{
    std::string result("");
    Node * currentRoot = root_;

    while(currentRoot->chars != str)
    {
        if (currentRoot->left && currentRoot->left->chars.find(str)!=std::string::npos)
        {
            currentRoot = currentRoot->left;
            result = result + "0";
        }
        else
        {
            currentRoot = currentRoot->right;
            result = result + "1";
        }
            
    }
    return result;
}

// вывести зашифрованную строку в виде строки
std::string HuffmanEncoder::getStringCompressed(char spacer='\0')
{
    if(isBuilt)
    {
        std::string result("");
        if (spacer=='\0')
        {
            for(char& c : stringToCompress_)
                 result = result + getCharCode_(std::string(1,c));
        }
        else
        {
            for(char& c : stringToCompress_)
                result = result + getCharCode_(std::string(1,c)) + std::string(1, spacer);
        }
        
        return result;
    }
    else
        return "";
}

// вывести зашифрованную строку в виде массива символов
std::vector<unsigned char> HuffmanEncoder::getStringCompressedVector()
{
    if(isBuilt)
    {
        bitsWriter_.clean();
        std::string strCompressed = getStringCompressed();
        for(char& c : strCompressed)
            bitsWriter_.writeBit((bool)( c == '1'));
        return bitsWriter_.GetResult();
    }
    else
        return std::vector<unsigned char>(0);
}

// получить таблицу префиксов в зашифрованном виде
// в начале печатается количество уникальных символов в строке
// и глубина дерева
std::vector<unsigned char> HuffmanEncoder::getPrefixTable()
{
    if(isBuilt)
    {
        bitsWriter_.clean();
        bitsWriter_.writeByte(charFrequencies_.size());
        bitsWriter_.writeByte(getMaxDepthOfTree_(root_));

        auto nodeHandler = [](Node* node, BitsWriter& outputStream)->void
        {

            if(node->left && node->right)
            {
                outputStream.writeBit(false);
            }
            else
            {
                outputStream.writeBit(true);
                outputStream.writeByte(node->chars[0]);
            }
        };
        
        doPreOrder_(nodeHandler);
        return bitsWriter_.GetResult();
    }
    else
        return std::vector<unsigned char>(0);
}

// получить таблицу префиксов и строку в зашифрованном виде
// в начале печатается количество уникальных символов в строке
// Схема кодировки
// (количество уникальных символов)(глубина дерева)(код_символа_в_дереве,8_битный_код_символа)...
// ...(код_символа_в_дереве,8_битный_код_символа)(зашифрованная_строка)(количество_использованных_разрядов_в_предыдущем_байте)
std::vector<unsigned char> HuffmanEncoder::getStringAndTableCopmressed()
{
    if(isBuilt)
    {
        bitsWriter_.clean();
        bitsWriter_.writeByte(charFrequencies_.size());
        bitsWriter_.writeByte(getMaxDepthOfTree_(root_));
        
        auto nodeHandler = [](Node* node, BitsWriter& outputStream)->void
        {

            if(node->left && node->right)
            {
                outputStream.writeBit(false);
            }
            else
            {
                outputStream.writeBit(true);
                outputStream.writeByte(node->chars[0]);
            }
        };
        
        doPreOrder_(nodeHandler);
        
        std::string strCompressed = getStringCompressed();
        
        for(char& c : strCompressed)
            bitsWriter_.writeBit((bool)( c == '1'));
        return bitsWriter_.GetResult();
    }
    else
        return std::vector<unsigned char>(0);
}

int HuffmanEncoder::getMaxDepthOfTree_(Node* node)
{
    if (node == NULL)  
        return 0;  
    else
    {  
        // считатем глубины поддеревьев
        int lDepth = getMaxDepthOfTree_(node->left);  
        int rDepth = getMaxDepthOfTree_(node->right);  
      
        // выбирем наибольшую
        if (lDepth > rDepth)  
            return (lDepth + 1);  
        else return (rDepth + 1);  
    }
}

int HuffmanEncoder::getTreeDepth()
{
    return getMaxDepthOfTree_(root_);
}





// класс побитового считывателя
class BitReader 
{
    public:
        void writeBytes(const std::vector<unsigned char>& inputChars);
        void writeBit(bool bitToPut);
        void writeByte(unsigned char ch);

        bool readBit();
        unsigned char readByte();
        //получить последний байт = количество использованных разрядов в предпоследнем байте
        //без изменения индекса текущего читаемого бита
        unsigned char getLastByte();

        bool hasString();

        // начать читать с начала
        void resetReading();
        // очистить считыватель
        void clean();
        
        //битовая длина (включая недоиспользованные разряды последнего бита)
        int getBufferBitLength();
        //количество записанних битов = битовая длина - количество неиспользованных битов
        //в последнем байте
        int getNumberOfBitsWritten();
        //текущий читаемый бит
        int getCurrentReadIdx();
        

    private:
        std::vector<unsigned char> charsToDecode_;
        // idxBitToRead_  = индекс читаемого бита при вызове readBit()
        int idxBitToRead_ = 0;
        // idxBitToWrite_ = индекс, в который будет записан бит при вызове writeBit()
        int idxBitToWrite_ = 0;
        bool hasString_ = false;
};

void BitReader::writeBytes(const std::vector<unsigned char>& inputChars)
{
    charsToDecode_.clear();
    charsToDecode_.assign(inputChars.begin(), inputChars.end());
    
    idxBitToRead_ = 0;
    idxBitToWrite_ = 0;
    hasString_ = true;
}

void BitReader::writeByte(unsigned char ch)
{
    hasString_ = true;

    int bitIdx = (idxBitToWrite_ % 8);
    int byteIdx = (idxBitToWrite_ >> 3);

    if (bitIdx == 0)
    {
        charsToDecode_.push_back(ch);
        idxBitToWrite_ += 8;
    } 
    else 
    {
        charsToDecode_[byteIdx] |= ch << bitIdx;
        charsToDecode_.push_back(0);
        charsToDecode_[byteIdx + 1] |= ch >> (8 - bitIdx);
        idxBitToWrite_ += 8;
    }
}

void BitReader::writeBit(bool bitToPut)
{
    hasString_ = true;
    
    int bitIdx = (idxBitToWrite_ % 8);
    int byteIdx = (idxBitToWrite_ >> 3);
    
    if(bitIdx > 0)
    {
        charsToDecode_[byteIdx] |= static_cast<unsigned char>(bitToPut) << bitIdx;
    }
    else
    {
        charsToDecode_.push_back(0);
        charsToDecode_[byteIdx] |= static_cast<unsigned char>(bitToPut) << bitIdx;
    }
    idxBitToWrite_ ++;

}

bool BitReader::readBit() 
{
    if (hasString_ && idxBitToRead_ < idxBitToWrite_)
    {

        int bitIdx =  (idxBitToRead_ % 8);
        int byteIdx = (idxBitToRead_ >> 3);

        idxBitToRead_++;
        return (bool)((charsToDecode_[byteIdx] >> (bitIdx)) & 1);
    }
    else
    {
        return hasString_; 
    }
    
}

unsigned char BitReader::readByte()
{
    
    if (hasString_ && idxBitToRead_ + 7 < idxBitToWrite_ )
    {
        int bitIdx =  (idxBitToRead_ % 8);
        int byteIdx = (idxBitToRead_ >> 3);

        unsigned char result = 0;

        if(bitIdx == 0 && byteIdx < charsToDecode_.size())
        {
            idxBitToRead_ += 8;
            return charsToDecode_[byteIdx];
        }
        else if(bitIdx == 0)
        {
            return charsToDecode_[byteIdx - 1];
        }
        else if(byteIdx + 1 < charsToDecode_.size())
        {
            result |= (charsToDecode_[byteIdx] >> bitIdx);
            result |= (charsToDecode_[byteIdx + 1] << (8- bitIdx));
            idxBitToRead_ += 8;
            return result;           
        }
        else
        {
            return charsToDecode_[byteIdx];
        }
            
    }
    else if (hasString_ && idxBitToRead_ + 7 >=  idxBitToWrite_ )
    {
        int byteIdx = (idxBitToRead_ >> 3);
        return charsToDecode_[byteIdx];
    }
    else
    {
        return (unsigned char) hasString_;
    }
}

void BitReader::resetReading()
{
    idxBitToRead_ = 0;
}

void BitReader::clean()
{
    charsToDecode_.clear();
    hasString_ = false;
    idxBitToRead_ = 0;
    idxBitToWrite_ = 0;
}

bool BitReader::hasString()
{
    return hasString_;
}

int BitReader::getNumberOfBitsWritten()
{
    return idxBitToWrite_;
}


int BitReader::getBufferBitLength()
{
    return 8*charsToDecode_.size();
}

int BitReader::getCurrentReadIdx()
{
    return idxBitToRead_;
}

unsigned char BitReader::getLastByte()
{
    if(hasString_)
        return charsToDecode_.back();
}


// класс декодера
class HuffmanDecoder
{
    public:
        explicit HuffmanDecoder(const std::vector<unsigned char>& inputChars)
            : root_(nullptr), maxLeaves_(0), charsToDecompress_(inputChars), isBuilt_(false) {}
        HuffmanDecoder(const std::string& inputChars)
            : root_(nullptr), maxLeaves_(0), charsToDecompress_(inputChars.begin(), inputChars.end()), isBuilt_(false) {}
        HuffmanDecoder()
            : root_(nullptr), maxLeaves_(0), isBuilt_(false) {}
        ~HuffmanDecoder();
        
        HuffmanDecoder(const HuffmanDecoder&) = delete;
        HuffmanDecoder(HuffmanDecoder&& other) = delete;
        HuffmanDecoder& operator=(const HuffmanDecoder&) = delete;
        HuffmanDecoder& operator=(HuffmanDecoder&&) = delete;

        //метод для загрузки данных в виде массива байтов / строки / одиночного байта
        void putBytesToDecode(const std::vector<unsigned char>& inputChars);
        void putBytesToDecode(const std::string& inputChars);
        void putSigleByteToDecode(unsigned char inputChar);

        bool isTreeBuilt();

        //напечатать строку, которую необхдимо расшифровать
        void printStringToDecompress();
        //получить расшифрованные данные в виде строки
        std::string getDecodedString();
        //получить расшифрованные данные в виде vector<unsigned char>
        std::vector<unsigned char> getDecodedStringAsCharVector();
       
        // удаление данных
        void reset();

        //построение префиксного дерева и расшифровка данных
        void buildDecodingTreeAndDecodeData();
        //печать префиксного дерева
        void printDecodingTree();
        

    private:
        struct Node
        {
            int key;
            unsigned char symbol;
            Node* left;
            Node* right;
            Node()
                : key(0), left(nullptr), right(nullptr), symbol('\0') {} ;
            
            bool operator==(const Node& other) const
            {
                return (key == other.key && left == other.left && right == other.right && symbol == other.symbol) ;
            }
        };

        //корень
        Node* root_;
        //количество листьев в дереве
        int maxLeaves_;
        //высота дерева
        int treeDepth_;
        //сжатые данные
        std::vector<unsigned char> charsToDecompress_;
        //расшифрованные данные
        std::vector<unsigned char> decodedString_;
        //построено ли дерево
        bool isBuilt_;

        bool wasPutSigleByteToDecodeUsed_ = false;
        //декодер
        BitReader charDecoder;

        //метод получения числа листьев в префиксном дереве
        int  getMaxNumberOFLeaves_();
        //метод получения глубины дерева
        int getTreeDepth_();
        //построение пустого дерева
        void buildEmptyTree_(Node* node, int  currentLevel, int maxDepth);
        //метод восстановления префиксного дерева
        void decodePrefixTree_();

        //метод для удаления дерева
        using NodeCallBack = void (*)(Node*);
        void doPostOrder_(Node* rootNode, NodeCallBack visitNode);
        
        //метод прямого обхода для восстановления префиксного дерева
        using NodeCallBackPreOrder = void (*)(Node*, BitReader&, NodeCallBack);
        void doPreOrder_(Node* rootNode, NodeCallBackPreOrder visitNode);

        //метод печати дерева, используемый в обёртке void printDecodingTree();
        void printTree_(Node* node, int level);

        //метод расшифровки расшифровка данных
        void decodeData_();

        
};

HuffmanDecoder::~HuffmanDecoder()
{
    //обойдем дерево и удалим каждый из узлов
    doPostOrder_(root_, [](Node* node)->void{delete node; });
}

void HuffmanDecoder::putBytesToDecode(const std::vector<unsigned char>& inputChars)
{
    wasPutSigleByteToDecodeUsed_ = false;
    charDecoder.clean();
    charsToDecompress_.clear();
    bool ifOne;
    for (unsigned char byte : inputChars) 
    {
        for (int i = 0; i < 8; ++i) 
        {
            ifOne = ((byte >> i) & 1);
            if (ifOne)
                charsToDecompress_.push_back((unsigned char) '1');
            else 
                charsToDecompress_.push_back((unsigned char) '0');
        }   
    }
}

void HuffmanDecoder::putBytesToDecode(const std::string& inputChars)
{
    wasPutSigleByteToDecodeUsed_ = false;
    charDecoder.clean();
    charsToDecompress_.clear();
    std::vector<unsigned char> bufferVector (inputChars.begin(), inputChars.end());
    charsToDecompress_ = bufferVector;
}

void HuffmanDecoder::putSigleByteToDecode(unsigned char inputChar)
{
    /*
    bool ifOne;
    for (int i = 0; i < 8; ++i) 
    {
        ifOne = ((inputChar >> i) & 1);
        if (ifOne)
            charsToDecompress_.push_back((unsigned char) '1');
        else 
            charsToDecompress_.push_back((unsigned char) '0');
    }   
    */
   charDecoder.writeByte(inputChar);
   wasPutSigleByteToDecodeUsed_ = true;
}

void HuffmanDecoder::printStringToDecompress()
{
    int counter = 0;
    for (unsigned char& ch : charsToDecompress_)
    {
        if (counter % 8 == 0)
            std::cout << " ";
        counter++;
        std::cout << ch;
    }
    std::cout << std::endl;
}

bool HuffmanDecoder::isTreeBuilt()
{
    return isBuilt_;
}

std::string HuffmanDecoder::getDecodedString()
{
    if(isBuilt_)
        return std::string(decodedString_.begin(), decodedString_.end());
}

std::vector<unsigned char> HuffmanDecoder::getDecodedStringAsCharVector()
{
    if(isBuilt_)
        return decodedString_;
}

void HuffmanDecoder::reset()
{
    if (isBuilt_)
    {
        wasPutSigleByteToDecodeUsed_ = false;
        doPostOrder_(root_, [](Node* node)->void{delete node; });
        root_ = nullptr;
        maxLeaves_ = 0;
        charsToDecompress_.clear();
        decodedString_.clear();
        isBuilt_ = false;
    }
    else
    {
        wasPutSigleByteToDecodeUsed_ = false;
        charsToDecompress_.clear();  
    }
}

//метод печати префиксного дерева - обёртка над printTree_
void HuffmanDecoder::printDecodingTree()
{
    if(isBuilt_)
        printTree_(root_, 0);
}

//метод построения префиксного дерева и декодировки данных
void HuffmanDecoder::buildDecodingTreeAndDecodeData()
{
    if(charsToDecompress_.size() || wasPutSigleByteToDecodeUsed_)
    {
        
        //побитная запись данных в декодер
        if(!wasPutSigleByteToDecodeUsed_)
        {
            charDecoder.clean();
            for(int i =0; i <charsToDecompress_.size(); i++)
                charDecoder.writeBit((charsToDecompress_[i]) == '1');
        }

        
        //получение числа листьев в дереве
        maxLeaves_ = getMaxNumberOFLeaves_();
        //определение корня дерева
        treeDepth_ = getTreeDepth_();
        root_ = new Node();
        
        //построение пустого дерева глубины = treeDepth_
        buildEmptyTree_(root_, 0, treeDepth_);

        //восстановление префиксного дерева
        decodePrefixTree_();
        
        isBuilt_ = true;

        //декодирование данных
        decodeData_();
    }
    else
        return;
}

//метод получения числа листьев в дереве = первый байт
int HuffmanDecoder::getMaxNumberOFLeaves_()
{
    return static_cast<int>(charDecoder.readByte());
}

int HuffmanDecoder::getTreeDepth_()
{
    return static_cast<int>(charDecoder.readByte());
}

//рекурсивное построение пустого дерева
//глубины maxDepth
//currentLevel = текущая глубина уровня
void HuffmanDecoder::buildEmptyTree_(Node* node, int currentLevel, int maxDepth)
{
    //пока не достигли maxDepth, создаём потомков
    if(currentLevel < maxDepth)
    {
        node->left = new Node();
        node->right = new Node();
        node->key = maxDepth - currentLevel;
        node->symbol = '\0';
        buildEmptyTree_(node->left,  currentLevel+1, maxDepth);
        buildEmptyTree_(node->right, currentLevel+1, maxDepth);
    }
    else
    {
        //если уровень = maxDepth, завершаем построение дерева
        node->key = maxDepth - currentLevel;
        node->left = nullptr;
        node->right = nullptr;
        node->symbol = '\0';
        return;
    }
    
}

//метод восстановления префиксного дерева
//используется методом прямого обхода doPreOrder_
//удаление лиших узлов происходит внутри doPreOrder_
void HuffmanDecoder::decodePrefixTree_()
{
    //обработка узла: если узел - лист, то считываем байт и записываем
    //соответствующий ему символ в узел
    auto nodeHandler = [](Node* node, BitReader& inputStream, NodeCallBack doWithNode)->void
    {
        bool ifLeaf;
        ifLeaf = (bool) inputStream.readBit();
        if(ifLeaf)
            node->symbol = inputStream.readByte();  
    };
    doPreOrder_(root_, nodeHandler);
}

//метод обратного обхода для удаления дерева
void HuffmanDecoder::doPostOrder_(Node * rootNode, NodeCallBack doWithNodes)
{
    std::stack<Node*> nodeStack;
    Node* currentNode;
    currentNode = rootNode;
    Node* lastVisitedNode = nullptr;
    Node* topOfStack = nullptr;
 
    while ((nodeStack.size() > 0) or currentNode)
    {
        if (currentNode) 
        {
            nodeStack.push(currentNode);
            currentNode = currentNode->left;
        } 
        else
        {
            topOfStack = nodeStack.top();
            if (topOfStack->right and lastVisitedNode != topOfStack->right)
                currentNode = topOfStack->right;                
            else 
            {
                nodeStack.pop();
                lastVisitedNode = topOfStack;
                doWithNodes(topOfStack);
            }
        }
    }
}

//метод прямого обхода для восстановления префиксного дерева
void HuffmanDecoder::doPreOrder_(Node* node, NodeCallBackPreOrder visitNode) 
{
    std::stack<Node*> nodeStack;
    Node* currentNode;
    Node* bufferNode;
    currentNode = node;

    while (nodeStack.size() != 0 || currentNode != nullptr) 
    {
        if (currentNode != nullptr) 
        {
            visitNode(currentNode, charDecoder, [](Node* node)->void{return; });
            
            //если обрабтчиком выше был встречен лист и в него был записан ненулевой символ,
            // то удалаем правое и левое поддеревья и делаем узел листом
            if(currentNode->symbol != '\0')
            {
                if(currentNode->left)
                    doPostOrder_(currentNode->left,  [](Node* node)->void{ delete node; });
                if(currentNode->right)
                    doPostOrder_(currentNode->right,  [](Node* node)->void{delete node; });
                currentNode->left = nullptr;
                currentNode->right = nullptr;
            }

            if (currentNode->right)
                nodeStack.push(currentNode->right);
            currentNode = currentNode->left;
        } 
        else 
        {
            currentNode = nodeStack.top();
            nodeStack.pop();
        }
    }
}

//метод печати префиксного дерева
void HuffmanDecoder::printTree_(Node * node, int level)
{
    if (!node)
        return;
    
    level += 3;
    printTree_(node->right, level);
    std::cout << std::endl;

        
    for(int i = 3; i < level; ++i)
        std::cout << "  ";
        
    std::cout << node->key << " " << node->symbol << std::endl;
    printTree_(node->left, level);
}

//метод расшифровки данных
//метод расшифровки данных
void HuffmanDecoder::decodeData_()
{
    // считываем из последнего байта, сколько разрядов было использовано
    // в предыдущем байте
    int digitsInLastByte = static_cast<int>( charDecoder.getLastByte());
    // lastIdxToDecode - количество бит, в которых содержится зашифрованные данные
    if (digitsInLastByte == 0) 
        digitsInLastByte = 8;
    int lastIdxToDecode = charDecoder.getBufferBitLength() - 16 + digitsInLastByte; ;


    Node* currentNode = root_;
    bool goRight;
    
    //считываем бит = нужно ли идти вправо по дереву
    //goRight = charDecoder.readBit();
    while(charDecoder.getCurrentReadIdx() < lastIdxToDecode)
    {
        goRight = charDecoder.readBit();
        if (goRight)
            currentNode = currentNode->right;
        else
            currentNode = currentNode->left;
        
        //если попали в лист, считываем записанный в нём символ и возвращаемся в корень
        if(currentNode->left == nullptr && currentNode->left == nullptr)
        {
            decodedString_.push_back(currentNode->symbol);
            currentNode = root_;
        }
        
    }
}

typedef unsigned char byte;

static void listenStreamAndEncode(IInputStream& input, IOutputStream& output)
{
	byte value;
    HuffmanEncoder charHuffmanEncoder;

	while (input.Read(value))
		charHuffmanEncoder.addChar(value);

    charHuffmanEncoder.buildTree();
    std::vector<byte>  result = std::move(charHuffmanEncoder.getStringAndTableCopmressed());

    for(int i = 0; i < result.size(); ++i)
        output.Write(result[i]);
}

static void listenStreamAndDecode(IInputStream& input, IOutputStream& output)
{
	byte value;
    HuffmanDecoder charHuffmanDecoder;

	while (input.Read(value))
		charHuffmanDecoder.putSigleByteToDecode(value);

    charHuffmanDecoder.buildDecodingTreeAndDecodeData();
    
    vector<byte> result = std::move(charHuffmanDecoder.getDecodedStringAsCharVector());
    
    for(int i = 0; i < result.size(); ++i)
        output.Write(result[i]);
}

void Encode(IInputStream& original, IOutputStream& compressed) 
{
	listenStreamAndEncode(original, compressed); 
}

void Decode(IInputStream& compressed, IOutputStream& original) 
{ 
	listenStreamAndDecode(compressed, original); 
}
