// abstract syntax tree
// this is the output of the parser stage after it parses the token list
// Node class is a generic virtual class that all specific node types inherit from
// each specific node type implements its own print and evaluation method, as it is different for all of them
// this structure also allows Node* to be a common interface for all node types, as used in Parser.hpp parsing functions

#pragma once
#include <string>
#include <ostream>
using namespace std;

class AST
{
public:
	class Node
	{
	public:
		enum class NodeType { VAL, ID, ADD, SUB, MUL, DIV, NEG, ASSIGN, };
		
		NodeType type;
		Node(NodeType t) : type(t) { }
		virtual ~Node() = default;
		virtual double evaluate() const = 0;
		virtual void print(ostream& out) const = 0;
	};

	Node* root = nullptr;

	class ValNode : public Node
	{
	public:
		double value;
		ValNode(double v = 0) : Node(NodeType::VAL), value(v) { }
		double evaluate() const override { return value; }
		void print(ostream& out) const override { out << value; }
	};

	class IdNode : public Node
	{
	public:
		string name;
		IdNode(const string& s) : Node(NodeType::ID), name(s) { }
		double evaluate() const override { /* code to look up the variable's value if known */ return 0 /* placeholder!! */; }
		void print(ostream& out) const override { out << name; }
	};

	class AddNode : public Node
	{
	public:
		Node* left;
		Node* right;
		AddNode(Node* l = nullptr, Node* r = nullptr) : Node(NodeType::ADD), left(l), right(r) { }
		double evaluate() const override { return left->evaluate() + right->evaluate(); }
		void print(ostream& out) const override { out << '('; left->print(out); out << '+'; right->print(out); out << ")"; }
	};

	class SubNode : public Node
	{
	public:
		Node* left;
		Node* right;
		SubNode(Node* l = nullptr, Node* r = nullptr) : Node(NodeType::SUB), left(l), right(r) { }
		double evaluate() const override { return left->evaluate() - right->evaluate(); }
		void print(ostream& out) const override { out << '('; left->print(out); out << '-'; right->print(out); out << ")"; }
	};

	class MulNode : public Node
	{
	public:
		Node* left;
		Node* right;
		MulNode(Node* l = nullptr, Node* r = nullptr) : Node(NodeType::MUL), left(l), right(r) { }
		double evaluate() const override { return left->evaluate() * right->evaluate(); }
		void print(ostream& out) const override { out << '('; left->print(out); out << '*'; right->print(out); out << ")"; }
	};
	
	class DivNode : public Node
	{
	public:
		Node* left;
		Node* right;
		DivNode(Node* l = nullptr, Node* r = nullptr) : Node(NodeType::DIV), left(l), right(r) { }
		double evaluate() const override { return left->evaluate() / right->evaluate(); }
		void print(ostream& out) const override { out << '('; left->print(out); out << '/'; right->print(out); out << ")"; }
	};

	class NegNode : public Node
	{
	public:
		Node* operand;
		NegNode(Node* o = nullptr) : Node(NodeType::NEG), operand(o) { }
		double evaluate() const override { return - operand->evaluate(); }
		void print(ostream& out) const override { out << '-'; operand->print(out); }
	};

	class AssignNode : public Node
	{
	public:
		IdNode* assignee;
		Node* right;
		AssignNode(IdNode* a = nullptr, Node* r = nullptr) : Node(NodeType::ASSIGN), assignee(a), right(r) { }
		double evaluate() const override { }
		void print(ostream& out) const override { }
	};

	void printExpressionTo(ostream& out) const
	{
		if (root)
			root->print(out);
		out << endl;
	}

	double evaluate() const
	{
		if (root)
			return root->evaluate();
		else return 0;
	}
};
