// abstract syntax tree
// this is the output of the parser stage after it parses the token list
// Node class is a generic virtual class that all specific node types inherit from
// each concrete node type implements its own code generation method, as it is distinct for all of them
// this structure also allows abstract types to be a common interface for the inheriting node types, as used in Parser.hpp parsing functions

#pragma once
#include <string>
#include <ostream>
#include "DynamicList.hpp";
using namespace std;

class AST
{
public:
	enum Register
	{
		$zero, $at
	};

	class Node
	{
	public:
		//enum class NodeType { VAL, ID, ADD, SUB, MUL, DIV, NEG, ASSIGN, };
		
		//NodeType type;
		Node() = default;
		virtual ~Node() = 0;
		virtual void generateCode() const = 0;
	};

	class StatementNode : public Node
	{
	public:
		StatementNode() = default;
		virtual ~StatementNode() = 0;
		virtual void generateCode() const = 0;
	};

	class ExpressionNode : public Node
	{
	public:
		ExpressionNode() = default;
		virtual ~ExpressionNode() = 0;
		virtual void generateCode() const = 0;
	};

	class StatementList : public Node
	{
	public:
		DynamicList<StatementNode*> statements;

		StatementList() { }
		~StatementList() { }
		virtual void generateCode() const override 
		{
			for (StatementNode* statement : statements) statement->generateCode();
		}
	};

	StatementList* root;
	ostream& out;

	AST() : root(nullptr), out(cout)
	{
		
	}

	class IdentifierNode : public ExpressionNode
	{
	public:
		string identifier;
		IdentifierNode(const string& name) : identifier(name) { }
		virtual void generateCode() const override
		{

		}
	};

	class IntegerLiteralNode : public ExpressionNode
	{
	public:
		int value;
		IntegerLiteralNode(int v = 0) : value(v) { }
		virtual void generateCode() const override
		{

		}
	};

	class BinaryOpNode : public ExpressionNode
	{
	public:
		enum class Mode { ADD, SUB, MUL, DIV, EQ, LT, GT, LTE, GTE };

		Mode mode;
		ExpressionNode* LOperand;
		ExpressionNode* ROperand;
		
		BinaryOpNode(Mode m, ExpressionNode* l, ExpressionNode* r) : mode(m), LOperand(l), ROperand(r) { }
		~BinaryOpNode() { delete LOperand; delete ROperand; }
		virtual void generateCode() const override
		{

		}
	};

	class UnaryOpNode : public ExpressionNode
	{
	public:
		enum class Mode { NEG };

		Mode mode;
		ExpressionNode* operand;

		UnaryOpNode(Mode m, ExpressionNode* o) : mode(m), operand(o) { }
		~UnaryOpNode() { delete operand; }
		virtual void generateCode() const override
		{

		}
	};

	class AssignNode : public StatementNode
	{
	public:
		IdentifierNode* LValue;
		ExpressionNode* RValue;

		AssignNode() : LValue(nullptr), RValue(nullptr) { }
		~AssignNode() { delete LValue; delete RValue; }
		virtual void generateCode() const override
		{

		}
	};

	class IfNode : public StatementNode
	{
	public:
		ExpressionNode* condition;
		StatementList* body;
		StatementList* elseBody;

		IfNode() : condition(nullptr), body(nullptr), elseBody(nullptr) { }
		~IfNode()
		{ delete condition; delete body; delete elseBody; }
		virtual void generateCode() const override
		{
			// still working on it...

			body->generateCode();
		}
	};


};
