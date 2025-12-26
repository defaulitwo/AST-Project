#include <iostream>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "Scope.hpp"
using namespace std;

int main()
{
    Scope scope;

    while (true)
    {
        // getting input string from user
        string input;
        while(true)
        {
            cout << "> ";
            string line;
            getline(cin, line);
            if (line.compare("run") == 0) break;
            else if (line.compare("exit") == 0) exit(0);
            else
            {
                input += line;
                input += "\n";
            }
        }

        // tokenizing the input string, generating the token list
        Tokenizer tokenizer(input);
        //cout << "Tokenization result:" << endl;
        //tokenizer.printTokensTo(cout);
        //cout << endl;

        // parsing the token list, building AST
        try 
        {
            Parser parser;
            AST tree = parser.Parse(tokenizer);

            tree.execute(scope);

            cout << endl;
        }
        catch (runtime_error error)
        {
            cout << "Error parsing: " << error.what() << endl;
        }
    }
}
