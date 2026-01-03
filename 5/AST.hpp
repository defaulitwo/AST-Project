// abstract syntax tree
// this is the output of the parser stage after it parses the token list
// Node class is a generic virtual class that all specific node types inherit from
// each concrete node type implements its own execution method, as it is distinct for all of them
// this structure also allows abstract types to be a common interface for the inheriting node types, as used in Parser.hpp parsing functions

#pragma once
#include <string>
#include <iostream>
#include <ostream>
#include "DynamicList.hpp"
#include "ExecutionResult.hpp"
#include "Environment.hpp"
using namespace std;

class AST
{
public:

	class Node
	{
	public:
		Node() = default;
		virtual ~Node() = default;
		virtual ExecutionResult execute(Environment& env)  = 0;
	};

	class StatementNode : public Node
	{
	public:
		StatementNode() = default;
		virtual ~StatementNode() = default;
		virtual ExecutionResult execute(Environment& env) = 0;
	};

	class ExpressionNode : public Node
	{
	public:
		ExpressionNode() = default;
		virtual ~ExpressionNode() = default;
		virtual ExecutionResult execute(Environment& env) = 0;
	};

	class StatementList : public StatementNode
	{
	public:
		DynamicList<StatementNode*> statements;
		int variableCount;

		StatementList() : variableCount(0) { }
		virtual ~StatementList() { for (Node* n : statements) delete n; }
		virtual ExecutionResult execute(Environment& env) override
		{
			ExecutionResult execution;
			for (StatementNode* statement : statements)
			{
				execution = statement->execute(env);
				if (execution.type == ExecutionResult::Type::Break) { return execution; }
				if (execution.type == ExecutionResult::Type::VariableDeclaration) { variableCount++; }
				if (execution.type == ExecutionResult::Type::Return) { break; }
			}
			env.popVariables(variableCount);
			variableCount = 0; // reset variableCount for later calls
			return execution;
		}
	};

	class IdentifierNode : public ExpressionNode
	{
	public:
		string identifier;
		IdentifierNode(const string& name) : identifier(name) { }
		~IdentifierNode() { }
		int getValue(Environment& env) const
		{
			return env.getValue(identifier);
		}
		void setValue(Environment& env, int v)
		{
			env.setValue(identifier, v);
		}
		virtual ExecutionResult execute(Environment& env) override
		{
			return ExecutionResult(ExecutionResult::Type::Normal, getValue(env));
		}
	};

	class IntegerLiteralNode : public ExpressionNode
	{
	public:
		int value;
		IntegerLiteralNode(int v = 0) : value(v) { }
		~IntegerLiteralNode() { }
		virtual ExecutionResult execute(Environment& env) override
		{
			return ExecutionResult(ExecutionResult::Type::Normal,value);
		}
	};

	class RandomNode : public ExpressionNode
	{
	public:
		virtual ExecutionResult execute(Environment& env) override
		{
			int random = rand();
			return ExecutionResult(ExecutionResult::Type::Normal, random);
		}
	};

	class BinaryOpNode : public ExpressionNode
	{
	public:
		enum class Mode { ADD, SUB, MUL, DIV, MOD, ASS, AND, OR, EQ, NEQ, LT, GT, LTE, GTE };

		Mode mode;
		ExpressionNode* LOperand;
		ExpressionNode* ROperand;
		
		BinaryOpNode(Mode m, ExpressionNode* l = nullptr, ExpressionNode* r = nullptr) : mode(m), LOperand(l), ROperand(r) { }
		~BinaryOpNode() { delete LOperand; delete ROperand; }
		virtual ExecutionResult execute(Environment& env) override
		{
			ExecutionResult result;
			int LOperandExecution;
			int ROperandExecution;
			switch (mode)
			{
			case Mode::ADD:	result.value = LOperand->execute(env).value + ROperand->execute(env).value; break;
			case Mode::SUB:	result.value = LOperand->execute(env).value - ROperand->execute(env).value; break;
			case Mode::MUL:	result.value = LOperand->execute(env).value * ROperand->execute(env).value; break;
			case Mode::DIV:
				LOperandExecution = LOperand->execute(env).value;
				ROperandExecution = ROperand->execute(env).value;
				if (ROperandExecution == 0) throw runtime_error("Run error: Attempted to divide by zero");
				result.value = LOperandExecution / ROperandExecution; 
				break;
			case Mode::MOD:
				LOperandExecution = LOperand->execute(env).value;
				ROperandExecution = ROperand->execute(env).value;
				if (ROperandExecution == 0) throw runtime_error("Run error: Attempted to mod by zero");
				result.value = LOperandExecution % ROperandExecution;
				break;
			case Mode::EQ:	result.value = LOperand->execute(env).value == ROperand->execute(env).value; break;
			case Mode::NEQ:	result.value = LOperand->execute(env).value != ROperand->execute(env).value; break;
			case Mode::GT:	result.value = LOperand->execute(env).value > ROperand->execute(env).value; break;
			case Mode::LT:	result.value = LOperand->execute(env).value < ROperand->execute(env).value; break;
			case Mode::GTE:	result.value = LOperand->execute(env).value >= ROperand->execute(env).value; break;
			case Mode::LTE:	result.value = LOperand->execute(env).value <= ROperand->execute(env).value; break;
			case Mode::AND:	// AND, OR with short-circuiting
				if (!(LOperand->execute(env).value)) result.value = 0;
				else if (!(ROperand->execute(env).value)) result.value = 0;
				else result.value = 1;
				break;
			case Mode::OR:
				if (LOperand->execute(env).value) result.value = 1;
				else if (ROperand->execute(env).value) result.value = 1;
				else result.value = 0;
				break;
			case Mode::ASS: 
				result = ROperand->execute(env);
				((IdentifierNode*)LOperand)->setValue(env, result.value);
				break;
			}
			return result;
		}
	};

