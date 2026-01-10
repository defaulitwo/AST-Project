#pragma once

class ExecutionResult // a value and enum pair which are used to transfer info and data between nodes
{
public:
	enum class Type // note: some types are currently unused
	{
		Normal, Null, Return, Break, Continue, 
		VariableDeclaration, GlobalDeclaration, ArrayDeclaration, GlobalArrayDeclaration, FunctionDeclaration,
		Error
	};
	Type type;
	long long value;
	ExecutionResult(Type t = Type::Normal, long long v = 0) : type(t), value(v) { }
};

class Error : public ExecutionResult // also not currently used
{
	string message;
	Error(const string& m) : message(m)
	{
		this->type = ExecutionResult::Type::Error;
	}
};