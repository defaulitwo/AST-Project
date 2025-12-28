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
			VAR, GLOBAL, INPUT,
			EQUAL, GREATER, LESS, GREATEREQUAL, LESSEQUAL,
			AND, OR, NOT,
			PLUS, MINUS, 
			STAR, SLASH, MOD, 
			PLUSPLUS, MINUSMINUS, EQUALEQUAL, NOTEQUAL,
			LPAREN, RPAREN, LCURLY, RCURLY,
			NUM, CHAR, IDENT, TRUE, FALSE,
			SEMICOLON, COMMA,
			IF, ELSE, WHILE, FOR, DO,
			PRINT, PRINTLN, PRINTCHAR,
			RANDOM,
			BREAK, CONTINUE,
			FUNC,
			ERROR, END,
		};

		Type type;
		int value;
		string text;
		char character;
		int lineNumber;
	
		Token() { }; // default constructor, allows creating an uninitialized array of Token

		Token(Type t, int v = 0, const string& s = "", char c = ' ') : type(t), value(v), text(s), character(c) {}
		
		static string toString(Token::Type type)
		{
			switch (type)
			{
			case Type::VAR:				return "VAR";
			case Type::GLOBAL:			return "GLOBAL";
			case Type::INPUT:			return "INPUT";
			case Type::EQUAL:			return "EQUAL";
			case Type::GREATER:			return "GREATER";
			case Type::LESS:			return "LESS";
			case Type::GREATEREQUAL:	return "GREATEREQUAL";
			case Type::LESSEQUAL:		return "LESSEQUAL";
			case Type::AND:				return "AND";
			case Type::OR:				return "OR";
			case Type::NOT:				return "NOT";
			case Type::PLUS:			return "PLUS";
			case Type::MINUS:			return "MINUS";
			case Type::STAR:			return "STAR";
			case Type::SLASH:			return "SLASH";
			case Type::MOD:				return "MOD";
			case Type::PLUSPLUS:		return "PLUSPLUS";
			case Type::MINUSMINUS:		return "MINUSMINUS";
			case Type::EQUALEQUAL:		return "EQUALEQUAL";
			case Type::NOTEQUAL:		return "NOTEQUAL";
			case Type::LPAREN:			return "LPAREN";
			case Type::RPAREN:			return "RPAREN";
			case Type::LCURLY:			return "LCURLY";
			case Type::RCURLY:			return "RCURLY";
			case Type::NUM:				return "NUMBER";
			case Type::CHAR:			return "CHAR";
			case Type::IDENT:			return "IDENTIFIER";
			case Type::TRUE:			return "TRUE";
			case Type::FALSE:			return "FALSE";
			case Type::SEMICOLON:		return "SEMICOLON";
			case Type::COMMA:			return "COMMA";
			case Type::IF:				return "IF";
			case Type::ELSE:			return "ELSE";
			case Type::WHILE:			return "WHILE";
			case Type::FOR:				return "FOR";
			case Type::PRINT:			return "PRINT";
			case Type::PRINTLN:			return "PRINTLN";
			case Type::PRINTCHAR:		return "PRINTCHAR";
			case Type::RANDOM:			return "RANDOM";
			case Type::BREAK:			return "BREAK";
			case Type::CONTINUE:		return "CONTINUE";
			case Type::ERROR:			return "ERROR";
			case Type::END:				return "END";
			}
			return "UNDEFINED";
		}
	};

	typedef Token::Type TokenType;

	DynamicList<Token> tokenList;

	void printTokensTo(ostream& out) const
	{
		for (Token token : tokenList)
		{
			out << Token::toString(token.type) << " ";
		}
		cout << endl;
	}

	Tokenizer(const string& inputString) 
	{
		int i = 0;
		int lineNumber = 0;
		while (i < inputString.size())
		{
			if (inputString[i] == '\n') lineNumber++;
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
				else if (textString.compare("random") == 0)		tokenList.push(Token(Token::Type::RANDOM));
				else if (textString.compare("var") == 0)		tokenList.push(Token(Token::Type::VAR));
				else if (textString.compare("global") == 0)		tokenList.push(Token(Token::Type::GLOBAL));
				else if (textString.compare("input") == 0)		tokenList.push(Token(Token::Type::INPUT));
				else if (textString.compare("true") == 0)		tokenList.push(Token(Token::Type::TRUE));
				else if (textString.compare("false") == 0)		tokenList.push(Token(Token::Type::FALSE));
				else if (textString.compare("if") == 0)			tokenList.push(Token(Token::Type::IF, 0, textString));
				else if (textString.compare("else") == 0)		tokenList.push(Token(Token::Type::ELSE, 0, textString));
				else if (textString.compare("while") == 0)		tokenList.push(Token(Token::Type::WHILE, 0, textString));
				else if (textString.compare("for") == 0)		tokenList.push(Token(Token::Type::FOR, 0, textString));
				else if (textString.compare("do") == 0)			tokenList.push(Token(Token::Type::DO, 0, textString));
				else if (textString.compare("break") == 0)		tokenList.push(Token(Token::Type::BREAK, 0, textString));
				else if (textString.compare("and") == 0)		tokenList.push(Token(Token::Type::AND));
				else if (textString.compare("or") == 0)			tokenList.push(Token(Token::Type::OR));
				else if (textString.compare("not") == 0)		tokenList.push(Token(Token::Type::NOT));
				else											tokenList.push(Token(Token::Type::IDENT, 0, textString));
			}
			else 
			{
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
				case '&': 
					if (inputString[i + 1] != '&') { /*...*/ i++; }
					else { tokenList.push(Token(Token::Type::AND)); i += 2; }
					break;
				case '|': 
					if (inputString[i + 1] != '|') { /*...*/ i++; }
					else { tokenList.push(Token(Token::Type::OR)); i += 2; }
					break;
				case ';': tokenList.push(Token(Token::Type::SEMICOLON));	i++; break;
				case ',': tokenList.push(Token(Token::Type::COMMA));		i++; break;
				case '#':
					while (i < inputString.size() && inputString[i] != '\n') { i++; } i++;
					break;
				case '\'':
					i++;
					tokenList.push(Token(Token::Type::CHAR, 0, "", inputString[i]));
					i++;
					if (inputString[i] == '\'') i++;
					break;
				}
			}
		}
		tokenList.push(Token(Token::Type::END));
	}
};