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
		enum class Type 
		{ 
			VAR, GLOBAL, INPUT,
			EQUAL, GREATER, LESS, GREATEREQUAL, LESSEQUAL,
			AND, OR, NOT,
			PLUS, MINUS, 
			STAR, SLASH, MOD, 
			INCREMENT, DECREMENT, EQUALITY, NOTEQUALITY,
			AMPERSAND, TILDA, PIPE, CARET,
			BITSHIFTLEFT, BITSHIFTRIGHT,
			LPAREN, RPAREN, LCURLY, RCURLY,
			NUM, CHAR, IDENT, TRUE, FALSE,
			SEMICOLON, COMMA,
			IF, ELSE, WHILE, FOR, REPEAT, DO,
			PRINT, PRINTLN, PRINTCHAR,
			RANDOM,
			BREAK, CONTINUE, RETURN,
			FUNC,
			ERROR, END,
		};

		Type type;
		long long value;
		string text;
		char character;
		int lineNumber;
	
		Token() { }; // default constructor, allows creating an uninitialized array of Token

		Token(Type t, long long v = 0, const string& s = "", char c = 0) : type(t), value(v), text(s), character(c) {}
		
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
			case Type::INCREMENT:		return "INCREMENT";
			case Type::DECREMENT:		return "DECREMENT";
			case Type::EQUALITY:		return "EQUALITY";
			case Type::NOTEQUALITY:		return "NOTEQUALITY";
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
			case Type::REPEAT:			return "REPEAT";
			case Type::DO:				return "DO";
			case Type::PRINT:			return "PRINT";
			case Type::PRINTLN:			return "PRINTLN";
			case Type::PRINTCHAR:		return "PRINTCHAR";
			case Type::RANDOM:			return "RANDOM";
			case Type::BREAK:			return "BREAK";
			case Type::CONTINUE:		return "CONTINUE";
			case Type::RETURN:			return "RETURN";
			case Type::ERROR:			return "ERROR";
			case Type::FUNC:			return "FUNC";
			case Type::END:				return "END";
			}
			return "UNDEFINED";
		}
	};

	typedef Token::Type TokenType;

	DynamicList<Token> tokenList;
	int lineNumber = 0;
	// push new token to token list
	void pushToken(Token token)
	{
		token.lineNumber = lineNumber;
		tokenList.push(token);
	}
	// printing tokens, for debugging purposes
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
		while (i < inputString.size())
		{
			if (inputString[i] == '\n') lineNumber++;
			if (isspace(inputString[i])) { i++; continue; } // ignore whitespaces
			if (isdigit(inputString[i])) // number literal
			{
				string numString;
				while (isdigit(inputString[i]) || inputString[i] == '.')
				{
					numString += inputString[i];
					i++;
				}
				pushToken(Token(Token::Type::NUM, stoll(numString)));
			}
			else if (isalpha(inputString[i]))
			{
				string textString;
				while (isalpha(inputString[i]) || isdigit(inputString[i]) || inputString[i] == '_')
				{
					textString += inputString[i];
					i++;
				}
				// reserved keywords
				if		(textString.compare("print") == 0)		pushToken(Token(Token::Type::PRINT));
				else if (textString.compare("println") == 0)	pushToken(Token(Token::Type::PRINTLN));
				else if (textString.compare("printchar") == 0)	pushToken(Token(Token::Type::PRINTCHAR));
				else if (textString.compare("random") == 0)		pushToken(Token(Token::Type::RANDOM));
				else if (textString.compare("var") == 0)		pushToken(Token(Token::Type::VAR));
				else if (textString.compare("global") == 0)		pushToken(Token(Token::Type::GLOBAL));
				else if (textString.compare("input") == 0)		pushToken(Token(Token::Type::INPUT));
				else if (textString.compare("true") == 0)		pushToken(Token(Token::Type::TRUE));
				else if (textString.compare("false") == 0)		pushToken(Token(Token::Type::FALSE));
				else if (textString.compare("if") == 0)			pushToken(Token(Token::Type::IF));
				else if (textString.compare("else") == 0)		pushToken(Token(Token::Type::ELSE));
				else if (textString.compare("while") == 0)		pushToken(Token(Token::Type::WHILE));
				else if (textString.compare("for") == 0)		pushToken(Token(Token::Type::FOR));
				else if (textString.compare("repeat") == 0)		pushToken(Token(Token::Type::REPEAT));
				else if (textString.compare("do") == 0)			pushToken(Token(Token::Type::DO));
				else if (textString.compare("break") == 0)		pushToken(Token(Token::Type::BREAK));
				else if (textString.compare("return") == 0)		pushToken(Token(Token::Type::RETURN));
				else if (textString.compare("and") == 0)		pushToken(Token(Token::Type::AND));
				else if (textString.compare("or") == 0)			pushToken(Token(Token::Type::OR));
				else if (textString.compare("not") == 0)		pushToken(Token(Token::Type::NOT));
				else if (textString.compare("func") == 0)		pushToken(Token(Token::Type::FUNC));
				// identifier
				else pushToken(Token(Token::Type::IDENT, 0, textString));
			}
			else // symbols
			{
				switch (inputString[i])
				{
				case '+': 
					if (inputString[i+1] != '+') { pushToken(Token(Token::Type::PLUS)); i++; }
					else { pushToken(Token(Token::Type::INCREMENT)); i += 2; }
					continue;
				case '-': 					
					if (inputString[i+1] != '-') { pushToken(Token(Token::Type::MINUS)); i++; }
					else { pushToken(Token(Token::Type::DECREMENT)); i += 2; }
					continue;
				case '*': pushToken(Token(Token::Type::STAR));		i++; continue;
				case '/': pushToken(Token(Token::Type::SLASH));		i++; continue;
				case '%': pushToken(Token(Token::Type::MOD));		i++; continue;
				case '(': pushToken(Token(Token::Type::LPAREN));	i++; continue;
				case ')': pushToken(Token(Token::Type::RPAREN));	i++; continue;
				case '{': pushToken(Token(Token::Type::LCURLY));	i++; continue;
				case '}': pushToken(Token(Token::Type::RCURLY));	i++; continue;
				case '=': 
					if (inputString[i+1] != '=') { pushToken(Token(Token::Type::EQUAL)); i++; }
					else { pushToken(Token(Token::Type::EQUALITY)); i += 2; }
					continue;
				case '>':
					if (inputString[i+1] == '=') { pushToken(Token(Token::Type::GREATEREQUAL)); i += 2; }
					else if (inputString[i+1] == '>') { pushToken(Token(Token::Type::BITSHIFTRIGHT)); i += 2; }
					else { pushToken(Token(Token::Type::GREATER)); i++; }
					continue;
				case '<':
					if (inputString[i+1] == '=') { pushToken(Token(Token::Type::LESSEQUAL)); i += 2; }
					else if (inputString[i+1] == '<') { pushToken(Token(Token::Type::BITSHIFTLEFT)); i += 2; }
					else { pushToken(Token(Token::Type::LESS)); i++; }
					continue;
				case '!': 
					if (inputString[i+1] != '=') { pushToken(Token(Token::Type::NOT)); i++; }
					else { pushToken(Token(Token::Type::NOTEQUALITY)); i += 2; }
					continue;
				case '&': 
					if (inputString[i+1] != '&') { pushToken(Token(Token::Type::AMPERSAND)); i++; }
					else { pushToken(Token(Token::Type::AND)); i += 2; }
					continue;
				case '|': 
					if (inputString[i+1] != '|') { pushToken(Token(Token::Type::PIPE)); i++; }
					else { pushToken(Token(Token::Type::OR)); i += 2; }
					continue;
				case '^': pushToken(Token(Token::Type::CARET));	i++; continue;
				case '~': pushToken(Token(Token::Type::TILDA));	i++; continue;
				case ';': pushToken(Token(Token::Type::SEMICOLON));	i++; continue;
				case ',': pushToken(Token(Token::Type::COMMA));		i++; continue;
				case '#': // comment
					while (i < inputString.size() && inputString[i] != '\n') i++; 
					continue;
				case '\'': // character literal
					i++;
					pushToken(Token(Token::Type::CHAR, 0, "", inputString[i]));
					i++;
					if (inputString[i] == '\'') i++;
					continue;
				default: // unknown symbol, skip it
					i++; continue;
				}
			}
		}
		pushToken(Token(Token::Type::END));
	}
};