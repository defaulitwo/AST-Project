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
		enum class Type { 
			PRINT, PRINTLN,
			VAR, GLOBAL,
			TRUE, FALSE,
			ASSIGN,
			GREATER, LESS,
			AND, OR, NOT,
			PLUS, MINUS, 
			STAR, SLASH, MOD, 
			PLUSPLUS, MINUSMINUS, EQUALEQUAL, NOTEQUAL,
			LPAREN, RPAREN, LCURLY, RCURLY, 
			NUM, IDENT, EQUAL, 
			SEMICOLON, COMMA,
			IF, ELSE, WHILE, FOR,
			BREAK, CONTINUE,
			ERROR, END,
		};

		Type type;
		int value;
		string text;

		Token() { }; // default constructor, allows creating an uninitialized array of Token

		Token(Type t, int v = 0, const string& s = "") : type(t), value(v), text(s) { }
		
		void print(ostream& out) const
		{
			switch (type)
			{
			case Type::PLUS: out << "PLUS"; break;
			case Type::MINUS: out << "MINUS"; break;
			case Type::STAR: out << "STAR"; break;
			case Type::SLASH: out << "SLASH"; break;
			case Type::LPAREN: out << "LPAREN"; break;
			case Type::RPAREN: out << "RPAREN"; break;
			case Type::NUM: out << "NUMBER(" << value << ")"; break;
			case Type::IDENT: out << "IDENTIFIER(" << text << ")"; break;
			case Type::EQUAL: out << "EQUAL"; break;
			case Type::END: out << "END"; break;
			case Type::IF: out << "IF"; break;
			case Type::LCURLY: out << "LCURLY"; break;
			case Type::RCURLY: out << "RCURLY"; break;
			case Type::SEMICOLON: out << "SEMICOLON"; break;
			}
		}
	};

	typedef Token::Type TokenType;

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
				tokenList.push(Token(Token::Type::NUM, stoi(numString)));
			}
			else if (isalpha(inputString[i])) // this if statement body is to be re-done if expanded to a full compiler
			{
				string textString;
				while (isalpha(inputString[i]) || isdigit(inputString[i]) || inputString[i] == '_')
				{
					textString += inputString[i];
					i++;
				}
				if (textString.compare("print") == 0)		tokenList.push(Token(Token::Type::PRINT));
				else if (textString.compare("println") == 0)tokenList.push(Token(Token::Type::PRINTLN));
				else if (textString.compare("var") == 0)	tokenList.push(Token(Token::Type::VAR));
				else if (textString.compare("global") == 0)	tokenList.push(Token(Token::Type::GLOBAL));
				else if (textString.compare("true") == 0)	tokenList.push(Token(Token::Type::TRUE));
				else if (textString.compare("false") == 0)	tokenList.push(Token(Token::Type::FALSE));
				else if (textString.compare("if") == 0)		tokenList.push(Token(Token::Type::IF, 0, textString));
				else if (textString.compare("else") == 0)	tokenList.push(Token(Token::Type::ELSE, 0, textString));
				else if (textString.compare("while") == 0)	tokenList.push(Token(Token::Type::WHILE, 0, textString));
				else if (textString.compare("for") == 0)	tokenList.push(Token(Token::Type::FOR, 0, textString));
				else if (textString.compare("break") == 0)	tokenList.push(Token(Token::Type::BREAK, 0, textString));
				else if (textString.compare("and") == 0)	tokenList.push(Token(Token::Type::AND));
				else if (textString.compare("or") == 0)		tokenList.push(Token(Token::Type::OR));
				else if (textString.compare("not") == 0)	tokenList.push(Token(Token::Type::NOT));
				else										tokenList.push(Token(Token::Type::IDENT, 0, textString));
			}
			else
			{
				switch (inputString[i])
				{
				case '+': tokenList.push(Token(Token::Type::PLUS)); break;
				case '-': tokenList.push(Token(Token::Type::MINUS)); break;
				case '*': tokenList.push(Token(Token::Type::STAR)); break;
				case '/': tokenList.push(Token(Token::Type::SLASH)); break;
				case '%': tokenList.push(Token(Token::Type::MOD)); break;
				case '(': tokenList.push(Token(Token::Type::LPAREN)); break;
				case ')': tokenList.push(Token(Token::Type::RPAREN)); break;
				case '{': tokenList.push(Token(Token::Type::LCURLY)); break;
				case '}': tokenList.push(Token(Token::Type::RCURLY)); break;
				case '=': tokenList.push(Token(Token::Type::EQUAL)); break;
				case '>': tokenList.push(Token(Token::Type::GREATER)); break;
				case '<': tokenList.push(Token(Token::Type::LESS)); break;
				case '!': tokenList.push(Token(Token::Type::NOT)); break;
				case ';': tokenList.push(Token(Token::Type::SEMICOLON)); break;
				case ',': tokenList.push(Token(Token::Type::COMMA)); break;
				}
				i++;
			}
		}
		tokenList.push(Token(Token::Type::END));
	}
};