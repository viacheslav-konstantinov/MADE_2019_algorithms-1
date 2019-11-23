/*
7_1. Солдаты ДД

Ограничение времени	1 секунда
Ограничение памяти	500Mb

Ввод	стандартный ввод или input.txt
Вывод	стандартный вывод или output.txt

В одной военной части решили построить в одну шеренгу по росту. 
Т.к. часть была далеко не образцовая, то солдаты часто приходили не вовремя, 
а то их и вовсе приходилось выгонять из шеренги за плохо начищенные сапоги. 
Однако солдаты в процессе прихода и ухода должны были всегда быть выстроены по росту – 
сначала самые высокие, а в конце – самые низкие. За расстановку солдат отвечал прапорщик, 
который заметил интересную особенность – все солдаты в части разного роста.

Ваша задача состоит в том, чтобы помочь прапорщику правильно расставлять солдат, а 
именно для каждого приходящего солдата указывать, перед каким солдатом в строе он должен 
становится. Требуемая скорость выполнения команды - O(log n) в среднем.

В реализации используйте декартовы деревья.

Формат ввода
Первая строка содержит число N – количество команд (1 ≤ N ≤ 90 000).

В каждой следующей строке содержится описание команды: число 1 и X если солдат приходит 
в строй (X – рост солдата, натуральное число до 100 000 включительно) и число 2 и Y если 
солдата, стоящим в строе на месте Y надо удалить из строя. Солдаты в строе нумеруются с нуля.

Формат вывода
На каждую команду 1 (добавление в строй) вы должны выводить число K – номер позиции, на 
которую должен встать этот солдат (все стоящие за ним двигаются назад).

*/
#include <iostream>
#include <bits/stdc++.h>
#include <stack>

using namespace std;

class ArmyLine
{
    public:
        ArmyLine()
            : root_(nullptr) {}
        explicit ArmyLine(const int key)
            : root_(new Node(key)) {}
        ~ArmyLine();
          ArmyLine(const ArmyLine&) = delete;
        ArmyLine(ArmyLine&&) = delete;
        ArmyLine& operator=(const ArmyLine&) = delete;
        ArmyLine& operator=(ArmyLine&&) = delete;

        int getIdxOfkey(int key);
        
        int add(int key);
        int get(int idx);
        void remove(int idxToRemove);
        
        void print();
    
    private:
        struct Node
        {
            int key;
            int priority;
            int numberOfSubtrees; // количество поддеревьев в узле, включая сам узел
            Node* left;
            Node* right;
            
            explicit Node(int key)
                : key(key), left(nullptr), right(nullptr), priority(rand() % 1000000), numberOfSubtrees(1) {};
        };

        Node* root_;

        int getNumberOfSubtrees_(Node* node);
        void updateNumberOfSubtrees_(Node* node);
        int getKIndex_(Node* node, int k);

        Node* merge_(Node* T1, Node* T2);
        void split_(Node* T, int key, Node*& T1, Node*& T2);
        void RemoveByKey_(int key);

        // DoPostOrder_ используется в методе печати и деструкторе
        using keyCallback = void (*)(int);
        using nodeCallback = void (*)(Node*);
        void DoPostOrder_(Node*& node, keyCallback doWithKey, nodeCallback doWithNodes);
};

//метод получения количества поддеревьев в узле
int ArmyLine::getNumberOfSubtrees_(Node* node) 
{
    return node ? node->numberOfSubtrees: 0;
}

//метод обновления количества значения количества поддеревьев в узлах
void ArmyLine::updateNumberOfSubtrees_(Node* node)
{
    if(node)
        node->numberOfSubtrees = 1 +  getNumberOfSubtrees_(node->left) + getNumberOfSubtrees_(node->right);
}

//получение роста бойка на позиции k
int ArmyLine::getKIndex_(Node* node, int k)
    {
    if(k < getNumberOfSubtrees_(node->right))
        return getKIndex_(node->right, k);
    else if (k == getNumberOfSubtrees_(node->right))
        return node->key;
    else
        return getKIndex_(node->left, k - 1 - getNumberOfSubtrees_(node->right));
}

