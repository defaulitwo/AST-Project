#pragma once

// just a value and enum pair which are used to transfer info and data between nodes
class ExecutionResult
{
public:
	enum class Type 
	{
		Normal, Null, Return, Break, Continue, VariableDeclaration, GlobalDeclaration,
		Error
	};
	
	Type type;
	long long value;
	
	ExecutionResult(Type t = Type::Normal, long long v = 0) : type(t), value(v) { }
};

class Error : public ExecutionResult
{
	string message;

	Error(const string& m) : message(m)
	{
		this->type = ExecutionResult::Type::Error;
	}
};