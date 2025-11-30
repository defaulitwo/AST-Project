#include <iostream>
#include <string>
#include <cctype>

using namespace std;

enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LPAREN,
    RPAREN,
    END,
    ERROR
};

//class Token
//{
//public:
//    TokenType type;
//    string text;
//    long long value;
//};
//
//vector<Token> tokenizeString(string input)
//{
//    vector<Token> tokenList;
//    for (int i = 0; i < input.length(); i++)
//    {
//        if (isspace(input[i]))
//        {
//            continue;
//        }
//        else if (isdigit(input[i]))
//        {
//            string numberString;
//            for (int j = i; j < input.length() && isdigit(input[j]); j++, i = j - 1)
//            {
//                numberString += input[j];
//            }
//            tokenList.push_back({ TokenType::NUMBER,("NUMBER(" + numberString + ")"),stoi(numberString) });
//        }
//        else if (input[i] == '+')
//        {
//            tokenList.push_back({ TokenType::PLUS,"PLUS",0 });
//        }
//        else if (input[i] == '-')
//        {
//            tokenList.push_back({ TokenType::MINUS,"MINUS",0 });
//        }
//        else if (input[i] == '*')
//        {
//            tokenList.push_back({ TokenType::STAR,"STAR",0 });
//        }
//        else if (input[i] == '/')
//        {
//            tokenList.push_back({ TokenType::SLASH,"SLASH",0 });
//        }
//        else if (input[i] == '(')
//        {
//            tokenList.push_back({ TokenType::LPAREN,"LPAREN",0 });
//        }
//        else if (input[i] == ')')
//        {
//            tokenList.push_back({ TokenType::RPAREN,"RPAREN",0 });
//        }
//        else
//        {
//            tokenList.push_back({ TokenType::ERROR, "ERROR",0 });
//        }
//    }
//
//    tokenList.push_back({ TokenType::END,"END",0});
//
//    return tokenList;
//}
//
//std::ostream& operator<<(std::ostream& out, const Token token)
//{
//    out << token.text;
//    return out;
//}

//-----

enum class NodeType
{
    ADD,
    SUB,
    MUL,
    DIV,
    VALUE,
};

class Node
{
public:
    NodeType type;
    double value;
    Node* left;
    Node* right;

    Node(NodeType t, double v = 0, Node* l = nullptr, Node* r = nullptr) : type(t), value(v), left(l), right(r) { }
};

class AST
{
public:
    Node* root;

    double analyzeTree() const
    {
        return evaluate(root);
    }

    void printExpression()
    {
        printNode(root);
        cout << endl;
    }

private:
    static double evaluate(const Node* node)
    {
        switch (node->type)
        {
        case NodeType::VALUE:
            return node->value;
        case NodeType::ADD:
            return evaluate(node->left) + evaluate(node->right);
        case NodeType::SUB:
            return evaluate(node->left) - evaluate(node->right);
        case NodeType::MUL:
            return evaluate(node->left) * evaluate(node->right);
        case NodeType::DIV:
            return evaluate(node->left) / evaluate(node->right);
        }
    }

    static void printNode(const Node* node)
    {
        if (node->type == NodeType::VALUE)
        {
            cout << node->value;
            return;
        }
        cout << "(";
        printNode(node->left);
        switch (node->type)
        {
        case NodeType::ADD: cout << "+"; break;
        case NodeType::SUB: cout << "-"; break;
        case NodeType::MUL: cout << "*"; break;
        case NodeType::DIV: cout << "/"; break;
        }
        printNode(node->right);
        cout << ")";
    }
};

//-----

//class Parser
//{
//public:
//    static vector<Token> tokenList;
//    static int currentTokenIndex;
//
//    Token peek()
//    {
//        return tokenList[currentTokenIndex];
//    }
//    
//    Token previous()
//    {
//        return tokenList[currentTokenIndex - 1];
//    }
//
//    static bool isAtEnd()
//    {
//        if (currentTokenIndex == tokenList.size() - 1)
//        {
//            return true;
//        }
//        return false;
//    }
//
//    Token advance() 
//    {
//        if (!isAtEnd()) currentTokenIndex++;
//        return tokenList[currentTokenIndex - 1]; // return the token we just consumed
//    }
//
//
//    static Node* buildAST()
//    {
//        for (Token token : tokenList)
//        {
//
//        }
//    }
//
//    static Node* parseExpression()
//    {
//
//    }
//
//    static Node* parseTerm()
//    {
//
//    }
//
//    static Node* parseFactor()
//    {
//
//    }
//
//};
//
//int Parser::currentTokenIndex = 0;

//-----

int main()
{
    //vector<Token> tokenList = tokenizeString("5+2*(9-3)");
    //Parser::tokenList = tokenList;

    AST tree;
    tree.root = new Node(NodeType::MUL);
    tree.root->left = new Node(NodeType::VALUE, 3.0);
    tree.root->right = new Node(NodeType::ADD);
    tree.root->right->left = new Node(NodeType::VALUE, 10.0);
    tree.root->right->right = new Node(NodeType::VALUE, 20.0);

    tree.printExpression();
}