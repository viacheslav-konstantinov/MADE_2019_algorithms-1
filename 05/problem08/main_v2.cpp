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

#include "Huffman.h"
#include <string>
#include <queue>
#include <vector>
#include <fstream>
#include <iostream>
#include <bitset>
#include <stack>
#include <functional>

class HuffmanEncoderDecoder
{
	public:
		HuffmanEncoderDecoder();
		void buildCharFreqOrderedDict();      // составление упорядоченного словаря частот символов
		void buildEncodingTree();             // составление кодирующего дерева на основе частот символов
		void calculateCharHuffmanCodes();     // вычисление кодов каждого из символов в дереве
		void writeEncodedData();              //  сохранить сжатые данные
		void writeDecodedData();              // сохранить раскодированные данные
		void decodeEncodingTree();            // восстановление кодирующего дерева
		void writeByte(unsigned char, bool);  // запись байта, флаг определяет режим работы6 сжатие или рашифровка. По умолчанию - сжатие
		std::vector<unsigned char> getResult();    // получить итоговые данные: если работа в режиме сжатия, то - зашифрованные данные и наоборот
		~HuffmanEncoderDecoder();             // деструктор, удаляющий кодирующее дерево обратным проходом

        HuffmanEncoderDecoder(const HuffmanEncoderDecoder&) = delete;
        HuffmanEncoderDecoder(HuffmanEncoderDecoder&& other) = delete;
        HuffmanEncoderDecoder& operator=(const HuffmanEncoderDecoder&) = delete;
        HuffmanEncoderDecoder& operator=(HuffmanEncoderDecoder&&) = delete;

	
	private:
		struct Node
		{
			unsigned char id; // символ
			int freq;         //частота
			std::string code;      //код символа в кодирующем дереве
			Node* left;
			Node* right;
			Node()
			{
				left = nullptr;
				right = nullptr;
			}

			bool operator==(const Node& other) const
            {
                return (id == other.id && freq == other.freq && code == other.code && left == other.left && right == other.right );
            }
		};

	using nodePtr = Node*;
	nodePtr nodeArray_[256]; // массив узлов с частотами каждого из 256 символов
	nodePtr root_;           // корень кодирующего дерева
	void buildEncodingTree_(std::string&, unsigned char); // метод для восстановления дерева: строка - путь в дереве, символ - символ из nodeArray_
	std::vector<unsigned char> inputChars_;  // массив входных символов
	std::vector<unsigned char> outputChars_; // массив выходных символов
	
	// класс компаратора узлов дерева
	// сравнение по частотам
	class nodeComparator
	{
		public:
			bool operator()(const nodePtr& Node1, const nodePtr& Node2) const
			{
				return Node1->freq > Node2->freq;
			}
	};

	// класс обработчика узла
	using NodeCallBack = void (*)(nodePtr);
	// метод обратного обхода дерево - используется в деструкторе
    void doPostOrder_(nodePtr rootNode, NodeCallBack visitNode);

	// упорядоченная по возрастанию очередь узлов - необходима при построении кодирующего дерева
	std::priority_queue<nodePtr, std::vector<nodePtr>, nodeComparator> charFrequenciesPriority_;

	//метод создание пустого массива частот
	void buildEmptyCharFreqArray_();																
	void traverse(nodePtr, std::string);
	int convertBinaryToDecimal_(const std::string&);
	std::string convertDeciminalToBinary_(int);
};


