// 2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using namespace std;

enum class NodeType
{
    ADD,
    SUB,
    MUL,
    DIV,
    VALUE
};

class Node
{
public:
    NodeType type;
    int value;
    Node* left;
    Node* right;
    Node(NodeType type, int value = 0, Node* left = nullptr, Node* right = nullptr) : type(type), value(value), left(left), right(right)
    {

    }
};

class Tree
{
public:
    Node* root;
    
    static int analyze(const Node* node)
    {
        switch (node->type)
        {
        case (NodeType::VALUE):
            return node->value;
        case (NodeType::ADD):
            return analyze(node->left) + analyze(node->right);
        case (NodeType::SUB):
            return analyze(node->left) - analyze(node->right);
        case (NodeType::MUL):
            return analyze(node->left) * analyze(node->right);
        case (NodeType::DIV):
            return analyze(node->left) / analyze(node->right);
        }
        return 0;
    }
};

int main()
{
    Tree tree;
    tree.root = new Node(NodeType::ADD);
    tree.root->left = new Node(NodeType::VALUE, 12);
    tree.root->right = new Node(NodeType::VALUE, 5);
    cout << Tree::analyze(tree.root);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
