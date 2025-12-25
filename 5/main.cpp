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
        //cout << endl;

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
            tree.execute();
        }
        catch (runtime_error error)
        {
            cout << "Error parsing: " << error.what() << endl;
        }
    }
}
