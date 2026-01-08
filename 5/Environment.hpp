// environment object, holds local variables, global variables and functions

#pragma once
#include <string>
#include <iostream>
#include "AST.hpp"
#include "DynamicList.hpp"

using namespace std;

class Environment
{
public:
	class Variable
	{
	public:
		string identifier;
		long long value;
		Variable() : identifier(""), value(0) {}
		Variable(const string& n, long long v = 0) : identifier(n), value(v) {}
	};

	class Function
	{
	public:
		string identifier;
		DynamicList<string> parameters;
		Environment* environment;
		void* body; // gets casted to AST::StatementListNode* when executed
		Function() = default;
		Function(const string& i, DynamicList<string>& p, Environment* e, void* b)
			: identifier(i), parameters(p), environment(e), body(b) {
		}
		~Function() { }
	};

	DynamicList<Variable>* variables = new DynamicList<Variable>(); // local variables of environment
	DynamicList<Variable>* globals = new DynamicList<Variable>(); // global variables
	DynamicList<Function>* functions = new DynamicList<Function>(); // functions

	Environment() = default;
	~Environment() { delete variables; delete globals; delete functions; }
	// push new local variable
	void pushVariable(const string& ident, long long initialValue)
	{
		variables->push(Variable(ident, initialValue));
	}
	// push new global variable
	void pushGlobal(const string& ident, long long initialValue)
	{
		// can't define a global with same name more than once
		for (Variable& v : *globals) if (ident == v.identifier) return;
		globals->push(Variable(ident, initialValue));
	}
	// pop n local variables
	void popVariables(int n)
	{
		for (int i = 0; i < n; i++) if (!variables->empty()) variables->pop();
	}
	// fetch variable
	Variable& getVariable(const string& name)
	{
		// although linear search is used here, which is not very fast, caching is used
		// in the AST, which compensates for this, 
		// searching needs to happen only once per variable lookup
		for (int i = variables->size() - 1; i >= 0; i--) // first traverse scope backwards
		{
			Variable& v = (*variables)[i];
			if (name.compare(v.identifier) == 0) return v;
		}
		for (Variable& v : *globals) // then check globals
		{
			if (name.compare(v.identifier) == 0) return v;
		}
		throw runtime_error("Run error: Identifier \"" + name + "\" is undefined");
	}
	// set variable value
	void setValue(const string& name, long long v)
	{
		getVariable(name).value = v;
	}
	// fetch address of variable
	long long getAddress(const string& name)
	{
		return (long long)(&(getVariable(name)));
	}
	// check if address is out of bounds
	void checkAddress(long long a)
	{
		if ((Variable*)a > variables->end() || (Variable*)a < variables->begin())
		{
			throw runtime_error("Run error: Out of bounds access attempt");
		}
	}
	// push new function
	void pushFunction(const string& name, DynamicList<string>& parameters, Environment* env, void* body, Environment& callerEnv)
	{
		for (Function& f : *functions) if (name.compare(f.identifier) == 0 && f.parameters.size() == parameters.size()) 
			throw runtime_error("Run error: Function with same signature is already defined");
		Function f = Function(name, parameters, env, body);
		// functions share the same environment globals and functions with the caller
		f.environment->globals = callerEnv.globals;
		f.environment->functions = callerEnv.functions;
		functions->push(f);
	}
	// fetch function
	Function& getFunction(const string& name, int argCount)
	{
		for (Function& f : *functions)
		{
			if (f.identifier.compare(name) == 0 && f.parameters.size() == argCount)
			{
				return f;
			}
		}
		throw runtime_error("Run error: Invalid function call \"" + name + "( " + to_string(argCount) + " arguments )\"");
	}
};

