#pragma once
#include <string>
#include <vector>
#include <cctype>

using namespace std;

class Tokenizer
{
public:
	
	class Token
	{
	public:
		enum class TokenType { AND, OR, NOT, ARROW, LPAREN, RPAREN, VARIABLE, SEMICOLON, IF, THEN, THEREFORE, END };

		TokenType type;
		string text;

		Token() { }; // default constructor, allows creating an uninitialized array of Token

		Token(TokenType t, const string& s = "") : type(t), text(s) { }
		
		void print(ostream& out) const
		{
			switch (type)
			{
			case TokenType::AND: out << "AND"; break;
			case TokenType::OR: out << "OR"; break;
			case TokenType::NOT: out << "NOT"; break;
			case TokenType::ARROW: out << "ARROW"; break;
			case TokenType::LPAREN: out << "LPAREN"; break;
			case TokenType::RPAREN: out << "RPAREN"; break;
			case TokenType::VARIABLE: out << "VARIABLE(" + text + ")"; break;
			case TokenType::SEMICOLON: out << "SEMICOLON"; break;
			case TokenType::IF: out << "IF"; break;
			case TokenType::THEN: out << "THEN"; break;
			case TokenType::THEREFORE: out << "THEREFORE"; break;
			case TokenType::END: out << "END"; break;
			}
		}
	};

	typedef Token::TokenType TokenType;

	vector<Token> tokenList;
	vector<string> variableList;
	bool** truthTable;

	void printTokensTo(ostream& out) const
	{
		for (Token token : tokenList)
		{
			token.print(out);
			out << " ";
		}
		out << endl;
	}

	Tokenizer(const string& inputString) 
	{
		int i = 0;
		while (i < inputString.size())
		{
			if (isspace(inputString[i])) { i++; continue; }
			if (isalpha(inputString[i]))
			{
				string textString;
				while (isalpha(inputString[i]))
				{
					textString += inputString[i];
					i++;
				}
				if (textString.compare("and") == 0) tokenList.push_back(Token(TokenType::AND));
				else if (textString.compare("or") == 0) tokenList.push_back(Token(TokenType::OR));
				else if (textString.compare("not") == 0) tokenList.push_back(Token(TokenType::NOT));
				else if (textString.compare("if") == 0) tokenList.push_back(Token(TokenType::IF));
				else if (textString.compare("then") == 0) tokenList.push_back(Token(TokenType::THEN));
				else if (textString.compare("therefore") == 0) tokenList.push_back(Token(TokenType::THEREFORE));
				else
				{
					tokenList.push_back(Token(TokenType::VARIABLE, textString));
					bool isUnique = true;
					for (string s : variableList)
					{
						if (s.compare(textString) == 0) isUnique = false;
					}
					if (isUnique) variableList.push_back(textString);
				}
			}
			else
			{
				switch (inputString[i])
				{
				case '^': tokenList.push_back(Token(TokenType::AND)); break;
				case 'v': tokenList.push_back(Token(TokenType::OR)); break;
				case '~': tokenList.push_back(Token(TokenType::NOT)); break;
				case '(': tokenList.push_back(Token(TokenType::LPAREN)); break;
				case ')': tokenList.push_back(Token(TokenType::RPAREN)); break;
				case ';': tokenList.push_back(Token(TokenType::SEMICOLON)); break;
				case '-': 
					if (inputString[i + 1] == '>')
					{
						tokenList.push_back(Token(TokenType::ARROW));
						i++;
					}
					break;
				}
				i++;
			}
		}
		tokenList.push_back(Token(TokenType::END));

		truthTable = new bool* [pow(2, variableList.size())];
		
		for (int i = 0; i < pow(2, variableList.size()); i++)
			truthTable[i] = new bool[variableList.size()];

		for (int i = 0; i < pow(2, variableList.size()); i++)
		{
			for (int j = 0; j < variableList.size(); j++)
				truthTable[i][j] = (bool)(i >> j & 1);
		}
	}

	~Tokenizer()
	{
		for (int i = 0; i < pow(2, variableList.size()); i++) // dealloacte truth table
		{
			delete[] truthTable[i];
		}
		delete[] truthTable;
	}

	int getVariableIndex(string s) const
	{
		for (int i = 0; i < variableList.size(); i++)
		{
			if (variableList[i].compare(s) == 0) return i;
		}
		return -1;
	}
};