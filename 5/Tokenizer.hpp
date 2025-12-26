#pragma once
#include <string>
#include <iostream>
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
			PRINT, PRINTLN, PRINTCHAR,
			VAR, GLOBAL,
			ASSIGN,
			EQUAL, GREATER, LESS, GREATEREQUAL, LESSEQUAL,
			AND, OR, NOT,
			PLUS, MINUS, 
			STAR, SLASH, MOD, 
			PLUSPLUS, MINUSMINUS, EQUALEQUAL, NOTEQUAL,
			LPAREN, RPAREN, LCURLY, RCURLY, 
			NUM, IDENT, TRUE, FALSE,
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
		
		const string& print(ostream& out) const
		{
			switch (type)
			{
			case Type::PLUS: return "PLUS";
			case Type::MINUS: return "MINUS";
			case Type::STAR: return "STAR";
			case Type::SLASH: return "SLASH";
			case Type::LPAREN: return "LPAREN";
			case Type::RPAREN: return "RPAREN";
			case Type::NUM: return "NUMBER";
			case Type::IDENT: return "IDENTIFIER";
			case Type::EQUAL: return "EQUAL";
			case Type::END: return "END";
			case Type::IF: return "IF";
			case Type::LCURLY: return "LCURLY";
			case Type::RCURLY: return "RCURLY";
			case Type::SEMICOLON: return "SEMICOLON";
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
			else if (isalpha(inputString[i]))
			{
				string textString;
				while (isalpha(inputString[i]) || isdigit(inputString[i]) || inputString[i] == '_')
				{
					textString += inputString[i];
					i++;
				}
				if (textString.compare("print") == 0)			tokenList.push(Token(Token::Type::PRINT));
				else if (textString.compare("println") == 0)	tokenList.push(Token(Token::Type::PRINTLN));
				else if (textString.compare("printchar") == 0)	tokenList.push(Token(Token::Type::PRINTCHAR));
				else if (textString.compare("var") == 0)		tokenList.push(Token(Token::Type::VAR));
				else if (textString.compare("global") == 0)		tokenList.push(Token(Token::Type::GLOBAL));
				else if (textString.compare("true") == 0)		tokenList.push(Token(Token::Type::TRUE));
				else if (textString.compare("false") == 0)		tokenList.push(Token(Token::Type::FALSE));
				else if (textString.compare("if") == 0)			tokenList.push(Token(Token::Type::IF, 0, textString));
				else if (textString.compare("else") == 0)		tokenList.push(Token(Token::Type::ELSE, 0, textString));
				else if (textString.compare("while") == 0)		tokenList.push(Token(Token::Type::WHILE, 0, textString));
				else if (textString.compare("for") == 0)		tokenList.push(Token(Token::Type::FOR, 0, textString));
				else if (textString.compare("break") == 0)		tokenList.push(Token(Token::Type::BREAK, 0, textString));
				else if (textString.compare("and") == 0)		tokenList.push(Token(Token::Type::AND));
				else if (textString.compare("or") == 0)			tokenList.push(Token(Token::Type::OR));
				else if (textString.compare("not") == 0)		tokenList.push(Token(Token::Type::NOT));
				else											tokenList.push(Token(Token::Type::IDENT, 0, textString));
			}
			else 
			{
				bool canLookAhead = (i + 1) < tokenList.size();
				switch (inputString[i])
				{
				case '+': 
					if (inputString[i + 1] != '+') { tokenList.push(Token(Token::Type::PLUS)); i++; }
					else { tokenList.push(Token(Token::Type::PLUSPLUS)); i += 2; }
					break;
				case '-': 					
					if (inputString[i + 1] != '-') { tokenList.push(Token(Token::Type::MINUS)); i++; }
					else { tokenList.push(Token(Token::Type::MINUSMINUS)); i += 2; }
					break;
				case '*': tokenList.push(Token(Token::Type::STAR));		i++; break;
				case '/': tokenList.push(Token(Token::Type::SLASH));	i++; break;
				case '%': tokenList.push(Token(Token::Type::MOD));		i++; break;
				case '(': tokenList.push(Token(Token::Type::LPAREN));	i++; break;
				case ')': tokenList.push(Token(Token::Type::RPAREN));	i++; break;
				case '{': tokenList.push(Token(Token::Type::LCURLY));	i++; break;
				case '}': tokenList.push(Token(Token::Type::RCURLY));	i++; break;
				case '=': 
					if (inputString[i + 1] != '=') { tokenList.push(Token(Token::Type::EQUAL)); i++; }
					else { tokenList.push(Token(Token::Type::EQUALEQUAL)); i += 2; }
					break;
				case '>':
					if (inputString[i + 1] != '=') { tokenList.push(Token(Token::Type::GREATER)); i++; }
					else { tokenList.push(Token(Token::Type::GREATEREQUAL)); i += 2; }
					break;
				case '<':
					if (inputString[i + 1] != '=') { tokenList.push(Token(Token::Type::LESS)); i++; }
					else { tokenList.push(Token(Token::Type::LESSEQUAL)); i += 2; }
					break;
				case '!': 
					if (inputString[i + 1] != '=') { tokenList.push(Token(Token::Type::NOT)); i++; }
					else { tokenList.push(Token(Token::Type::NOTEQUAL)); i += 2; }
					break;
				case ';': tokenList.push(Token(Token::Type::SEMICOLON));	i++; break;
				case ',': tokenList.push(Token(Token::Type::COMMA));		i++; break;
				}
			}
		}
		tokenList.push(Token(Token::Type::END));
	}
};