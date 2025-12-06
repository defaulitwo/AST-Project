// abstract syntax tree
// this is the output of the parser stage after it parses the token list
// Node class is a generic virtual class that all specific node types inherit from
// each specific node type implements its own print and evaluation method, as it is different for all of them
// this structure also allows Node* to be a common interface for all node types, as used in Parser.hpp parsing functions

#pragma once
#include <string>
#include <ostream>
#include <vector>
#include "Tokenizer.hpp"
using namespace std;

class AST
{
public:
	class Node
	{
	public:
		Node() = default;
		virtual ~Node() = default;
		virtual bool evaluate(AST& tree, int i) const = 0;
		virtual void print(AST& tree, ostream& out) const = 0;
	};

	class StatementList : public Node
	{
	public:
		vector<Node*> statements;
		StatementList() { }
		~StatementList() { for (Node* statement : statements) delete statement; }
		virtual bool evaluate(AST& tree, int i) const
		{
			return false;
			for (Node* statement : statements)
			{
				// TODO
			}
		};
		virtual void print(AST& tree, ostream& out) const
		{ 
			out << "Statement List\n|" << endl;
			for (int i = 0; i < statements.size(); i++)
			{
				out << "|_ ";
				if (i == statements.size() - 1) out << "therefore ";
				statements[i]->print(tree, out);
				out << endl << "|" << endl;
			}
		};
	};

	class BinaryNode : public Node
	{
	public:
		Node* left;
		Node* right;
		BinaryNode(Node* l = nullptr, Node* r = nullptr) : left(l), right(r) { }
		virtual bool evaluate(AST& tree, int i) const = 0;
		virtual void print(AST& tree, ostream& out) const = 0;
		~BinaryNode() { delete left; delete right; }
	};

	class ConditionalNode : public BinaryNode
	{
	public:
		ConditionalNode(Node* l = nullptr, Node* r = nullptr) : BinaryNode(l, r) { }
		virtual bool evaluate(AST& tree, int i) const override
		{ 
			return !(left->evaluate(tree, i)) || (right->evaluate(tree, i)); 
		}
		void print(AST& tree, ostream& out) const override
		{ 
			left->print(tree, out);
			out << " -> ";
			right->print(tree, out);
		}
	};

	class AndNode : public BinaryNode
	{
	public:
		AndNode(Node* l = nullptr, Node* r = nullptr) : BinaryNode(l, r) { }
		virtual bool evaluate(AST& tree, int i) const override
		{ 
			return (left->evaluate(tree, i)) && (right->evaluate(tree, i)); 
		}
		void print(AST& tree, ostream& out) const override
		{
			out << "( ";
			left->print(tree, out);
			out << " AND ";
			right->print(tree, out);
			out << " )";
		}
	};

	class OrNode : public BinaryNode
	{
	public:
		OrNode(Node* l = nullptr, Node* r = nullptr) : BinaryNode(l, r) { }
		virtual bool evaluate(AST& tree, int i) const override { return left->evaluate(tree, i) || right->evaluate(tree, i); }
		void print(AST& tree, ostream& out) const override
		{
			out << "( ";
			left->print(tree, out);
			out << " OR ";
			right->print(tree, out);
			out << " )";
		}
	};

	class NotNode : public Node
	{
	public:
		Node* operand;
		NotNode(Node* o = nullptr) : operand(o) { }
		~NotNode() { delete operand; }
		virtual bool evaluate(AST& tree, int i) const override { return !(operand->evaluate(tree, i)); }
		void print(AST& tree, ostream& out) const override
		{
			out << "NOT( ";
			operand->print(tree, out);
			out << " )";
		}
	};

	class VariableNode : public Node
	{
	public:
		int varIndex;
		VariableNode(int v) : varIndex(v) { }
		virtual bool evaluate(AST& tree, int i) const override { return tree.truthTable[i][varIndex]; }
		void print(AST& tree, ostream& out) const override { out << tree.variableList[varIndex]; }
	};

	StatementList* root;
	const Tokenizer& tokenizer;
	vector<string> variableList;
	bool** truthTable;
	bool** premiseAndConclusionTable;
	vector<int> criticalRows;

	AST(const Tokenizer& tokenizer) : root(nullptr), truthTable(nullptr), premiseAndConclusionTable(nullptr), tokenizer(tokenizer)
	{
		truthTable = tokenizer.truthTable;
		variableList = tokenizer.variableList;
	}

	void buildPremiseAndConclusionTable()
	{
		premiseAndConclusionTable = new bool* [pow(2, variableList.size())];
		
		for (int i = 0; i < pow(2, variableList.size()); i++)
		{
			premiseAndConclusionTable[i] = new bool[root->statements.size()];
		}

		for (int i = 0; i < pow(2, variableList.size()); i++)
		{
			for (int j = 0; j < root->statements.size(); j++)
				premiseAndConclusionTable[i][j] = root->statements[j]->evaluate(*this, i);
		}
	}

	bool isSatisfiable() const
	{
		bool satisfiable;
		for (int i = 0; i < pow(2, variableList.size()); i++)
		{
			satisfiable = true;
			for (int j = 0; j < root->statements.size(); j++)
			{
				if (premiseAndConclusionTable[i][j] == false) satisfiable = false;
			}
			if (satisfiable) return true;
		}
		return false;
	}

	bool isValid() const
	{
		for (int i = 0; i < pow(2, variableList.size()); i++)
		{
			bool criticalRow = true;
			for (int j = 0; j < root->statements.size() - 1; j++)
			{
				if (premiseAndConclusionTable[i][j] == false) criticalRow = false;
			}
			if (criticalRow && premiseAndConclusionTable[i][root->statements.size() - 1] == false)
				return false;
		}
		return true;
	}

	void printExpressionTo(ostream& out)
	{
		if (root)
			root->print(*this, out);
		out << endl;
	}
};
