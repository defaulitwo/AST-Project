// abstract syntax tree
// this is the output of the parser stage after it parses the token list
// Node class is a generic virtual class that all specific node types inherit from
// each concrete node type implements its own code generation method, as it is distinct for all of them
// this structure also allows abstract types to be a common interface for the inheriting node types, as used in Parser.hpp parsing functions

#pragma once
#include <string>
#include <iostream>
#include <ostream>
#include "DynamicList.hpp";
#include "ExecutionResult.hpp";
#include "Scope.hpp";
using namespace std;

class AST
{
public:

	class Node
	{
	public:
		Node() = default;
		virtual ~Node() = default;
		virtual ExecutionResult execute(Scope& scope)  = 0;
	};

	class StatementNode : public Node
	{
	public:
		StatementNode() = default;
		virtual ~StatementNode() = default;
		virtual ExecutionResult execute(Scope& scope) = 0;
	};

	class ExpressionNode : public Node
	{
	public:
		ExpressionNode() = default;
		virtual ~ExpressionNode() = default;
		virtual ExecutionResult execute(Scope& scope) = 0;
	};

	class StatementList : public StatementNode
	{
	public:

		DynamicList<StatementNode*> statements;
		int variableCount;

		StatementList() : variableCount(0) { }
		virtual ~StatementList() { for (Node* n : statements) delete n; }
		virtual ExecutionResult execute(Scope& scope) override
		{
			ExecutionResult execution;
			for (StatementNode* statement : statements)
			{
				execution = statement->execute(scope);
				if (execution.type == ExecutionResult::Type::Break) { return ExecutionResult(execution.type); }
				if (execution.type == ExecutionResult::Type::VariableDeclaration) { variableCount++; }
			}
			scope.pop(variableCount);
			variableCount = 0; // reset variableCount for later calls
			return execution;
		}
	};

	StatementNode* root;

	AST() : root(nullptr) { }

	class IdentifierNode : public ExpressionNode
	{
	public:
		string identifier;
		IdentifierNode(const string& name) : identifier(name) { }
		~IdentifierNode() { }
		int getValue(Scope& scope) const
		{
			return scope.getValue(identifier);
		}
		void setValue(Scope& scope, int v)
		{
			scope.setValue(identifier, v);
		}
		virtual ExecutionResult execute(Scope& scope) override
		{
			return ExecutionResult(ExecutionResult::Type::Return, getValue(scope));
		}
	};

	class IntegerLiteralNode : public ExpressionNode
	{
	public:
		int value;
		IntegerLiteralNode(int v = 0) : value(v) { }
		virtual ExecutionResult execute(Scope& scope) override
		{
			return ExecutionResult(ExecutionResult::Type::Normal,value);
		}
	};

	class booleanLiteralNode : public ExpressionNode
	{
	public:

	};

	class BinaryOpNode : public ExpressionNode
	{
	public:
		enum class Mode { ADD, SUB, MUL, DIV, MOD, ASS, AND, OR, EQ, LT, GT, LTE, GTE };

		Mode mode;
		ExpressionNode* LOperand;
		ExpressionNode* ROperand;
		
		BinaryOpNode(Mode m, ExpressionNode* l = nullptr, ExpressionNode* r = nullptr) : mode(m), LOperand(l), ROperand(r) { }
		~BinaryOpNode() { delete LOperand; delete ROperand; }
		virtual ExecutionResult execute(Scope& scope) override
		{
			ExecutionResult result;
			switch (mode)
			{
			case Mode::ADD:	result.value = LOperand->execute(scope).value + ROperand->execute(scope).value; break;
			case Mode::SUB:	result.value = LOperand->execute(scope).value - ROperand->execute(scope).value; break;
			case Mode::MUL:	result.value = LOperand->execute(scope).value * ROperand->execute(scope).value; break;
			case Mode::DIV:	result.value = LOperand->execute(scope).value / ROperand->execute(scope).value; break;
			case Mode::MOD:	result.value = LOperand->execute(scope).value % ROperand->execute(scope).value; break;
			case Mode::GT:	result.value = LOperand->execute(scope).value > ROperand->execute(scope).value; break;
			case Mode::LT:	result.value = LOperand->execute(scope).value < ROperand->execute(scope).value; break;
			case Mode::AND: result.value = LOperand->execute(scope).value && ROperand->execute(scope).value; break;
			case Mode::OR:	result.value = LOperand->execute(scope).value || ROperand->execute(scope).value; break;
			case Mode::ASS: 
				result = ROperand->execute(scope);
				((IdentifierNode*)LOperand)->setValue(scope, result.value);
				break;
			}
			return result;
		}
	};

