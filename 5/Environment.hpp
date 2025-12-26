#pragma once
#include <string>
#include <iostream>
#include "DynamicList.hpp"
#include "AST.hpp"
using namespace std;

class Environment
{
public:
	class Variable
	{
	public:
		string identifier;
		int value;
		// note, do not allow nameless variables in constructor, by design
		Variable() = default;
		Variable(const string& n, int v = 0) : identifier(n), value(v) { }
	};

	class Function
	{
	public:
		string identifier;
		//AST root;
	};

	DynamicList<Variable> variables;
	DynamicList<Variable> globals;

	Environment() = default;
	
	void pushVariable(const string& ident, int initialValue)
	{
		variables.push(Variable(ident, initialValue));
	}

	void pushGlobal(const string& ident, int initialValue)
	{
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
		return 0;
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
	}
};

