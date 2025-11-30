// recursive descent parser
// each grammar rule is turned into a recursive function
// 
//	Expression -> Term { (+|-) Term }
//	Term       -> Factor{ (*|/) Factor }
//	Factor     -> NUMBER | '-' Factor | '(' Expression ')'
// 
// each function returns a node of the abstract syntax tree
// after parsing is done, the root node of the AST is returned

#pragma once
//#include <vector> replaced by DynamicList.hpp
#include "DynamicList.hpp"
#include "Tokenizer.hpp"
#include "AST.hpp"
using namespace std;

typedef Tokenizer::Token Token;
typedef Tokenizer::Token::TokenType TokenType;
typedef AST::Node Node;
typedef AST::Node::NodeType NodeType;

class Parser
{
public:
	//vector<Token> tokenList;
	DynamicList<Token> tokenList;
	int nextTokenIndex = 0;

	AST resultTree;

	Parser(const Tokenizer& inputTokenizer) : tokenList(inputTokenizer.tokenList)
	{
		resultTree.root = parseExpression();
	}

private:
	void scanToken()
	{
		if (nextTokenIndex < tokenList.size())
			nextTokenIndex++;
	}

	AST::Node* parseExpression()
	{
		AST::Node* node = parseTerm();

		while (tokenList[nextTokenIndex].type == TokenType::PLUS ||
			tokenList[nextTokenIndex].type == TokenType::MINUS)
		{
			TokenType op = tokenList[nextTokenIndex].type;
			scanToken(); // consume + or -

			AST::Node* right = parseTerm();

			if (op == TokenType::PLUS)
				node = new AST::AddNode(node, right);
			else
				node = new AST::SubNode(node, right);
		}

		return node;
	}

	AST::Node* parseTerm()
	{
		AST::Node* node = parseFactor();

		// While the next token is * or /
		while (tokenList[nextTokenIndex].type == TokenType::STAR ||
			tokenList[nextTokenIndex].type == TokenType::SLASH)
		{
			TokenType op = tokenList[nextTokenIndex].type;
			scanToken(); // consume the operator

			AST::Node* right = parseFactor();

			if (op == TokenType::STAR)
				node = new AST::MulNode(node, right);
			else
				node = new AST::DivNode(node, right);
		}

		return node;
	}

	AST::Node* parseFactor()
	{
		if (tokenList[nextTokenIndex].type == TokenType::NUM)
		{
			double value = tokenList[nextTokenIndex].value; // must store value before consuming token
			scanToken();
			return new AST::ValNode(value);
		}
		else if (tokenList[nextTokenIndex].type == TokenType::IDENT)
		{
			string text = tokenList[nextTokenIndex].text; // same thing here
			scanToken();
			return new AST::IdNode(text);
		}
		else if (tokenList[nextTokenIndex].type == TokenType::MINUS)
		{
			scanToken();
			return new AST::NegNode(parseFactor());
		}
		else if (tokenList[nextTokenIndex].type == TokenType::LPAREN)
		{
			scanToken();
			AST::Node* expression = parseExpression();
			if (tokenList[nextTokenIndex].type == TokenType::RPAREN)
			{
				scanToken();
				return expression;
			}
			else
			{
				throw runtime_error("Expected ')' in parseFactor()");
			}
		}
		else
		{
			throw runtime_error("Unexpected token in parseFactor()");
		}
	}
};