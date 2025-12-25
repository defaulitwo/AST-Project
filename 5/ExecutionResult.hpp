#pragma once
class ExecutionResult
{
public:
	enum class Type {
		Normal, Return, Break, Continue
	};

	int value;

	Type type;
	ExecutionResult(Type t = Type::Normal, int v = 0) : type(t), value(v)
	{

	}
};

