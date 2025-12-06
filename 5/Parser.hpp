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
#include <vector>
#include "Tokenizer.hpp"
#include "AST.hpp"
using namespace std;

typedef Tokenizer::Token Token;
typedef Tokenizer::Token::TokenType TokenType;
typedef AST::Node Node;

class Parser
{
public:
	vector<Token> tokenList;
	int tokenIndex = 0;

	AST resultTree;
	
	const Tokenizer& tokenizer;

	Parser(const Tokenizer& inputTokenizer) : tokenList(inputTokenizer.tokenList), resultTree(inputTokenizer), tokenizer(inputTokenizer)
	{
		resultTree.root = parseStatementList();
	}

private:
	void consumeToken()
	{
		if (tokenIndex < tokenList.size()) tokenIndex++;
	}

	const Token& peek(int n = 0) { return tokenList[tokenIndex + n]; }

	AST::StatementList* parseStatementList()
	{
		AST::StatementList * returnNode = new AST::StatementList;
		while (peek().type != TokenType::END)
		{
			if (peek().type == TokenType::THEREFORE)
			{
				consumeToken();
			}
			returnNode->statements.push_back(parseConditional());
			if (peek().type == TokenType::SEMICOLON) consumeToken();
			else throw runtime_error("Syntax error, expected ';' after statement");
		}
		return returnNode;
	}

	AST::Node* parseConditional()
	{
		AST::Node* returnNode;
		if (peek().type == TokenType::IF)
		{
			consumeToken();
			returnNode = parseExpression();
			if (peek().type == TokenType::THEN)
			{
				consumeToken();
				returnNode = new AST::ConditionalNode(returnNode, parseExpression());
			}
			else throw runtime_error("Syntax error, if with no then");
		}
		else
		{
			returnNode = parseExpression();
			if (peek().type == TokenType::ARROW)
			{
				consumeToken();
				returnNode = new AST::ConditionalNode(returnNode, parseExpression());
			}
		}
		return returnNode;
	}

	AST::Node* parseExpression()
	{
		Node* returnNode = parseTerm();
		while (peek().type == TokenType::OR)
		{
			consumeToken();
			returnNode = new AST::OrNode(returnNode, parseTerm());
		}
		return returnNode;
	}

	AST::Node* parseTerm()
	{
		Node* returnNode = parseFactor();
		while (peek().type == TokenType::AND)
		{
			consumeToken();
			returnNode = new AST::AndNode(returnNode, parseFactor());
		}
		return returnNode;
	}

	AST::Node* parseFactor()
	{
		string identifier;
		Node* returnNode;
		switch (peek().type)
		{
		case TokenType::VARIABLE:
			identifier = peek().text;
			consumeToken();
			return new AST::VariableNode(tokenizer.getVariableIndex(identifier));
		case TokenType::LPAREN:
			consumeToken();
			returnNode = parseExpression();
			if (peek().type == TokenType::RPAREN)
			{
				consumeToken();
				return returnNode;
			}
			else throw runtime_error("Expected ')'");
		case TokenType::NOT:
			consumeToken();
			return new AST::NotNode(parseFactor());
		default:
			throw runtime_error("Unexpected token in parseFactor()");
		}
	}
};