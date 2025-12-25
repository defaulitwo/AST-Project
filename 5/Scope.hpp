#pragma once
#include <string>
#include <iostream>
#include "DynamicList.hpp"
using namespace std;

class Scope
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

	DynamicList<Variable> variables;

	Scope() = default;
	
	void pushVariable(const string& ident, int initialValue)
	{
		variables.push(Variable(ident, initialValue));

	}

	void pop(int n)
	{
		for (int i = 0; i < n; i++) if(!variables.empty()) variables.pop();
	}

	int getValue(const string& name)
	{
		for (int i = variables.size() - 1; i >= 0; i--) // traverse scope backwards
		{
			Variable& currentVar = variables[i];
			if (name.compare(currentVar.identifier) == 0) return currentVar.value;
		}
		return 0;
	}

	void setValue(const string& name, int v)
	{
		for (int i = variables.size() - 1; i >= 0; i--) // traverse scope backwards
		{
			Variable& currentVar = variables[i];
			if (name.compare(currentVar.identifier) == 0) currentVar.value = v;
		}
	}

};

