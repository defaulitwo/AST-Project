#include <iostream>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
using namespace std;

int main()
{
    while (true)
    {
        // getting input string from user
        string input;
        cout << "> ";
        getline(cin, input);
        if (input.compare("exit") == 0 || input.compare("quit") == 0) exit(0);
        cout << endl;

        // tokenizing the input string, generating the token list
        Tokenizer tokenizer(input);
        cout << "Tokenization result:" << endl;
        tokenizer.printTokensTo(cout);
        cout << endl;

        // printing detected variables
        cout << "Variables:" << endl;
        for (string s : tokenizer.variableList) cout << s << " ";
        cout << endl;
        cout << endl;

        // printing generated truth table
        cout << "Truth table:" << endl;
        for (int i = 0; i < pow(2, tokenizer.variableList.size()); i++)
        {
            for (int j = 0; j < tokenizer.variableList.size(); j++)
            {
                cout << tokenizer.truthTable[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;

        // parsing the token list, building AST
        try 
        {
            Parser parser(tokenizer);
            cout << "Parser result tree:" << endl;
            parser.resultTree.printExpressionTo(cout);

            parser.resultTree.buildPremiseAndConclusionTable();

            cout << "Premise and conclusion table:" << endl;
            for (int i = 0; i < pow(2, tokenizer.variableList.size()); i++)
            {
                for (int j = 0; j < parser.resultTree.root->statements.size(); j++)
                {
                    cout << parser.resultTree.premiseAndConclusionTable[i][j] << " ";
                    if (j == parser.resultTree.root->statements.size() - 2) cout << "\t";
                }
                cout << endl;
            }
            cout << endl;

            // satisfiability and validity
            if (parser.resultTree.isSatisfiable()) cout << "Satisfiable." << endl;
            else cout << "Not satisfiable." << endl;

            cout << endl;

            if (parser.resultTree.isValid()) cout << "Valid argument." << endl;
            else cout << "Falsifiable argument." << endl;
        }
        catch (runtime_error error)
        {
            cout << "Error parsing: " << error.what() << endl;
        }
    }
}
