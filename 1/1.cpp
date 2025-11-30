// 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using std::cout;
using std::cin;
using std::getline;
using std::endl;

using std::string;
using std::vector;

using std::stoi;
using std::stoll;
using std::stoull;

using std::isdigit;
using std::isspace;
using std::isalpha;
using std::isalnum;

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

struct Token
{
    TokenType type;
    string text;
    long long value;
};

std::ostream& operator<<(std::ostream& out, const Token token)
{
    out << token.text;
    return out;
}

int main()
{
    string input;
    vector<Token> tokenList;
    getline(cin, input);
    
    for (int i = 0; i < input.length(); i++)
    {
        if (isspace(input[i]))
        {
            continue;
        }
        else if (isdigit(input[i]))
        {
            string numberString;
            for (int j = i; j < input.length() && isdigit(input[j]); j++, i = j-1)
            {
                numberString += input[j];
            }
            tokenList.push_back({ TokenType::NUMBER,("NUMBER(" + numberString + ")"),stoi(numberString)});
        }
        else if (input[i] == '+')
        {
            tokenList.push_back({ TokenType::PLUS,"PLUS",0 });
        }
        else if (input[i] == '-')
        {
            tokenList.push_back({ TokenType::MINUS,"MINUS",0 });
        }
        else if (input[i] == '*')
        {
            tokenList.push_back({ TokenType::STAR,"STAR",0 });
        }
        else if (input[i] == '/')
        {
            tokenList.push_back({ TokenType::SLASH,"SLASH",0 });
        }
        else if (input[i] == '(')
        {
            tokenList.push_back({ TokenType::LPAREN,"LPAREN",0 });
        }
        else if (input[i] == ')')
        {
            tokenList.push_back({ TokenType::RPAREN,"RPAREN",0 });
        }
        else
        {
            tokenList.push_back({ TokenType::ERROR, "ERROR",0 });
        }
    }

    for (Token token : tokenList)
    {
        cout << token << " ";
    }
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
