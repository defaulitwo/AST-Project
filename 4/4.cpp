#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
using namespace std;

enum class NodeType
{
    BINARY_OP,
    IF,
    VALUE,
    IDENTIFIER,
    ASSIGN,
};

enum class Operation
{
    ADD,
    SUB,
    MUL,
    DIV,
    LESS_THAN,
    GREATER_THAN,
};

class Node
{
public:
    NodeType type;
    Node(NodeType t) : type(t) {}
};

class ValueNode : public Node
{
public:
    double value;
    ValueNode(double v) : Node(NodeType::VALUE), value(v) {}
};

class IdentifierNode : public Node
{
public:
    string name;
    IdentifierNode(string n) : Node(NodeType::IDENTIFIER), name(n) {}
};

class BinaryOpNode : public Node
{
public:
    Operation operation;
    Node* left;
    Node* right;
    BinaryOpNode(Operation op, Node* l = nullptr, Node* r = nullptr) : Node(NodeType::BINARY_OP), operation(op), left(l), right(r) {}
};

class AssignNode : public Node
{
public:
    IdentifierNode* target;
    Node* value;
    AssignNode(IdentifierNode* t = nullptr, Node* v = nullptr) : Node(NodeType::ASSIGN), target(t), value(v) {}
};

class IfNode : public Node
{
public:
    Node* condition;
    Node* thenBranch;
    Node* elseBranch;
    IfNode(Node* c = nullptr, Node* t = nullptr, Node* e = nullptr) : Node(NodeType::IF), condition(c), thenBranch(t), elseBranch(e) {}
};

class AST
{
public:
    Node* root;

    //double analyzeTree() const
    //{
    //    return analyze(root);
    //}

    //void generateIR() const
    //{
    //    generate(root);
    //}

private:
    //static void emit(const string& instruction)
    //{
    //    outputFile << instruction << endl;
    //}

    //static void generate(const Node* node)
    //{
    //    switch (node->type)
    //    {
    //    case NodeType::VALUE:
    //        emit("LOAD" + to_string(node->value));
    //        break;
    //    case NodeType::ADD:
    //        generate(node->left);
    //        generate(node->right);
    //        emit("ADD");
    //        break;
    //    case NodeType::SUB:
    //        generate(node->left);
    //        generate(node->right);
    //        emit("SUB");
    //        break;
    //    case NodeType::MUL:
    //        generate(node->left);
    //        generate(node->right);
    //        emit("MUL");
    //        break;
    //    case NodeType::DIV:
    //        generate(node->left);
    //        generate(node->right);
    //        emit("DIV");
    //        break;
    //    }
    //}

    //static double analyze(const Node* node)
    //{
    //    switch (node->type)
    //    {
    //    case NodeType::VALUE:
    //        return node->value;
    //    case NodeType::ADD:
    //        return analyze(node->left) + analyze(node->right);
    //    case NodeType::SUB:
    //        return analyze(node->left) - analyze(node->right);
    //    case NodeType::MUL:
    //        return analyze(node->left) * analyze(node->right);
    //    case NodeType::DIV:
    //        return analyze(node->left) / analyze(node->right);
    //    }
    //}
} tree;

class IRGenerator 
{
public:
    ofstream out;
    ifstream in;
    static int labelCount;
    IRGenerator(const string& filename, const string& filename2) : out(filename) 
    {
        in.open(filename2);
    }

    void emit(const string& instruction) {
        out << instruction << endl;
    }

    void generate(const Node* node) {
        switch (node->type) 
        {
        case NodeType::VALUE:
        {
            auto valueNode = (ValueNode*)node;
            emit("PUSH " + to_string(valueNode->value));
            break;
        }
        case NodeType::IDENTIFIER:
        {
            auto identifierNode = (IdentifierNode*)node;
            emit("LOAD " + identifierNode->name);
            break;
        }
        case NodeType::BINARY_OP:
        {
            auto binaryOpNode = (BinaryOpNode*)node;
            generate(binaryOpNode->left);
            generate(binaryOpNode->right);
            switch (binaryOpNode->operation)
            {
            case Operation::ADD: emit("ADD"); break;
            case Operation::SUB: emit("SUB"); break;
            case Operation::MUL: emit("MUL"); break;
            case Operation::DIV: emit("DIV"); break;
            case Operation::LESS_THAN: emit("LT"); break;
            case Operation::GREATER_THAN: emit("GT"); break;
            }
            break;
        }
        case NodeType::ASSIGN:
        {
            auto assignNode = (AssignNode*)node;
            generate(assignNode->value);
            emit("STORE " + assignNode->target->name);
            break;
        }
        case NodeType::IF:
        {
            auto ifNode = (IfNode*)node;
            generate(ifNode->condition);
            generate(ifNode->thenBranch);
            if (ifNode->thenBranch)
                generate(ifNode->elseBranch);
            emit(";");
            break;
        }
        }
    }
};

int IRGenerator::labelCount = 0;

int main(int argc, char* argv[])
{
    tree.root = new AssignNode();
    ((AssignNode*)(tree.root))->target = new IdentifierNode("a");
    ((AssignNode*)(tree.root))->value = new BinaryOpNode(Operation::ADD);
    ((BinaryOpNode*)(((AssignNode*)(tree.root))->value))->left = new BinaryOpNode(Operation::GREATER_THAN, new IdentifierNode("b"), new IdentifierNode("c"));
    ((BinaryOpNode*)(((AssignNode*)(tree.root))->value))->right = new IdentifierNode("d");
    IRGenerator gen(argv[2], argv[1]);

    string str;
    string s;
    while (gen.in >> s)
    {
        str += s;
    }

    cout << str;

    gen.generate(tree.root);
}