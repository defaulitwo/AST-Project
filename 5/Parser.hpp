// recursive descent parser
// each grammar rule is turned into a recursive function
// each function returns a node of the abstract syntax tree
// after parsing is done, the root node of the AST is returned, and the tree is fully constructed

#pragma once
//#include <vector> replaced by DynamicList.hpp
#include "DynamicList.hpp"
#include "Tokenizer.hpp"
#include "AST.hpp"
using namespace std;

typedef Tokenizer::Token Token;
typedef Tokenizer::Token::TokenType TokenType;
typedef AST::Node Node;
//typedef AST::Node::NodeType NodeType;

class Parser
{
public:
	DynamicList<Token> tokenList;
	int tokenIndex = 0;

	AST resultTree;

	Parser(const Tokenizer& inputTokenizer) : tokenList(inputTokenizer.tokenList)
	{
		resultTree.root = parseStatementList();
	}

private:
	void consumeToken()
	{
		if (tokenList[tokenIndex].type != TokenType::END) tokenIndex++;
	}

	const Token& peek(int n = 0)
	{
		if (tokenIndex + n >= tokenList.size()) throw runtime_error("Invalid peek attempt");
		return tokenList[tokenIndex + n];
	}

	AST::StatementList* parseStatementList()
	{
		AST::StatementList* returnNode = new AST::StatementList();
		while (peek().type != TokenType::END && peek().type != TokenType::RCURLY)
			returnNode->statements.push(parseStatement());
		return returnNode;
	}

	AST::StatementNode* parseStatement()
	{
		//AST::StatementNode* returnNode;
		switch (peek().type)
		{
		case TokenType::IDENT:
			AST::AssignNode* node = new AST::AssignNode();
			node->LValue = new AST::IdentifierNode(peek().text);
			consumeToken();
			if (peek().type != TokenType::EQUAL) throw runtime_error("Expected '=' after identifier");
			consumeToken();
			node->RValue = parseExpression();
			if (peek().type != TokenType::SEMICOLON) throw runtime_error("Expected ';'");
			consumeToken();
			return node;
			break;
		//case TokenType::PRINT:
			// todo
		default:
			throw runtime_error("Unexpected token in parseStatement()");
		}
	}

	AST::ExpressionNode* parseExpression()
	{
		AST::ExpressionNode* returnNode = parseTerm();

		while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS)
		{
			TokenType operation = peek().type;
			consumeToken();
			switch (operation)
			{
			case TokenType::PLUS:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::ADD, returnNode, parseTerm()); 
				break;
			case TokenType::MINUS:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::SUB, returnNode, parseTerm()); 
				break;
			}
		}

		return returnNode;
	}

	AST::ExpressionNode* parseTerm()
	{
		AST::ExpressionNode* returnNode = parseFactor();

		while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH)
		{
			TokenType operation = peek().type;
			consumeToken();
			switch (operation)
			{
			case TokenType::STAR:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::MUL, returnNode, parseFactor()); break;
			case TokenType::SLASH:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::DIV, returnNode, parseFactor()); break;
			}
		}

		return returnNode;
	}

	AST::ExpressionNode* parseFactor()
	{
		string identifier;
		switch (peek().type)
		{
		case TokenType::NUM:
			int value = peek().value;
			consumeToken();
			return new AST::IntegerLiteralNode(value);
			break;
		case TokenType::IDENT:
			identifier = peek().text;
			consumeToken();
			return new AST::IdentifierNode(identifier);
			break;
		case TokenType::MINUS:
			consumeToken();
			return new AST::UnaryOpNode(AST::UnaryOpNode::Mode::NEG, parseFactor());
			break;
		case TokenType::LPAREN:
			consumeToken();
			AST::ExpressionNode* expression = parseExpression();
			if (peek().type != TokenType::RPAREN) throw runtime_error("Expected ')'");
			consumeToken();
			return expression;
			break;
		default:
			throw runtime_error("Unexpected token in parseFactor()");
		}
	}
};