//стандартный merge_
//с обновлением числа подеревьев в корне после склеивания
ArmyLine::Node* ArmyLine::merge_(Node* T1, Node* T2)
{
    if(!T1) return T2;
    if(!T2) return T1;

    if(T1->priority > T2->priority)
    {
        T1->right = merge_(T1->right, T2);
            updateNumberOfSubtrees_(T1);
            return T1;
        }
    else
    {
        T2->left = merge_(T1, T2->left);
        updateNumberOfSubtrees_(T2);
            return T2;
    }
}

//стандартный split_ декартового дерева
//с обновлением числа подеревьев в корне после разрезания
void ArmyLine::split_(Node* T, int key, Node*& T1, Node*& T2)
{
    if(!T)
    {
        T1 = T2 = nullptr;
        return;
    }

    if(T->key < key)
    {
        split_(T->right, key, T->right, T2);
        T1 = T;
    }
    else
    {
        split_(T->left, key, T1, T->left);
        T2 = T;
    }
    updateNumberOfSubtrees_(T);
}

//удаление бойца с ростом key
void ArmyLine::RemoveByKey_(int key)
{
    Node *T1, *T2, *T3, *T4;
    
    split_(root_, key, T1, T2);
    split_(T2, key+1, T3, T4);

    root_ = merge_(T1, T4);

    delete T3;
}

//получение индекса - номера в строю по росту
int ArmyLine::getIdxOfkey(int key)
{
    if(!root_)
        return 0;
    int currentIdx(0), leftSubTrees(0), rightSubtrees(0);
            
    currentIdx = root_->numberOfSubtrees;

    if(root_->left) 
        leftSubTrees = root_->left->numberOfSubtrees;

    currentIdx = currentIdx - leftSubTrees;
            
    if (root_->key == key)
        return currentIdx - 1;
    
    Node* currentNode = root_;
    
    while(true)
    {
        if(currentNode->key > key)
        {
            currentNode = currentNode->left;
            
            if(currentNode->right) 
                rightSubtrees = currentNode->right->numberOfSubtrees;
            else
                rightSubtrees = 0;
            
            currentIdx = currentIdx + rightSubtrees + 1;
        }
                
        if(currentNode->key < key)
        {
            currentNode = currentNode->right;
            
            if(currentNode->left)  
                leftSubTrees = currentNode->left->numberOfSubtrees;
            else 
                leftSubTrees = 0;
            
            currentIdx = currentIdx - leftSubTrees - 1;
        }
                
        if(currentNode->key == key)
            return currentIdx - 1;
    }
}

//добавление бойца с ростом key
int ArmyLine::add(int key)
{
    Node *T1, *T2;

    split_(root_, key, T1, T2);
    Node* nodeToAdd = new Node(key);

    root_ = merge_(merge_(T1, nodeToAdd), T2);

    return getIdxOfkey(key);
}

//удаление из строя бойца на позиции idxToRemove
void ArmyLine::remove(int idxToRemove)
{
    RemoveByKey_(getKIndex_(root_, idxToRemove));
}

//получить рост по номеру в строю
//дополнительный к getKIndex_ нужен, чтобы не передавать ссылку на root_
int ArmyLine::get(int idx)
{
    return getKIndex_(root_, idx);
}

//метод обхода, необходимый для удаления и печати
void ArmyLine::DoPostOrder_(Node*& node, keyCallback doWithKey, nodeCallback doWithNodes)
{
    stack<Node*> nodeStack;
    Node* currentNode;
    currentNode = node;
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

// печать дерева
void ArmyLine::print()
{
    DoPostOrder_(root_, [](int x)->void {cout << x << " ";}, [](Node* node) ->void {return;});
    cout << endl;
    return;
}

//удаление дерева
ArmyLine::~ArmyLine()
{
    //обойдем дерево и удалим каждый из узлов
    DoPostOrder_(root_, [](int x)->void{return;}, [](Node* node)->void{delete node; });
}

int main()
{
    
    int numberOfComands;
    cin >> numberOfComands;

    ArmyLine myLine;
    int typeOfComand;
    int comandArgument;

    for(int i = 0; i< numberOfComands; ++i)
    {
        cin >> typeOfComand >> comandArgument;
        if(typeOfComand == 1)
            cout << myLine.add(comandArgument) << endl;
                        
        else if (typeOfComand == 2)
            myLine.remove(comandArgument);
    }
    
    //myLine.print();

    return 0;
}
