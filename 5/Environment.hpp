// environment object, holds local variables, global variables and functions
// and pointer to parent environment, acting as a linked list 

#pragma once
#include <string>
#include <iostream>
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
		Environment* environment; // every function gets its own environment
		void* body; // root of function body AST, gets casted to AST::StatementListNode* when executed
		Function() = default;
		Function(const string& i, DynamicList<string>& p, Environment* e, void* b)
			: identifier(i), parameters(p), environment(e), body(b) {
		}
		~Function() { }
	};

	Environment* parent; // pointer to parent environment, acts as linked list

	DynamicList<Variable> variables = DynamicList<Variable>(10);// local variables
	DynamicList<Variable> globals	= DynamicList<Variable>(10);// global variables
	DynamicList<Function> functions = DynamicList<Function>(10);// functions

	Environment() : parent(nullptr) { };
	~Environment() { }
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
		//for (int i = 0; i < n; i++) if (!variables.empty()) variables.pop();
		variables.popMultiple(n);
	}
	// fetch variable
	Variable& getVariable(const string& name)
	{
		// although linear search is used here, which is not very fast, caching pointers 
		// is used sometimes in the AST, which helps compensate for this
		// searching needs to happen only once per variable lookup if cached
		for (int i = variables.size() - 1; i >= 0; i--) // first traverse scope BACKWARDS
		{
			Variable& v = variables[i];
			if (name.compare(v.identifier) == 0) return v;
		}
		for (Variable& v : globals) // then check globals
		{
			if (name.compare(v.identifier) == 0) return v;
		}
		if (parent) return parent->getVariable(name); // not found in current environment, search in parent environment
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
	// check if address is out of bounds // REMOVED FOR NOW
	//void checkAddress(Variable* addr)
	//{
	//	if (addr > variables.end() || addr < variables.begin())
	//	{
	//		throw runtime_error("Run error: Out of bounds access attempt");
	//	}
	//}
	// push new function
	void pushFunction(const string& name, DynamicList<string>& parameters, void* body)
	{
		for (Function& f : functions) if (f.parameters.size() == parameters.size() && name.compare(f.identifier) == 0)
			return; // can't define function with same signature more than once, ignore
		Function f = Function(name, parameters, new Environment, body);
		f.environment->parent = this; // set parent to current environment
		functions.push(f);
	}
	// fetch function
	Function& getFunction(const string& name, int argCount)
	{
		// although linear search is used here, which is not very fast, caching pointers 
		// is used sometimes in the AST, which helps compensate for this
		// searching needs to happen only ONCE per function lookup if cached
		for (Function& f : functions)
		{
			if (f.parameters.size() == argCount && f.identifier.compare(name) == 0)
			{
				return f;
			}
		}
		if (parent) return parent->getFunction(name, argCount); // not found in current environment, search in parent
		throw runtime_error("Run error: Function signature \"" + name + "( " + to_string(argCount) + " arguments )\" is undefined.");
	}
};

