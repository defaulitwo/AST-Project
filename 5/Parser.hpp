// recursive descent parser
// each grammar rule is turned into a recursive function
// each function returns a node of the abstract syntax tree
// after parsing is done, the root node of the AST is returned, and the tree is fully constructed

#pragma once
//#include <vector> replaced by custom DynamicList.hpp
#include "DynamicList.hpp"
#include "Tokenizer.hpp"
#include "AST.hpp"
using namespace std;

typedef Tokenizer::Token Token;

class Parser
{
public:
	DynamicList<Token> tokenList;
	int tokenIndex;

	Parser() : tokenIndex(0) { }

	AST Parse(const Tokenizer& inputTokenizer)
	{
		tokenList = inputTokenizer.tokenList;
		AST resultTree;
		resultTree.root = parseStatementList();
		return resultTree;
	}

private:

	Token peek(int n = 0)
	{
		if (tokenIndex + n >= tokenList.size()) return Token(Token::Type::ERROR);
		return tokenList[tokenIndex + n];
	}

	Token eat()
	{
		Token returnToken = peek();
		if (tokenList[tokenIndex].type != Token::Type::END) tokenIndex++;
		else throw runtime_error("Reached end of token list");
		return returnToken;
	}

	Token expect(Token::Type type)
	{
		if (peek().type != type)
		{
			cout << (int)type;
			throw runtime_error("Unexpected Token");
		}
		else
		{
			return eat();
		}
	}

	AST::StatementNode* parseStatement()
	{
		AST::StatementNode* returnNode;
		switch (peek().type)
		{
		case Token::Type::LCURLY: // statement block {...}
			returnNode = parseBlock();
			break;
		case Token::Type::PRINT: // print statement
			returnNode = parsePrint();
			break;
		case Token::Type::IF: // if statement
			returnNode = parseIf();
			break;
		case Token::Type::WHILE: // while loop
			returnNode = parseWhile();
			break;
		case Token::Type::BREAK: // break
			returnNode = parseBreak();
			break;
		default: // expression statement ex: x = 10+3;
			returnNode = new AST::ExpressionStatementNode(parseExpression());
			expect(Token::Type::SEMICOLON);
		}
		return returnNode;
	}

	AST::StatementList* parseBlock()
	{
		AST::StatementList* returnNode;
		expect(Token::Type::LCURLY);
		returnNode = parseStatementList();
		expect(Token::Type::RCURLY);
		return returnNode;
	}

	AST::StatementList* parseStatementList()
	{
		AST::StatementList* returnNode = new AST::StatementList();
		while (peek().type != Token::Type::END && peek().type != Token::Type::RCURLY)
			returnNode->statements.push(parseStatement());
		return returnNode;
	}

	AST::PrintNode* parsePrint()
	{
		AST::PrintNode* returnNode;
		expect(Token::Type::PRINT);
		returnNode = new AST::PrintNode(parseExpression());
		expect(Token::Type::SEMICOLON);
		return returnNode;
	}

	AST::IfNode* parseIf()
	{
		expect(Token::Type::IF);
		AST::IfNode* returnNode = new AST::IfNode();
		if (peek().type == Token::Type::LPAREN)
		{
			expect(Token::Type::LPAREN);
			returnNode->condition = parseExpression();
			expect(Token::Type::RPAREN);
			returnNode->body = parseStatement();
		}
		else
		{
			returnNode->condition = parseExpression();
			returnNode->body = parseStatement();
		}
		if (peek().type == Token::Type::ELSE)
		{
			expect(Token::Type::ELSE);
			returnNode->elseBody = parseStatement();
		}
		return returnNode;
	}

	AST::WhileNode* parseWhile()
	{
		expect(Token::Type::WHILE);
		AST::WhileNode* returnNode = new AST::WhileNode();
		if (peek().type == Token::Type::LPAREN)
		{
			expect(Token::Type::LPAREN);
			returnNode->condition = parseExpression();
			expect(Token::Type::RPAREN);
			returnNode->body = parseStatement();
		}
		else
		{
			returnNode->condition = parseExpression();
			returnNode->body = parseStatement();
		}
		return returnNode;
	}

	AST::BreakNode* parseBreak()
	{
		expect(Token::Type::BREAK);
		expect(Token::Type::SEMICOLON);
		return new AST::BreakNode;
	}

	AST::ExpressionNode* parseExpression()
	{
		switch (peek().type)
		{
		case Token::Type::IF: // if expression, ex: x = if(y) {10;} else {20;};
			return new AST::IfExpressionNode(parseIf());
		default:
			return parseAssignment();
		}
	}

