/*
6_1. Обход дерева в порядке pre-order
Ограничение времени	0.2 секунды
Ограничение памяти	64Mb
Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt

Дано число N < 106 и последовательность целых чисел из [-231..231] длиной N. 
Требуется построить бинарное дерево, заданное наивным порядком вставки. 
Т.е., при добавлении очередного числа K в дерево с корнем root, 
если root→Key ≤ K, то узел K добавляется в правое поддерево root; иначе в левое 
поддерево root. Выведите элементы в порядке pre-order (сверху вниз).

Рекурсия запрещена.
*/

#include <iostream>
#include <algorithm>
#include <stack>

using namespace std;
class BinaryTree
{   
    public:
        BinaryTree(const int keyOfRoot)
            : size_(1), root_(new Node(keyOfRoot)) {}
        
        ~BinaryTree();
        BinaryTree(const BinaryTree&) = delete;
        BinaryTree(BinaryTree&& other) = delete;
        BinaryTree& operator=(const BinaryTree&) = delete;
        BinaryTree& operator=(BinaryTree&&) = delete;

        void Add(const int key);
        void PrintTreePostOrder();

    private:
        struct Node
        {
            int key;
            Node* left = nullptr;
            Node* right = nullptr;
            Node(Node* node)
                : key(node->key), left(node->left), right(node->right) {};
            Node(const int key)
                : key(key),  left(nullptr), right(nullptr) {};

            bool operator==(const Node& other) const
            {
                return (key == other.key && left == other.left && right == other.right);
            }
        };
        
        int size_;
        Node* root_;

        using keyCallback = void (*)(int);
        using nodeCallback = void (*)(Node*);
        // DoPostOrder_ используется в методе печати и деструкторе
        void DoPostOrder_(keyCallback, nodeCallback);
};

BinaryTree::~BinaryTree()
{
    //обойдем дерево и удалим каждый из узлов
    DoPostOrder_([](int x)->void{return; }, [](Node* node)->void{delete node; });
}
void BinaryTree::PrintTreePostOrder()
{
    //обойдем дерево и напечататем каждый из узлов
    //можно было бы обойтись и без этого метода,
    //если в деструкторе перед удалением узла печатать значение ключа
    DoPostOrder_([](int x)->void{cout << x << " " ;}, [](Node* node)->void{return; });
}

//метод обхода
void BinaryTree::DoPostOrder_(keyCallback doWithKey, nodeCallback doWithNodes)
{
    stack<Node*> nodeStack;
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
                doWithKey(topOfStack->key);
                lastVisitedNode = topOfStack;
                doWithNodes(topOfStack);
            }
        }
    }
}

void BinaryTree::Add(const int key)
{
    size_ += 1;
    
    Node* nodeToAdd = new Node(key);
    Node* currentNode = root_;

    while(currentNode != nullptr)
    {
        if (key > currentNode->key)
        {
            if (currentNode->right)
                currentNode = currentNode->right;
            else
            {
                currentNode->right = nodeToAdd;
                break;
            }
        }
        else
        {
            if(currentNode->left)
                currentNode = currentNode->left;
            else
            {
                currentNode->left = nodeToAdd;
                break;
            }
        }
    }
}

int main()
{
   size_t treeSize;
   int keyBuffer;
   
   cin >> treeSize;

   cin >> keyBuffer;
   BinaryTree myTree(keyBuffer);
   
   for (size_t i = 1; i < treeSize; ++i)
   {
       cin >> keyBuffer;
       myTree.Add(keyBuffer);
   }
   
   myTree.PrintTreePostOrder();

   return 0;
}
