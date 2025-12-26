#pragma once
class ExecutionResult
{
public:
	enum class Type {
		Normal, Null, Return, Break, Continue, VariableDeclaration, GlobalDeclaration
	};

	int value;
	Type type;
	
	ExecutionResult(Type t = Type::Normal, int v = 0) : type(t), value(v) { }
};