	AST::ExpressionNode* parseAssignment()
	{
		if (peek().type == Token::Type::IDENT && peek(1).type == Token::Type::EQUAL)
		{
			Token identifierToken = eat();
			expect(Token::Type::EQUAL); // consume '='
			AST::ExpressionNode* returnNode = new AST::BinaryOpNode(
				AST::BinaryOpNode::Mode::ASS, 
				new AST::IdentifierNode(identifierToken.text),
				parseExpression()
			);
			return returnNode;
		}
		else
		{
			return parseLogical();
		}
	}

	AST::ExpressionNode* parseLogical()
	{
		AST::ExpressionNode* returnNode = parseEquality();
		while (peek().type == Token::Type::AND || peek().type == Token::Type::OR)
		{
			Token operation = eat();
			switch (operation.type)
			{
			case Token::Type::AND:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::AND, returnNode, parseEquality());
				break;
			case Token::Type::OR:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::OR, returnNode, parseEquality());
				break;
			}
		}
		return returnNode;
	}

	AST::ExpressionNode* parseEquality()
	{
		AST::ExpressionNode* returnNode = parseComparison();
		// while (peek().type == Token::Type::) TODO
		return returnNode;
	}

	AST::ExpressionNode* parseComparison()
	{
		AST::ExpressionNode* returnNode = parseAdditive();
		while (peek().type == Token::Type::LESS || peek().type == Token::Type::GREATER)
		{
			Token operation = eat();
			switch (operation.type)
			{
			case Token::Type::GREATER:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::GT, returnNode, parseAdditive());
				break;
			case Token::Type::LESS:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::LT, returnNode, parseAdditive());
				break;
			}
		}
		return returnNode;
	}

	AST::ExpressionNode* parseAdditive()
	{
		AST::ExpressionNode* returnNode = parseMultiplicative();
		while (peek().type == Token::Type::PLUS || peek().type == Token::Type::MINUS)
		{
			Token operation = eat();
			switch (operation.type)
			{
			case Token::Type::PLUS:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::ADD, returnNode, parseMultiplicative()); 
				break;
			case Token::Type::MINUS:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::SUB, returnNode, parseMultiplicative()); 
				break;
			}
		}
		return returnNode;
	}

	AST::ExpressionNode* parseMultiplicative()
	{
		AST::ExpressionNode* returnNode = parseUnary();
		while (peek().type == Token::Type::STAR || peek().type == Token::Type::SLASH || peek().type == Token::Type::MOD)
		{
			Token operation = eat();
			switch (operation.type)
			{
			case Token::Type::STAR:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::MUL, returnNode, parseUnary()); 
				break;
			case Token::Type::SLASH:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::DIV, returnNode, parseUnary()); 
				break;
			case Token::Type::MOD:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::MOD, returnNode, parseUnary()); 
				break;
			}
		}
		return returnNode;
	}

	AST::ExpressionNode* parseUnary()
	{
		AST::ExpressionNode* returnNode;
		switch (peek().type)
		{
		case Token::Type::MINUS:
			expect(Token::Type::MINUS);
			returnNode = new AST::UnaryOpNode(AST::UnaryOpNode::Mode::NEG, parseUnary());
			break;
		case Token::Type::NOT:
			expect(Token::Type::NOT);
			returnNode = new AST::UnaryOpNode(AST::UnaryOpNode::Mode::NOT, parseUnary());
			break;
		case Token::Type::PLUSPLUS:
			expect(Token::Type::PLUSPLUS);
			returnNode = new AST::UnaryOpNode(AST::UnaryOpNode::Mode::INC, parseUnary());
			break;
		default: 
			returnNode = parsePrimary();
		}
		return returnNode;
	}

	AST::ExpressionNode* parsePrimary()
	{
		string identifier;
		int value;
		AST::ExpressionNode* returnNode;
		switch (peek().type)
		{
		case Token::Type::NUM:
			value = eat().value;
			returnNode = new AST::IntegerLiteralNode(value);
			break;
		case Token::Type::IDENT:
			identifier = eat().text;
			returnNode = new AST::IdentifierNode(identifier);
			break;
		case Token::Type::LPAREN:
			eat();
			returnNode = parseExpression();
			expect(Token::Type::RPAREN);
			break;
		default:
			cout << (int)peek().type << endl;
			throw runtime_error("Unexpected token in parsePrimary()");
		}
		return returnNode;
	}
};