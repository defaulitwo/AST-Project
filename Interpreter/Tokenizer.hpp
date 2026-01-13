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
			VAR, GLOBAL, INPUT, ARRAY,
			EQUAL, GREATER, LESS, GREATEREQUAL, LESSEQUAL,
			AND, OR, NOT,
			PLUS, MINUS, 
			STAR, SLASH, MOD, 
			INCREMENT, DECREMENT, EQUALITY, NOTEQUALITY,
			AMPERSAND, TILDA, PIPE, CARET,
			BITSHIFTLEFT, BITSHIFTRIGHT,
			LPAREN, RPAREN, LCURLY, RCURLY, LSQUAREBRACKET, RSQUAREBRACKET,
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

		Token(Type t, long long v = 0, const string& s = "", char c = 0) 
			: type(t), value(v), text(s), character(c) { }
		
		static string toString(Token::Type type)
		{
			switch (type)
			{
			case Type::VAR:				return "var";
			case Type::GLOBAL:			return "global";
			case Type::INPUT:			return "input";
			case Type::ARRAY:			return "array";
			case Type::EQUAL:			return "=";
			case Type::GREATER:			return ">";
			case Type::LESS:			return "<";
			case Type::GREATEREQUAL:	return ">=";
			case Type::LESSEQUAL:		return "<=";
			case Type::AND:				return "&&";
			case Type::OR:				return "||";
			case Type::NOT:				return "!";
			case Type::PLUS:			return "+";
			case Type::MINUS:			return "-";
			case Type::STAR:			return "*";
			case Type::SLASH:			return "/";
			case Type::MOD:				return "%";
			case Type::INCREMENT:		return "++";
			case Type::DECREMENT:		return "--";
			case Type::EQUALITY:		return "==";
			case Type::NOTEQUALITY:		return "!=";
			case Type::AMPERSAND:		return "&";
			case Type::TILDA:			return "~";
			case Type::PIPE:			return "|";
			case Type::CARET:			return "^";
			case Type::BITSHIFTLEFT:	return "<<";
			case Type::BITSHIFTRIGHT:	return ">>";
			case Type::LPAREN:			return "(";
			case Type::RPAREN:			return ")";
			case Type::LCURLY:			return "{";
			case Type::RCURLY:			return "}";
			case Type::LSQUAREBRACKET:	return "[";
			case Type::RSQUAREBRACKET:	return "]";
			case Type::NUM:				return "NUMBER";
			case Type::CHAR:			return "CHAR";
			case Type::IDENT:			return "IDENTIFIER";
			case Type::TRUE:			return "true";
			case Type::FALSE:			return "false";
			case Type::SEMICOLON:		return ";";
			case Type::COMMA:			return ",";
			case Type::IF:				return "if";
			case Type::ELSE:			return "else";
			case Type::WHILE:			return "while";
			case Type::FOR:				return "for";
			case Type::REPEAT:			return "repeat";
			case Type::DO:				return "do";
			case Type::PRINT:			return "print";
			case Type::PRINTLN:			return "println";
			case Type::PRINTCHAR:		return "printchar";
			case Type::RANDOM:			return "random";
			case Type::BREAK:			return "break";
			case Type::CONTINUE:		return "continue";
			case Type::RETURN:			return "return";
			case Type::ERROR:			return "ERROR";
			case Type::FUNC:			return "func";
			case Type::END:				return "END";
			}
			return "UNDEFINED";
		}
	};

	typedef Token::Type TokenType;

	DynamicList<Token> tokenList;
	int lineNumber;
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

	Tokenizer(const string& inputString) : lineNumber(0)
	{
		int i = 0;
		while (i < inputString.size())
		{
			if (inputString[i] == '\n') lineNumber++;
			if (isspace(inputString[i])) { i++; continue; } // ignoring whitespaces
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
				else if (textString.compare("array") == 0)		pushToken(Token(Token::Type::ARRAY));
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
				else if (textString.compare("continue") == 0)	pushToken(Token(Token::Type::CONTINUE));
				else if (textString.compare("func") == 0)		pushToken(Token(Token::Type::FUNC));
				// english replacements for SOME symbols
				else if (textString.compare("and") == 0)		pushToken(Token(Token::Type::AND));
				else if (textString.compare("or") == 0)			pushToken(Token(Token::Type::OR));
				else if (textString.compare("not") == 0)		pushToken(Token(Token::Type::NOT));
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
				case '[': pushToken(Token(Token::Type::LSQUAREBRACKET));	i++; continue;
				case ']': pushToken(Token(Token::Type::RSQUAREBRACKET));	i++; continue;
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
				case '^': pushToken(Token(Token::Type::CARET));		i++; continue;
				case '~': pushToken(Token(Token::Type::TILDA));		i++; continue;
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
					else throw runtime_error("Token error: expected \"'\", at line " + to_string(lineNumber));
					continue;
				default: // unknown symbol, ignore it
					throw runtime_error("Token error: Undefined symbol(s) found, at line " + to_string(lineNumber));
					break;
				}
			}
		}
		pushToken(Token(Token::Type::END));
	}
};