//метод обратного обхода для удаления дерева
void HuffmanEncoderDecoder::doPostOrder_(nodePtr rootNode, NodeCallBack doWithNodes)
{
    std::stack<nodePtr> nodeStack;
    nodePtr currentNode;
    currentNode = rootNode;
    nodePtr lastVisitedNode = nullptr;
    nodePtr topOfStack = nullptr;
 
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


HuffmanEncoderDecoder::~HuffmanEncoderDecoder()
{
    // обойдем дерево и удалим каждый из узлов
	if (root_)
    	doPostOrder_(root_, [](nodePtr node)->void{delete node; });
	else 
	{
	// удаляем неиспользованные узлы
		for (int i = 0; i < 256; i++)
		{
			if(nodeArray_[i])
				delete nodeArray_[i];
		}
	}
}

// массив узлов символов
void HuffmanEncoderDecoder::buildEmptyCharFreqArray_()
{
	for (int i = 0; i < 256; i++)
	{
		nodeArray_[i] = new Node;
		nodeArray_[i]->id = (unsigned char) i;
		nodeArray_[i]->freq = 0;
	}
}

// метод вычисления кода пути к листу
void HuffmanEncoderDecoder::traverse(nodePtr node, std::string code)
{
	if (node->left == nullptr && node->right == nullptr)
	{
		node->code = code;
	}
	else
	{
		traverse(node->left,  code + '0');
		traverse(node->right, code + '1');
	}
}

//преобразование из строки 0 и 1 в десятичное представление символа
int HuffmanEncoderDecoder::convertBinaryToDecimal_(const std::string& in)
{
	return std::stoi(in, nullptr, 2);
}

//преобразование кода символа в строку 0 и 1 - его двочного представления
std::string HuffmanEncoderDecoder::convertDeciminalToBinary_(int in)
{
    return std::bitset<8>(in).to_string();
}

//заполнение кодирующего дерева: path - путь к символу, a_code - символ
void HuffmanEncoderDecoder::buildEncodingTree_(std::string& path, unsigned char a_code)
{
	nodePtr current = root_;
	for (char& goRight : path)
	{
		if (goRight == '0')
		{
			if (current->left == NULL)
				current->left = new Node;
			current = current->left;
		}
		else if (goRight== '1')
		{
			if (current->right == NULL)
				current->right = new Node;
			current = current->right;														 
		}
	}
	current->id = a_code;
}


HuffmanEncoderDecoder::HuffmanEncoderDecoder()
{
	buildEmptyCharFreqArray_();
	inputChars_.clear();
	outputChars_.clear();
}

void HuffmanEncoderDecoder::writeByte(unsigned char ch, bool toEncode=true)
{
	if(toEncode)
		nodeArray_[ch]->freq++;
	inputChars_.push_back(ch);
}

// передача массива узлов в проиритетную очередь
void HuffmanEncoderDecoder::buildCharFreqOrderedDict()
{
	for (int i = 0; i < 256; i++)
	{
		if (nodeArray_[i]->freq)
			charFrequenciesPriority_.push(nodeArray_[i]);
	}
}

//публичный метод для получения кодирующего дерева
void HuffmanEncoderDecoder::buildEncodingTree()
{

	//копия упорядоченных узлов
	std::priority_queue<nodePtr, std::vector<nodePtr>, nodeComparator> temp(charFrequenciesPriority_);

	while (temp.size() > 1)
	{
		// берем два узла с наименьшими частотами приоритетами, объединяем в узел и отправляем обратно в очередь 
		// до тех пор, пока не останется 1 узел - корень
		root_ = new Node;
		root_->freq = 0;
		root_->left  = temp.top();
		root_->freq += temp.top()->freq;
		temp.pop();
		root_->right = temp.top();
		root_->freq += temp.top()->freq;
		temp.pop();
		temp.push(root_);
	}
}

// обертка для traverse - спуск и расчёт кодов символов в дереве
void HuffmanEncoderDecoder::calculateCharHuffmanCodes()
{
	traverse(root_, "");
}

// запись сжатых данных
// (количество листьев в дереве)((символ, его код в дереве)(1)(символ, его код в дереве)(1)...)(закодированные данные)(количество использованных битов в предыдущем байте)
// код символа кодируется 128 битами
void HuffmanEncoderDecoder::writeEncodedData()
{
	std::string buffer(""), s(""); // буферные строки

	buffer += (unsigned char)charFrequenciesPriority_.size();
	//копия приоритетной очереди
	std::priority_queue<nodePtr, std::vector<nodePtr>, nodeComparator> temp(charFrequenciesPriority_);

	while (!temp.empty())
	{	//запись кодов символов в порядке возрастания частоты
		nodePtr current = temp.top();

		//запись символа
		buffer += current->id;
		s.assign(127 - current->code.size(), '0'); //если код менее 128 бит, заполяем предстоящие быты нулями
		s += '1'; // разделитель
		s.append(current->code);

		// побайтная запись кода символа в кодирующем дереве
		// 128/8 = 16 байт 
		buffer += (unsigned char)convertBinaryToDecimal_(s.substr(0, 8));										
		for (int i = 0; i < 15; i++)
		{
			s = s.substr(8);
			buffer += (unsigned char)convertBinaryToDecimal_(s.substr(0, 8));
		}
		temp.pop();
	}
	s.clear();

	//кодировка данных
	for (int i = 0; i < inputChars_.size(); ++i)
	{
		s += nodeArray_[inputChars_[i]]->code;
		while (s.size() > 8)
		{
			buffer += (unsigned char)convertBinaryToDecimal_(s.substr(0, 8));
			s = s.substr(8);
		}
	}
	
	// количество битов, использованных в последнем байте
	int count = 8 - s.size();

	// добивание до байта
	if (s.size() < 8)
		s.append(count, '0');

	// запись последнего байта с данным и числа, равного количеству битов, использованных в последнем байте
	buffer += (unsigned char)convertBinaryToDecimal_(s);
	buffer += (unsigned char)count;

	// сохранение сжатых данных
	outputChars_.clear();
	outputChars_.assign(buffer.begin(), buffer.end());
}

//метод восстановления кодирующеего дерева
void HuffmanEncoderDecoder::decodeEncodingTree()
{
	unsigned char size = inputChars_[0];
	root_ = new Node;
	for (int i = 0; i < size; ++i)
	{
		unsigned char charIntCode; // символ
		unsigned char charHuffmanCodeRaw[16]; // строка с кодом символа в кодирующем дереве	
		
		charIntCode = inputChars_[1 + i*17]; 
		for(int y = 0; y < 16; ++y)
			charHuffmanCodeRaw[y] = inputChars_[1 + i*17 + 1 + y];
		
		std::string charHuffmanTrueCode = ""; // строка, куда будет сохранён истинный код символа без дополнительных нулей
										// и разделяющей "1"
		
		//побитное считывание и запись в charHuffmanTrueCode
		for (int z = 0; z < 16; z++)
			charHuffmanTrueCode += convertDeciminalToBinary_(charHuffmanCodeRaw[z]);

		
		int endOfStr = 0;
		while (charHuffmanTrueCode[endOfStr] == '0')
			endOfStr++;

		// удаление хвоста из нулей и пограничной "1"
		charHuffmanTrueCode = charHuffmanTrueCode.substr(endOfStr + 1);
		// запись инфорамции о символе в дерево
		buildEncodingTree_(charHuffmanTrueCode, charIntCode);
	}
}

//метод расшифровки данных
void HuffmanEncoderDecoder::writeDecodedData()
{
	outputChars_.clear();
	unsigned char size = inputChars_[0];       // количество уникальных символов
	unsigned char count0 = inputChars_.back(); // количество использованных битов в предпоследем байте
	
	// рашифрованный данные
	std::vector<unsigned char> text;

	//считываем данные о символах и их кодах
	for(auto item = inputChars_.begin() + 1 + 17*(int)size; item != inputChars_.end(); ++item)
		text.push_back(*item);


	nodePtr current = root_;
	std::string path;

	for (int i = 0; i < text.size() - 1; i++)
	{
		// идем по битам и по дереву: 0 - налево, 1 - направо
		// если встречаем лист, записываем символ в нём и возвращаемся в корень
		path = convertDeciminalToBinary_(text[i]);
		if (i == text.size() - 2)
			path = path.substr(0, 8 - count0);

		for (int j = 0; j < path.size(); j++)
		{
			if (path[j] == '0')
				current = current->left;
			else
				current = current->right;
			if (current->left == nullptr && current->right == nullptr)
			{
				outputChars_.push_back(current->id);
				current = root_;
			}
		}
	}
}

// получение зашифрованных/расшифрованных данных
std::vector<unsigned char> HuffmanEncoderDecoder::getResult()
{
	return outputChars_;
}

using byte = unsigned char;

// сжатие данных
static void listenStreamAndEncode(IInputStream& input, IOutputStream& output)
{
	byte value;
    HuffmanEncoderDecoder charHuffmanEncoder;

	while (input.Read(value))
		charHuffmanEncoder.writeByte(value);

	charHuffmanEncoder.buildCharFreqOrderedDict();
	charHuffmanEncoder.buildEncodingTree();
	charHuffmanEncoder.calculateCharHuffmanCodes();
	charHuffmanEncoder.writeEncodedData();
	vector<unsigned char> result = std::move(charHuffmanEncoder.getResult());

    for(int i = 0; i < result.size(); ++i)
        output.Write(result[i]);
}

// расшифровка данных
static void listenStreamAndDecode(IInputStream& input, IOutputStream& output)
{
	byte value;
    HuffmanEncoderDecoder charHuffmanDecoder;

	while (input.Read(value))
		charHuffmanDecoder.writeByte(value, false);

    charHuffmanDecoder.decodeEncodingTree();
	charHuffmanDecoder.writeDecodedData();
    
    vector<unsigned char> result = std::move(charHuffmanDecoder.getResult());
    
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
