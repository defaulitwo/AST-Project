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
		int value;
		Variable() = default;
		Variable(const string& n, int v = 0) : identifier(n), value(v) { }
	};

	class Function
	{
	public:
		string identifier;
		DynamicList<string> parameters;
		Function() = default;
		Function(const string& i, DynamicList<string>& p)
			: identifier(i), parameters(p) { }
		~Function() { }
	};

	DynamicList<Variable> variables;
	DynamicList<Variable> globals;
	DynamicList<Function> functions;

	Environment() = default;
	
	void pushVariable(const string& ident, int initialValue)
	{
		variables.push(Variable(ident, initialValue));
	}

	void pushGlobal(const string& ident, int initialValue)
	{
		for (Variable& v : globals) if (ident == v.identifier) return; // can't define a global with same name more than once
		globals.push(Variable(ident, initialValue));
	}

	void popVariables(int n)
	{
		for (int i = 0; i < n; i++) if(!variables.empty()) variables.pop();
	}

	int getValue(const string& name)
	{
		for (Variable& v : globals) // first check globals (globals have higher priority)
		{
			if (name.compare(v.identifier) == 0) return v.value;
		}
		for (int i = variables.size() - 1; i >= 0; i--) // traverse scope backwards
		{
			Variable& v = variables[i];
			if (name.compare(v.identifier) == 0) return v.value;
		}
		throw runtime_error("Run error: Identifier \"" + name + "\" is undefined");
	}

	void setValue(const string& name, int value)
	{
		for (Variable& v : globals) // first check globals (globals have higher priority)
		{
			if (name.compare(v.identifier) == 0) { v.value = value; return; }
		}
		for (int i = variables.size() - 1; i >= 0; i--) // traverse scope backwards
		{
			Variable& v = variables[i];
			if (name.compare(v.identifier) == 0) { v.value = value; return; }
		}
		throw runtime_error("Run error: Identifier \"" + name + "\" is undefined");
	}

	void pushFunction(const string& name, DynamicList<string>& parameters)
	{
		for (Function& f : functions) if (name.compare(f.identifier) == 0 && f.parameters.size() == parameters.size()) throw runtime_error("test");
		functions.push(Function(name, parameters));
	}

	Function& getFunction(const string& name, DynamicList<int>& arguments)
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

