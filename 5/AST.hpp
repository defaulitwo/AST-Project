// abstract syntax tree
// this is the output of the parser stage after it parses the token list
// Node class is a generic virtual class that all specific node types inherit from
// each concrete node type implements its own execution method, as it is distinct for all of them
// execute(env) works as a common interface between nodes to be able to execute each other
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

	class Node // abstract node type
	{
	public:
		Node() = default;
		virtual ~Node() = default;
		virtual ExecutionResult execute(Environment& env) = 0; // pure virtual method, makes it so class is abstract and cannot be instantiated.
	};

	class LValue // LValue interface, for things that can be assigned to
	{
	public:
		virtual ~LValue() = default;
		virtual long long getValue(Environment& env) = 0;
		virtual void setValue(Environment& env, long long v) = 0;
	};
	
	class StatementNode : public Node // abstract statement node type
	{
	public:
		StatementNode() = default;
		virtual ~StatementNode() = default;
		virtual ExecutionResult execute(Environment& env) = 0;
	};

	class ExpressionNode : public Node // abstract expression node type
	{
	public:
		ExpressionNode() = default;
		virtual ~ExpressionNode() = default;
		virtual ExecutionResult execute(Environment& env) = 0;
	};

	class StatementList : public StatementNode // statement list, AKA a scope { }
	{
	public:
		int variableCount; // used for tracking number of variables on stack, to pop when out of scope
		DynamicList<StatementNode*> statements;
		StatementList() : variableCount(0) { }
		virtual ~StatementList() { for (Node* n : statements) delete n; }
		virtual ExecutionResult execute(Environment& env) override
		{
			ExecutionResult execution;
			for (StatementNode* statement : statements)
			{
				execution = statement->execute(env);
				if (execution.type == ExecutionResult::Type::Break) { return execution; }
				if (execution.type == ExecutionResult::Type::Return) { break; }
				if (execution.type == ExecutionResult::Type::VariableDeclaration) { variableCount++; }
				else if (execution.type == ExecutionResult::Type::ArrayDeclaration) 
				{ 
					variableCount += execution.value; 
				}
			}
			clean(env);
			return execution;
		}
		void clean(Environment& env)
		{
			env.popVariables(variableCount);
			variableCount = 0; // reset variableCount for later calls
		}
	};

	class IdentifierNode : public ExpressionNode, public LValue // identifiers (for variables)
	{
	public:
		string identifier;
		IdentifierNode(const string& name) : identifier(name) { }
		~IdentifierNode() { }
		virtual long long getValue(Environment& env) override
		{
			return env.getVariable(identifier).value;
		}
		virtual void setValue(Environment& env, long long v) override
		{
			env.setValue(identifier, v);
		}
		virtual ExecutionResult execute(Environment& env) override
		{
			return ExecutionResult(ExecutionResult::Type::Normal, getValue(env));
		}
	};

	class IntegerLiteralNode : public ExpressionNode // integer literals
	{
	public:
		long long value;
		IntegerLiteralNode(long long v = 0) : value(v) { }
		~IntegerLiteralNode() { }
		virtual ExecutionResult execute(Environment& env) override
		{
			return ExecutionResult(ExecutionResult::Type::Normal,value);
		}
	};

	class RandomNode : public ExpressionNode // random
	{
	public:
		virtual ExecutionResult execute(Environment& env) override
		{
			int random = rand();
			return ExecutionResult(ExecutionResult::Type::Normal, random);
		}
	};

	class BinaryOpNode : public ExpressionNode // binary operations
	{
	public:
		enum class Mode 
		{ 
			ADD, SUB, MUL, DIV, MOD, ASS, AND, OR, EQ, NEQ, LT, GT, LTE, GTE, 
			BITAND, BITOR, BITXOR, BITSHIFTL, BITSHIFTR
		};
		Mode mode;
		ExpressionNode* LOperand;
		ExpressionNode* ROperand;
		BinaryOpNode(Mode m, ExpressionNode* l = nullptr, ExpressionNode* r = nullptr) : mode(m), LOperand(l), ROperand(r) { }
		~BinaryOpNode() { delete LOperand; delete ROperand; }
		virtual ExecutionResult execute(Environment& env) override
		{
			long long returnValue = 0;
			switch (mode)
			{
			// mathematical operators
			case Mode::ADD: returnValue = LOperand->execute(env).value + ROperand->execute(env).value; break;
			case Mode::SUB:	returnValue = LOperand->execute(env).value - ROperand->execute(env).value; break;
			case Mode::MUL:	returnValue = LOperand->execute(env).value * ROperand->execute(env).value; break;
			case Mode::DIV: // div, mod, with runtime detection for right operand being 0 and error handling
			{
				long long ROperandExecution = ROperand->execute(env).value;
				if (ROperandExecution == 0) throw runtime_error("Run error: Attempted to divide by zero");
				returnValue = LOperand->execute(env).value / ROperandExecution;
				break;
			}
			case Mode::MOD:
			{
				long long ROperandExecution = ROperand->execute(env).value;
				if (ROperandExecution == 0) throw runtime_error("Run error: Attempted to mod by zero");
				returnValue = LOperand->execute(env).value % ROperandExecution;
				break;
			}
			// comparisons
			case Mode::EQ:	returnValue = LOperand->execute(env).value == ROperand->execute(env).value; break;
			case Mode::NEQ:	returnValue = LOperand->execute(env).value != ROperand->execute(env).value; break;
			case Mode::GT:	returnValue = LOperand->execute(env).value > ROperand->execute(env).value; break;
			case Mode::LT:	returnValue = LOperand->execute(env).value < ROperand->execute(env).value; break;
			case Mode::GTE:	returnValue = LOperand->execute(env).value >= ROperand->execute(env).value; break;
			case Mode::LTE:	returnValue = LOperand->execute(env).value <= ROperand->execute(env).value; break;
			case Mode::AND:	// logical AND, OR with short-circuiting
				if (!(LOperand->execute(env).value)) returnValue = 0;
				else if (!(ROperand->execute(env).value)) returnValue = 0;
				else returnValue = 1;
				break;
			case Mode::OR:
				if (LOperand->execute(env).value) returnValue = 1;
				else if (ROperand->execute(env).value) returnValue = 1;
				else returnValue = 0;
				break;
			case Mode::ASS: // assignment
			{
				returnValue = ROperand->execute(env).value;
				LValue* lv = dynamic_cast<LValue*>(LOperand); // dynamic cast is required here due to multiple inheritance
				if (!lv) throw std::runtime_error("Run error: attempted to assign to a non-LValue");
				lv->setValue(env, returnValue);
				break;
			}
			// bitwise logical operators
			case Mode::BITAND: 	 returnValue = LOperand->execute(env).value & ROperand->execute(env).value; break;
			case Mode::BITOR:	 returnValue = LOperand->execute(env).value | ROperand->execute(env).value; break;
			case Mode::BITXOR:	 returnValue = LOperand->execute(env).value ^ ROperand->execute(env).value; break;
			// bitwise shift operators
			case Mode::BITSHIFTL:returnValue = LOperand->execute(env).value << ROperand->execute(env).value; break;
			case Mode::BITSHIFTR:returnValue = LOperand->execute(env).value >> ROperand->execute(env).value; break;
			}
			return ExecutionResult(ExecutionResult::Type::Normal, returnValue);
		}
	};

	class UnaryOpNode : public ExpressionNode // unary operations
	{
	public:
		enum class Mode { NEG, NOT, INC, DEC, BITNOT, ADDR, DEREF };
		Mode mode;
		ExpressionNode* operand;
		UnaryOpNode(Mode m, ExpressionNode* o) : mode(m), operand(o) { }
		~UnaryOpNode() { delete operand; }
		virtual ExecutionResult execute(Environment& env) override
		{
			long long returnValue = 0;
			string identifier;
			switch (mode)
			{
			case Mode::NEG: returnValue = -(operand->execute(env).value); break;
			case Mode::NOT: returnValue = !(operand->execute(env).value); break;
			case Mode::INC:
				identifier = ((IdentifierNode*)operand)->identifier;
				env.setValue(identifier, env.getVariable(identifier).value + 1);
				returnValue = env.getVariable(identifier).value;
				break;
			case Mode::DEC:
				identifier = ((IdentifierNode*)operand)->identifier;
				env.setValue(identifier, env.getVariable(identifier).value - 1);
				returnValue = env.getVariable(identifier).value;
				break;
			case Mode::BITNOT: returnValue = ~(operand->execute(env).value); break;
			case Mode::ADDR:
				identifier = ((IdentifierNode*)operand)->identifier;
				returnValue = env.getAddress(identifier);
				break;
			case Mode::DEREF:
				{
				Environment::Variable* address = (Environment::Variable*)operand->execute(env).value;
				//env.checkAddress(address);
				returnValue = (long long)address;
				break;
				}
			}
			return ExecutionResult(ExecutionResult::Type::Normal, returnValue);
		}
	};

	class ArrayAccessNode : public ExpressionNode, public LValue // subscript operator ex: arr[4];
	{
	public:
		string identifier;
		ExpressionNode* indexExpression;
		Environment::Variable* cachedVariable;
		ArrayAccessNode(): indexExpression(nullptr), cachedVariable(nullptr) { }
		~ArrayAccessNode() { delete indexExpression; }
		virtual long long getValue(Environment& env) override
		{
			if (!cachedVariable) cachedVariable = &env.getVariable(identifier);
			long long i = this->indexExpression->execute(env).value;
			return (cachedVariable + i)->value;
		}
		virtual void setValue(Environment& env, long long v) override
		{
			if (!cachedVariable) cachedVariable = &env.getVariable(identifier);
			long long i = this->indexExpression->execute(env).value;
			(cachedVariable + i)->value = v;
		}
		virtual ExecutionResult execute(Environment& env) override
		{
			return ExecutionResult(ExecutionResult::Type::Normal, getValue(env));
		}
	};
	
	class ExpressionStatementNode : public StatementNode // expression statement, ex: ++i; or a = b + c;
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

	class VariableDeclarationNode : public StatementNode // variable and array declarations
	{
	public:
		enum class Type { VARIABLE, GLOBAL, INPUT, VARARRAY, GLOBALARRAY };
		Type type;
		string identifier;
		ExpressionNode* initializerExpression;
		VariableDeclarationNode(const string& identifier = "", ExpressionNode* n = nullptr) : identifier(identifier), initializerExpression(n) {}
		~VariableDeclarationNode() { delete initializerExpression; }
		virtual ExecutionResult execute(Environment& env) override
		{
			long long initialValue = 0;
			if (initializerExpression) { initialValue = initializerExpression->execute(env).value; }
			switch (type)
			{
			case Type::VARIABLE: // local variable
				env.pushVariable(identifier, initialValue); 
				return ExecutionResult(ExecutionResult::Type::VariableDeclaration, initialValue);
			case Type::GLOBAL: // global variable
				env.pushGlobal(identifier, initialValue); 
				return ExecutionResult(ExecutionResult::Type::GlobalDeclaration, initialValue);
			case Type::INPUT: // input variable
			{
				long long n;
				if (!(std::cin >> n)) // invalid input value, fall back to initialization value
				{
					n = initialValue; 
					cin.clear();
					cin.ignore(1);
				}
				cin.ignore(1);
				env.pushVariable(identifier, n);
				return ExecutionResult(ExecutionResult::Type::VariableDeclaration, initialValue);
			}
			case Type::VARARRAY: // local variable array
				if (initialValue <= 0) throw runtime_error("Run error: Invalid array size");
				for (int i = 0; i < initialValue; i++)
				{
					string elementIdentifier;
					if (i == 0) elementIdentifier = identifier;
					else elementIdentifier = identifier += to_string(i);
					env.pushVariable(elementIdentifier, 0);
				}
				return ExecutionResult(ExecutionResult::Type::ArrayDeclaration, initialValue);
			case Type::GLOBALARRAY: // local variable array
				if (initialValue <= 0) throw runtime_error("Run error: Invalid array size");
				for (int i = 0; i < initialValue; i++)
				{
					string elementIdentifier;
					if (i == 0) elementIdentifier = identifier;
					else elementIdentifier = identifier += to_string(i);
					env.pushGlobal(elementIdentifier, 0);
				}
				return ExecutionResult(ExecutionResult::Type::GlobalArrayDeclaration, initialValue);
			default: return ExecutionResult(ExecutionResult::Type::Normal);
			}
		}
	};	

	class PrintNode : public StatementNode // print statements
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

	class IfNode : public StatementNode // if statement
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

	class StatementListExpressionNode : public ExpressionNode // statement block expressions
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

	class WhileNode : public StatementNode // while, do-while loop
	{
	public:
		enum class Type { WHILE, DOWHILE };
		Type type;
		ExpressionNode* condition;
		StatementNode* body;
		WhileNode(Type t, ExpressionNode* c = nullptr, StatementNode* b = nullptr) 
			: type(t), condition(c), body(b) { }
		~WhileNode() { delete condition; delete body; }
		virtual ExecutionResult execute(Environment& env) override
		{
			ExecutionResult bodyExecution;
			switch (type)
			{
			case Type::WHILE:
				while (condition->execute(env).value)
				{
					bodyExecution = body->execute(env);
					if (bodyExecution.type == ExecutionResult::Type::Break) { break; }
					if (bodyExecution.type == ExecutionResult::Type::Continue) { continue; }
					if (bodyExecution.type == ExecutionResult::Type::Return) { return bodyExecution; }
				}
				break;
			case Type::DOWHILE:
				do
				{
					bodyExecution = body->execute(env);
					if (bodyExecution.type == ExecutionResult::Type::Break) { break; }
					if (bodyExecution.type == ExecutionResult::Type::Continue) { continue; }
					if (bodyExecution.type == ExecutionResult::Type::Return)
					{
						return bodyExecution;
					}
				} while (condition->execute(env).value);
				break;
			default: return ExecutionResult(ExecutionResult::Type::Normal);
			}
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};

	class ForNode : public StatementNode // for loop
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
			for (;condition->execute(env).value; update->execute(env))
			{
				ExecutionResult bodyExecution = body->execute(env);
				if (bodyExecution.type == ExecutionResult::Type::Break) { break; }
				if (bodyExecution.type == ExecutionResult::Type::Continue) { continue; }
				if (bodyExecution.type == ExecutionResult::Type::Return) 
				{ 
					clean(env);
					return bodyExecution;
				}
			}
			clean(env);
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
		void clean(Environment& env)
		{
			if (initialization) env.popVariables(1); // pop the for loop's initialization variable
		}
	};

	class RepeatNode : public StatementNode // repeat loop
	{
	public:
		ExpressionNode* iterations;
		StatementNode* body;
		RepeatNode() : iterations(nullptr), body(nullptr) { }
		~RepeatNode() { delete iterations; delete body; }
		virtual ExecutionResult execute(Environment& env) override
		{
			long long iterationCount = iterations->execute(env).value;
			if (iterationCount <= 0) return ExecutionResult(ExecutionResult::Type::Normal);
			for (long long i = 0; i < iterationCount; i++)
			{
				ExecutionResult execution = body->execute(env);
				if (execution.type == ExecutionResult::Type::Break) { break; }
				if (execution.type == ExecutionResult::Type::Continue) { continue; }
				if (execution.type == ExecutionResult::Type::Return)
				{
					return execution;
				}
			}
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};

	class JumpNode : public StatementNode // jump nodes (break, return)
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
			default: return ExecutionResult(ExecutionResult::Type::Normal);
			}
		}
	};

	class FunctionDeclarationNode : public StatementNode // function declaration
	{
	public:
		string identifier;
		DynamicList<string> parameters;
		StatementList* body;
		FunctionDeclarationNode() : body(nullptr) { }
		~FunctionDeclarationNode() {}
		virtual ExecutionResult execute(Environment& env) override
		{
			env.pushFunction(identifier, parameters, body);
			return ExecutionResult(ExecutionResult::Type::Normal);
		}
	};

	class CallNode : public ExpressionNode // function call
	{
	public:
		string identifier;
		DynamicList<ExpressionNode*> arguments;
		Environment::Function* cachedFunction; // for caching function pointer, to avoid searching env every time
		CallNode() : cachedFunction(nullptr) { }
		~CallNode() { for (ExpressionNode* n : arguments) delete n; }
		virtual ExecutionResult execute(Environment& env) override
		{
			if (!cachedFunction) cachedFunction = &env.getFunction(identifier, arguments.size()); // cache function pointer
			for (int i = 0; i < arguments.size(); i++) // push arguments
			{
				cachedFunction->environment->pushVariable(
					cachedFunction->parameters[i], 
					arguments[i]->execute(env).value);
			}
			ExecutionResult execution = ((AST::Node*)(cachedFunction->body))->execute(*(cachedFunction->environment));
			clean(env);
			return ExecutionResult(ExecutionResult::Type::Normal, execution.value);
		}
		void clean(Environment& env)
		{
			cachedFunction->environment->popVariables(arguments.size()); // pop arguments
		}
	};

	Node* root;
	AST() : root(nullptr) { }
	// execute the AST
	ExecutionResult execute(Environment& env)
	{
		return root->execute(env);
	}
};

