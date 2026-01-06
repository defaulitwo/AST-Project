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
		Variable() : identifier(""), value(0) { }
		Variable(const string& n, long long v = 0) : identifier(n), value(v) { }
	};

	class Function
	{
	public:
		string identifier;
		DynamicList<string> parameters;
		Environment* environment;
		void* body;
		Function() = default;
		Function(const string& i, DynamicList<string>& p, Environment* e, void* b)
			: identifier(i), parameters(p), environment(e), body(b) { }
		~Function() { }
	};

	DynamicList<Variable> variables; // local variables of environment
	DynamicList<Variable> globals; // global variables
	DynamicList<Function> functions; // functions

	Environment() = default;
	// push new local variable
	void pushVariable(const string& ident, long long initialValue) 
	{
		variables.push(Variable(ident, initialValue));
	}
	// push new global variable
	void pushGlobal(const string& ident, long long initialValue) 
	{
		// can't define a global with same name more than once
		for (Variable& v : globals) if (ident == v.identifier) return; 
		globals.push(Variable(ident, initialValue));
	}
	// pop n local variables
	void popVariables(int n) 
	{
		for (int i = 0; i < n; i++) if(!variables.empty()) variables.pop();
	}
	// fetch variable
	Variable& getVariable(const string& name) 
	{
		for (Variable& v : globals) // first check globals (globals have higher priority)
		{
			if (name.compare(v.identifier) == 0) return v;
		}
		for (int i = variables.size() - 1; i >= 0; i--) // traverse scope backwards
		{
			Variable& v = variables[i];
			if (name.compare(v.identifier) == 0) return v;
		}
		throw runtime_error("Run error: Identifier \"" + name + "\" is undefined");
	}
	// set variable value
	void setValue(const string& name, long long value)
	{
		getVariable(name).value = value;
	}
	// fetch address of variable
	long long getAddress(const string& name)
	{
		 return (long long)(&(getVariable(name).value));
	}
	// check if address is out of bounds
	void checkAddress(long long a)
	{
		if ((Variable*)a > variables.end() || (Variable*)a < variables.begin())
		{
			throw runtime_error("Run error: Out of bounds access attempt");
		}
	}
	// push new function
	void pushFunction(const string& name, DynamicList<string>& parameters, Environment* env, void* body)
	{
		/*for (Function& f : functions) if (name.compare(f.identifier) == 0 && f.parameters.size() == parameters.size()) throw runtime_error("test");*/
		functions.push(Function(name, parameters, env, body));
	}
	// fetch function
	Function& getFunction(const string& name, DynamicList<long long>& arguments)
	{
		for (Function& f : functions)
		{
			if (f.identifier.compare(name) == 0 && f.parameters.size() == arguments.size())
			{
				return f;
			}
		}
		throw runtime_error("Run error: Invalid function call \"" + name + "(...)\"");
	}
};

