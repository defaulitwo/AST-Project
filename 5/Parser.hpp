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
		resultTree.root = parseCodeSnippet();
		return resultTree;
	}

private:

	Token peek(int n = 0)
	{
		if (tokenIndex + n >= tokenList.size()) return Token(Token::Type::ERROR);
		return tokenList[tokenIndex + n];
	}

	Token eat() // consumes a token
	{
		Token returnToken = peek();
		if (returnToken.type != Token::Type::ERROR) tokenIndex++;
		return returnToken;
	}

	Token expect(Token::Type type) // special version of eat() that asserts a certain token type
	{
		if (peek().type != type)
		{
			string error = "Parse error: Unexpected token, expected "
				+ Token::toString(type)
				+ ", found " + Token::toString(peek().type) 
				+ ", at line " + to_string(peek().lineNumber);
			throw runtime_error(error);
		}
		else
		{
			return eat();
		}
	}

	AST::Node* parseCodeSnippet()
	{
		if (peek().type == Token::Type::FUNC) return parseFunctionDeclaration();
		else return parseStatementList();
	}

	AST::FunctionDeclarationNode* parseFunctionDeclaration()
	{
		AST::FunctionDeclarationNode* returnNode = new AST::FunctionDeclarationNode();
		expect(Token::Type::FUNC);
		returnNode->identifier = expect(Token::Type::IDENT).text;
		expect(Token::Type::LPAREN);
		if (peek().type != Token::Type::RPAREN)
		{
			while (peek().type != Token::Type::RPAREN)
			{
				returnNode->parameters.push(expect(Token::Type::IDENT).text);
				if (peek().type != Token::Type::RPAREN) expect(Token::Type::COMMA);
			}
			expect(Token::Type::RPAREN);
		}
		else
		{
			expect(Token::Type::RPAREN);
		}
		returnNode->body = parseBlock();
		return returnNode;
	}

	AST::StatementList* parseStatementList() // statement list
	{
		AST::StatementList* returnNode = new AST::StatementList();
		while (peek().type != Token::Type::END && peek().type != Token::Type::RCURLY)
		{
			AST::StatementNode* statement = parseStatement();
			if (statement) returnNode->statements.push(statement);
		}
		return returnNode;
	}

	AST::StatementNode* parseStatement() // statement
	{
		AST::StatementNode* returnNode;
		switch (peek().type)
		{
		case Token::Type::INPUT: // variable declarations
		case Token::Type::GLOBAL:
		case Token::Type::VAR: 
			returnNode = parseVariableDeclaration();
			// handling consuming semicolon here to allow variable declaration in for loop
			expect(Token::Type::SEMICOLON); 
			break;
		case Token::Type::LCURLY: // statement block {...}
			returnNode = parseBlock();
			break;
		case Token::Type::PRINTLN: // print statement
		case Token::Type::PRINTCHAR:
		case Token::Type::PRINT:
			returnNode = parsePrint();
			break;
		case Token::Type::IF: // if statement
			returnNode = parseIf();
			break;
		case Token::Type::WHILE: // while loop
			returnNode = parseWhile();
			break;
		case Token::Type::FOR: // for loop
			returnNode = parseFor();
			break;
		case Token::Type::REPEAT: // repeat
			returnNode = parseRepeat();
			break;
		case Token::Type::DO: // do-while loop
			returnNode = parseDoWhile();
			break;
		case Token::Type::RETURN: // jump statements
		case Token::Type::BREAK:
			returnNode = parseJumpStatement();
			break;
		default: // expression statement ex: x = 10+3;
			returnNode = new AST::ExpressionStatementNode(parseExpression());
			// semicolon is optional for expression statement
			if (peek().type == Token::Type::SEMICOLON) expect(Token::Type::SEMICOLON);
		}
		return returnNode;
	}

	AST::StatementList* parseBlock() // block { ... }
	{
		AST::StatementList* returnNode;
		expect(Token::Type::LCURLY);
		returnNode = parseStatementList();
		expect(Token::Type::RCURLY);
		return returnNode;
	}

	AST::VariableDeclarationNode* parseVariableDeclaration() // variable declarations
	{
		AST::VariableDeclarationNode* returnNode;
		returnNode = new AST::VariableDeclarationNode();
		Token declaration = peek();
		eat();
		switch (declaration.type)
		{
		case Token::Type::VAR: // local scope variable
			returnNode->type = AST::VariableDeclarationNode::Type::VARIABLE;
			break;
		case Token::Type::GLOBAL: // global variable
			returnNode->type = AST::VariableDeclarationNode::Type::GLOBAL;
			break;
		case Token::Type::INPUT: // input variable, also a local variable but user can give input
			returnNode->type = AST::VariableDeclarationNode::Type::INPUT;
			break;
		default:
			expect(Token::Type::VAR);
		}
		returnNode->identifier = expect(Token::Type::IDENT).text;
		if (peek().type == Token::Type::EQUAL)
		{
			expect(Token::Type::EQUAL);
			returnNode->initializerExpression = parseExpression();
		}
		// note, semicolon not handled here
		return returnNode;
	}

	AST::PrintNode* parsePrint() // print statements
	{
		AST::PrintNode* returnNode;
		returnNode = new AST::PrintNode();
		switch (peek().type)
		{
		case Token::Type::PRINT: // print, outputs value of expression evaluation
			expect(Token::Type::PRINT);
			returnNode->type = AST::PrintNode::Type::PRINTEXPR;
			break;
		case Token::Type::PRINTLN: // print line, like print but prints a newline afterwards
			expect(Token::Type::PRINTLN); 
			returnNode->type = AST::PrintNode::Type::PRINTLN;
			break;
		case Token::Type::PRINTCHAR: // printchar, prints the character whose ascii code is the expression evaluation
			expect(Token::Type::PRINTCHAR);
			returnNode->type = AST::PrintNode::Type::PRINTCHAR;
			break;
		}
		if (peek().type != Token::Type::SEMICOLON) returnNode->expression = parseExpression();
		expect(Token::Type::SEMICOLON);
		return returnNode;
	}

	AST::IfNode* parseIf() // if statements
	{
		AST::IfNode* returnNode = new AST::IfNode();
		expect(Token::Type::IF);
		returnNode->condition = parseExpression(); // condition
		returnNode->body = parseStatement(); // body
		if (peek().type == Token::Type::ELSE) // else is optional
		{
			expect(Token::Type::ELSE);
			returnNode->elseBody = parseStatement();
		}
		return returnNode;
	}

	AST::WhileNode* parseWhile() // while loop statement
	{
		AST::WhileNode* returnNode = new AST::WhileNode();
		expect(Token::Type::WHILE);
		returnNode->condition = parseExpression(); // condition
		returnNode->body = parseStatement(); // body
		return returnNode;
	}

	AST::ForNode* parseFor() // for loop statement
	{
		AST::ForNode* returnNode = new AST::ForNode();
		expect(Token::Type::FOR);
		bool expectingRParen = false;
		if (peek().type == Token::Type::LPAREN) { expectingRParen = true; expect(Token::Type::LPAREN); }
		returnNode->initialization = parseVariableDeclaration(); // initialization
		if (peek().type == Token::Type::SEMICOLON) expect(Token::Type::SEMICOLON); // separator
		else expect(Token::Type::COMMA);
		returnNode->condition = parseExpression(); // condition
		if (peek().type == Token::Type::SEMICOLON) expect(Token::Type::SEMICOLON); // separator
		else expect(Token::Type::COMMA);
		returnNode->update = parseExpression(); // update
		if (expectingRParen) expect(Token::Type::RPAREN);
		returnNode->body = parseStatement(); // body
		return returnNode;
	}

	AST::RepeatNode* parseRepeat() // repeat statement ex: repeat 5 println 10;
	{
		AST::RepeatNode* returnNode = new AST::RepeatNode();
		expect(Token::Type::REPEAT);
		returnNode->iterations = parseExpression(); // iterations
		returnNode->body = parseStatement(); // body
		return returnNode;
	}

	AST::DoWhileNode* parseDoWhile() // do-while loop statement
	{
		AST::DoWhileNode* returnNode = new AST::DoWhileNode();
		expect(Token::Type::DO);
		returnNode->body = parseStatement();
		expect(Token::Type::WHILE);
		returnNode->condition = parseExpression();
		return returnNode;
	}

	AST::JumpNode* parseJumpStatement() // jump statements, ex: break, return...
	{
		AST::JumpNode* returnNode = new AST::JumpNode();
		Token jump = eat();
		switch (jump.type)
		{
		case Token::Type::BREAK: // break, keeps exiting scopes until program exits or encounters a loop
			returnNode->type = AST::JumpNode::Type::BREAK;
			break;
		case Token::Type::RETURN: // like break, but doesnt stop at loops, and may return an expression's value
			returnNode->type = AST::JumpNode::Type::RETURN;
		}
		if (peek().type != Token::Type::SEMICOLON) returnNode->expression = parseExpression();
		expect(Token::Type::SEMICOLON);
		return returnNode;
	}

	AST::ExpressionNode* parseExpression() // expressions
	{
		switch (peek().type)
		{
		default:
			return parseAssignment();
		}
	}

	AST::ExpressionNode* parseAssignment() // assignment operators
	{
		if (peek().type == Token::Type::IDENT && peek(1).type == Token::Type::EQUAL)
		{
			Token identifierToken = expect(Token::Type::IDENT);
			expect(Token::Type::EQUAL);
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

	AST::ExpressionNode* parseLogical() // logical AND, OR, NOT
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

	AST::ExpressionNode* parseEquality() // equality operators
	{
		AST::ExpressionNode* returnNode = parseComparison();
		while (peek().type == Token::Type::EQUALITY || peek().type == Token::Type::NOTEQUALITY)
		{
			Token operation = eat();
			switch (operation.type)
			{
			case Token::Type::EQUALITY:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::EQ, returnNode, parseComparison());
				break;
			case Token::Type::NOTEQUALITY:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::NEQ, returnNode, parseComparison());
				break;
			}
		}
		return returnNode;
	}

	AST::ExpressionNode* parseComparison() // comparison operators
	{
		AST::ExpressionNode* returnNode = parseAdditive();
		while (peek().type == Token::Type::LESS || peek().type == Token::Type::GREATER ||
				peek().type == Token::Type::LESSEQUAL || peek().type == Token::Type::GREATEREQUAL)
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
			case Token::Type::GREATEREQUAL:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::GTE, returnNode, parseAdditive());
				break;
			case Token::Type::LESSEQUAL:
				returnNode = new AST::BinaryOpNode(AST::BinaryOpNode::Mode::LTE, returnNode, parseAdditive());
				break;
			}
		}
		return returnNode;
	}

	AST::ExpressionNode* parseAdditive() // addition/subtraction
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

	AST::ExpressionNode* parseMultiplicative() // multiplication/division/mod
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

	AST::ExpressionNode* parseUnary() // unary operators
	{
		AST::ExpressionNode* returnNode;
		string identifier;
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
		case Token::Type::INCREMENT:
			expect(Token::Type::INCREMENT);
			identifier = expect(Token::Type::IDENT).text;
			returnNode = new AST::UnaryOpNode(
				AST::UnaryOpNode::Mode::INC, 
				new AST::IdentifierNode(identifier)
			);
			break;
		case Token::Type::DECREMENT:
			expect(Token::Type::DECREMENT);
			identifier = expect(Token::Type::IDENT).text;
			returnNode = new AST::UnaryOpNode(
				AST::UnaryOpNode::Mode::DEC, 
				new AST::IdentifierNode(identifier)
			);
			break;
		default: 
			returnNode = parsePrimary();
		}
		return returnNode;
	}

	AST::ExpressionNode* parsePrimary() // a primary expression, which can not be divided into more expressions
	{
		AST::ExpressionNode* returnNode;
		// identifier and value are declared outside switch, but may not always be used
		string identifier;
		int value;
		switch (peek().type)
		{
		case Token::Type::NUM:
			value = expect(Token::Type::NUM).value;
			returnNode = new AST::IntegerLiteralNode(value);
			break;
		case Token::Type::CHAR:
			value = (int)expect(Token::Type::CHAR).character;
			returnNode = new AST::IntegerLiteralNode(value);
			break;
		case Token::Type::RANDOM:
			expect(Token::Type::RANDOM);
			returnNode = new AST::RandomNode();
			break;
		case Token::Type::TRUE:
			expect(Token::Type::TRUE);
			returnNode = new AST::IntegerLiteralNode(1);
			break;
		case Token::Type::FALSE:
			expect(Token::Type::FALSE);
			returnNode = new AST::IntegerLiteralNode(0);
			break;
		case Token::Type::LPAREN:
			expect(Token::Type::LPAREN);
			returnNode = parseExpression();
			expect(Token::Type::RPAREN);
			break;
		case Token::Type::LCURLY: // statementlist expression, ex: x = { if (y > x) 10 else 20 };
			return new AST::StatementListExpressionNode(parseBlock());
		case Token::Type::IDENT:
			identifier = expect(Token::Type::IDENT).text;
			if (peek().type == Token::Type::LPAREN)
			{
				AST::callNode* newNode = new AST::callNode();
				newNode->identifier = identifier;
				expect(Token::Type::LPAREN);
				while (peek().type != Token::Type::RPAREN)
				{
					newNode->arguments.push(parseExpression());
					if (peek().type != Token::Type::RPAREN) expect(Token::Type::COMMA);
				}
				expect(Token::Type::RPAREN);
				returnNode = newNode;
			}
			else returnNode = new AST::IdentifierNode(identifier);
			break;
		default:
			throw runtime_error(
				"Parse error: Unexpected token in parsePrimary(), expected an expression, found " 
				+ Token::toString(peek().type)
				+ ", at line " + to_string(peek().lineNumber)
			);
		}
		return returnNode;
	}
};