	class UnaryOpNode : public ExpressionNode
	{
	public:
		enum class Mode { NEG, NOT, INC, DEC };

		Mode mode;
		ExpressionNode* operand;

		UnaryOpNode(Mode m, ExpressionNode* o) : mode(m), operand(o) { }
		~UnaryOpNode() { delete operand; }
		virtual ExecutionResult execute(Environment& env) override
		{
			ExecutionResult result;
			string identifier;
			switch (mode)
			{
			case Mode::NEG: result.value = -(operand->execute(env).value); break;
			case Mode::NOT: result.value = !(operand->execute(env).value); break;
			case Mode::INC:
				identifier = ((IdentifierNode*)operand)->identifier;
				env.setValue(identifier, env.getValue(identifier) + 1);
				result.value = env.getValue(identifier);
				break;
			case Mode::DEC:
				identifier = ((IdentifierNode*)operand)->identifier;
				env.setValue(identifier, env.getValue(identifier) - 1);
				result.value = env.getValue(identifier);
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
		virtual ExecutionResult execute(Environment& env) override
		{
			return expression->execute(env);
		}
	};

	class VariableDeclarationNode : public StatementNode
	{
	public:
		enum class Type { VARIABLE, GLOBAL, INPUT };
		string identifier;
		ExpressionNode* initializerExpression;
		Type type;
		VariableDeclarationNode(const string& identifier = "", ExpressionNode* n = nullptr) : identifier(identifier), initializerExpression(n) {}
		~VariableDeclarationNode() { delete initializerExpression; }
		virtual ExecutionResult execute(Environment& env) override
		{
			int initialValue = 0;
			ExecutionResult execution;
			if (initializerExpression) { execution = initializerExpression->execute(env); initialValue = execution.value; }
			switch (type)
			{
			case Type::VARIABLE: env.pushVariable(identifier, initialValue); break; // local variable
			case Type::GLOBAL: env.pushGlobal(identifier, initialValue); break; // global variable
			case Type::INPUT:  // input variable
			{
				int n;
				if (!(std::cin >> n))
				{
					// invalid value, fall back to initialization
					n = initialValue;
					cin.clear();
					cin.ignore(1);
				}
				cin.ignore(1);
				env.pushVariable(identifier, n);
				break;
			}
			}
			if (type == Type::VARIABLE || type == Type::INPUT) return ExecutionResult(ExecutionResult::Type::VariableDeclaration, initialValue);
			else return ExecutionResult(ExecutionResult::Type::GlobalDeclaration, initialValue);
		}
	};	

	class PrintNode : public StatementNode
	{
	public:
		enum class Type { PRINTEXPR, PRINTCHAR, PRINTLN };
		Type type;
		ExpressionNode* expression;
		PrintNode(Type t = Type::PRINTEXPR, ExpressionNode* e = nullptr) : type(t), expression(e) { }
		~PrintNode() { delete expression; }
		virtual ExecutionResult execute(Environment& env) override
		{
			if (expression)
			{
				if (type == Type::PRINTCHAR) cout << (char)expression->execute(env).value;
				else cout << expression->execute(env).value;
			}
			if (type == Type::PRINTLN) cout << endl;
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
		virtual ExecutionResult execute(Environment& env) override
		{
			if (condition->execute(env).value != 0) return body->execute(env);
			else if (elseBody) return (elseBody->execute(env));
			else return ExecutionResult(ExecutionResult::Type::Null);
		}
	};

	class StatementListExpressionNode : public ExpressionNode
	{
	public:
		StatementNode* statementList;
		StatementListExpressionNode(StatementNode* n = nullptr) : statementList(n) { }
		~StatementListExpressionNode() { delete statementList; }
		virtual ExecutionResult execute(Environment& env) override
		{
			return statementList->execute(env);
		}
	};

	class WhileNode : public StatementNode
	{
	public:
		ExpressionNode* condition;
		StatementNode* body;

		WhileNode() : condition(nullptr), body(nullptr) { }
		~WhileNode() { delete condition; delete body; }
		virtual ExecutionResult execute(Environment& env) override
		{
			ExecutionResult execution;
			while (condition->execute(env).value)
			{
				execution = body->execute(env);
				if (execution.type == ExecutionResult::Type::Break) { break; }
				if (execution.type == ExecutionResult::Type::Continue) { continue; }
				if (execution.type == ExecutionResult::Type::Return) { return execution; }
			}
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};

	class DoWhileNode : public StatementNode
	{
	public:
		ExpressionNode* condition;
		StatementNode* body;

		DoWhileNode() : condition(nullptr), body(nullptr) {}
		~DoWhileNode() { delete condition; delete body; }
		virtual ExecutionResult execute(Environment& env) override
		{
			ExecutionResult execution;
			do
			{
				execution = body->execute(env);
				if (execution.type == ExecutionResult::Type::Break) { break; }
				if (execution.type == ExecutionResult::Type::Continue) { continue; }
				if (execution.type == ExecutionResult::Type::Return)
				{
					return execution;
				}
			} while (condition->execute(env).value);
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};

	class ForNode : public StatementNode
	{
	public:
		VariableDeclarationNode* initialization;
		ExpressionNode* condition;
		ExpressionNode* update;
		StatementNode* body;
		ForNode() : initialization(nullptr), condition(nullptr), update(nullptr), body(nullptr) { }
		~ForNode() { delete initialization; delete condition; delete update; delete body; }
		virtual ExecutionResult execute(Environment& env) override
		{
			if (initialization) initialization->execute(env);
			while (condition->execute(env).value)
			{
				ExecutionResult execution = body->execute(env);
				if (execution.type == ExecutionResult::Type::Break) { break; }
				if (execution.type == ExecutionResult::Type::Continue) { continue; }
				if (execution.type == ExecutionResult::Type::Return) 
				{ 
					env.popVariables(1); // pop the for loop's initialization variable
					return execution;
				}
				update->execute(env);
			}
			if (initialization) env.popVariables(1); // pop the for loop's initialization variable
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};

	class RepeatNode : public StatementNode
	{
	public:
		ExpressionNode* iterations;
		StatementNode* body;
		RepeatNode() : iterations(nullptr), body(nullptr) { }
		~RepeatNode() { delete iterations; delete body; }
		virtual ExecutionResult execute(Environment& env) override
		{
			int iterationCount = iterations->execute(env).value;
			if (iterationCount > 0)
			{
				for (int i = 0; i < iterationCount; i++)
				{
					ExecutionResult execution = body->execute(env);
					if (execution.type == ExecutionResult::Type::Break) { break; }
					if (execution.type == ExecutionResult::Type::Continue) { continue; }
					if (execution.type == ExecutionResult::Type::Return)
					{
						return execution;
					}
				}
			}
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};

	class JumpNode : public StatementNode
	{
	public:
		enum class Type { BREAK, RETURN };
		Type type;
		ExpressionNode* expression;
		JumpNode() : expression(nullptr) { };
		~JumpNode() { delete expression; };
		virtual ExecutionResult execute(Environment& env) override
		{
			switch (type)
			{
			case Type::BREAK: return ExecutionResult(ExecutionResult::Type::Break);
			case Type::RETURN:
				if (expression) return ExecutionResult(ExecutionResult::Type::Return, expression->execute(env).value);
				else return ExecutionResult(ExecutionResult::Type::Return);
			}
		}
	};

	class FunctionDeclarationNode : public Node
	{
	public:
		string identifier;
		DynamicList<string> parameters;
		StatementList* body;
		FunctionDeclarationNode() { }
		~FunctionDeclarationNode() { }
		virtual ExecutionResult execute(Environment& env) override
		{
			env.pushFunction(identifier, parameters);
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
		
	};

	class callNode : public ExpressionNode
	{
	public:
		string identifier;
		DynamicList<ExpressionNode*> arguments;
		callNode() { }
		~callNode() { for (ExpressionNode* n : arguments) delete n; }
		virtual ExecutionResult execute(Environment& env) override
		{
			DynamicList<int> inputs;
			for (ExpressionNode* n : arguments) inputs.push(n->execute(env).value);
			Environment::Function& f = env.getFunction(identifier, inputs);
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};
	
	ExecutionResult execute(Environment& env) 
	{
		return root->execute(env);
	}

	Node* root;
	AST() : root(nullptr) { }
};