	class UnaryOpNode : public ExpressionNode
	{
	public:
		enum class Mode { NEG, INC, DEC, NOT };

		Mode mode;
		ExpressionNode* operand;

		UnaryOpNode(Mode m, ExpressionNode* o) : mode(m), operand(o) { }
		~UnaryOpNode() { delete operand; }
		virtual ExecutionResult execute(Scope& scope) override
		{
			ExecutionResult result;
			switch (mode)
			{
			case Mode::NEG:
				result.value = -(operand->execute(scope).value);
				break;
			case Mode::NOT:
				result.value = !(operand->execute(scope).value);
				break;
			}
			return result;
		}
	};

	class ExpressionStatementNode : public StatementNode
	{
	public:
		ExpressionNode* expression;
		ExpressionStatementNode(ExpressionNode* e = nullptr) : expression(e) { }
		~ExpressionStatementNode() { delete expression; }
		virtual ExecutionResult execute(Scope& scope) override
		{
			return expression->execute(scope);
		}
	};

	class VariableDeclarationNode : public StatementNode
	{
	public:
		string identifier;
		ExpressionNode* initializerExpression;
		VariableDeclarationNode(const string& identifier, ExpressionNode* n = nullptr) : identifier(identifier), initializerExpression(n) { }
		~VariableDeclarationNode() { }
		virtual ExecutionResult execute(Scope& scope) override
		{
			if (initializerExpression)
			{
				ExecutionResult execution = initializerExpression->execute(scope);
				scope.pushVariable(identifier, execution.value);
			}
			else scope.pushVariable(identifier, 0);
			return ExecutionResult(ExecutionResult::Type::VariableDeclaration);
		}
	};

	class PrintNode : public StatementNode
	{
	public:
		ExpressionNode* expression;
		PrintNode(ExpressionNode* e = nullptr) : expression(e) { }
		~PrintNode() { delete expression; }
		virtual ExecutionResult execute(Scope& scope) override
		{
			cout << expression->execute(scope).value << endl;
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};

	class IfNode : public StatementNode
	{
	public:
		ExpressionNode* condition;
		StatementNode* body;
		StatementNode* elseBody;

		IfNode() : condition(nullptr), body(nullptr), elseBody(nullptr) { }
		~IfNode() { delete condition; delete body; delete elseBody; }
		virtual ExecutionResult execute(Scope& scope) override
		{
			if (condition->execute(scope).value != 0) return body->execute(scope);
			else if (elseBody) return (elseBody->execute(scope));
			else return ExecutionResult(ExecutionResult::Type::Null);
		}
	};

	class IfExpressionNode : public ExpressionNode
	{
	public:
		IfNode* ifNode;
		IfExpressionNode(IfNode* n = nullptr) : ifNode(n) { }
		~IfExpressionNode() { delete ifNode; }
		virtual ExecutionResult execute(Scope& scope) override
		{
			return ifNode->execute(scope);
		}
	};

	class WhileNode : public StatementNode
	{
	public:
		ExpressionNode* condition;
		StatementNode* body;

		WhileNode() : condition(nullptr), body(nullptr) { }
		~WhileNode() { delete condition; delete body; }
		virtual ExecutionResult execute(Scope& scope) override
		{
			while (condition->execute(scope).value)
			{
				ExecutionResult execution = body->execute(scope);
				if (execution.type == ExecutionResult::Type::Break) { break; }
				if (execution.type == ExecutionResult::Type::Continue) { continue; }
			}
			return ExecutionResult();
		}
	};

	class BreakNode : public StatementNode
	{
	public:
		BreakNode() = default;
		~BreakNode() = default;
		virtual ExecutionResult execute(Scope& scope) override
		{
			return ExecutionResult(ExecutionResult::Type::Break);
		}
	};

	void execute() 
	{
		Scope scope;
		root->execute(scope);
	}
};
