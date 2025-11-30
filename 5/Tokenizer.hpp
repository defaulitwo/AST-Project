#pragma once
#include <string>
//#include <vector>
#include "DynamicList.hpp"
#include <cctype>

using namespace std;

class Tokenizer
{
public:
	
	class Token
	{
	public:
		enum class TokenType { PLUS, MINUS, STAR, SLASH, LPAREN, RPAREN, NUM, IDENT, EQUAL, END, };

		TokenType type;
		double value;
		string text;

		Token() { }; // default constructor, allows creating an uninitialized array of Token

		Token(TokenType t, double v = 0, const string& s = "") : type(t), value(v), text(s) { }
		
		void print(ostream& out) const
		{
			switch (type)
			{
			case TokenType::PLUS: out << "PLUS"; break;
			case TokenType::MINUS: out << "MINUS"; break;
			case TokenType::STAR: out << "STAR"; break;
			case TokenType::SLASH: out << "SLASH"; break;
			case TokenType::LPAREN: out << "LPAREN"; break;
			case TokenType::RPAREN: out << "RPAREN"; break;
			case TokenType::NUM: out << "NUMBER(" << value << ")"; break;
			case TokenType::IDENT: out << "IDENTIFIER(" << text << ")"; break;
			case TokenType::EQUAL: out << "EQUAL"; break;
			case TokenType::END: out << "END"; break;
			}
		}
	};

	typedef Token::TokenType TokenType;

	//vector<Token> tokenList;

	DynamicList<Token> tokenList;

	void printTokensTo(ostream& out) const
	{
		for (Token token : tokenList)
		{
			token.print(out);
			out << " ";
		}
		cout << endl;
	}

	Tokenizer(const string& inputString) 
	{
		int i = 0;
		while (i < inputString.size())
		{
			if (isspace(inputString[i])) { i++; continue; }
			if (isdigit(inputString[i]))
			{
				string numString;
				while (isdigit(inputString[i]) || inputString[i] == '.')
				{
					numString += inputString[i];
					i++;
				}
				tokenList.push(Token(TokenType::NUM, stod(numString)));
			}
			else if (isalpha(inputString[i])) // this if statement body is to be re-done if expanded to a full compiler
			{
				string textString;
				while (isalpha(inputString[i]))
				{
					textString += inputString[i];
					i++;
				}
				tokenList.push(Token(TokenType::IDENT, 0, textString));
			}
			else
			{
				switch (inputString[i])
				{
				case '+': tokenList.push(Token(TokenType::PLUS)); break;
				case '-': tokenList.push(Token(TokenType::MINUS)); break;
				case '*': tokenList.push(Token(TokenType::STAR)); break;
				case '/': tokenList.push(Token(TokenType::SLASH)); break;
				case '(': tokenList.push(Token(TokenType::LPAREN)); break;
				case ')': tokenList.push(Token(TokenType::RPAREN)); break;
				case '=': tokenList.push(Token(TokenType::EQUAL)); break;
				}
				i++;
			}
		}
		tokenList.push(Token(TokenType::END));

		// For loop to handle implicit *
		// puts star where multiplication is implied
		// ex: 2(3-5) = 2*(3-5) and (2+5)(3-8) = (2+5)*(3-8) 
		for (int i = 0; i < tokenList.size() - 1; i++)
		{
			if ((tokenList[i].type == TokenType::NUM && tokenList[i + 1].type == TokenType::LPAREN)
				|| (tokenList[i].type == TokenType::RPAREN && tokenList[i + 1].type == TokenType::LPAREN))
			{
				//tokenList.insert(tokenList.begin() + (i + 1), Token(TokenType::STAR));
				tokenList.insert(Token(TokenType::STAR), i + 1);
				i++;
			}
		}
	}
};