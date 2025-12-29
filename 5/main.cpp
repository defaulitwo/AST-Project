#include <iostream>
#include <iomanip>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "Environment.hpp"
using namespace std;

int main(int argc, char* argv[])
{
    srand(time(0));
    Environment env;

    // REPL
    while (true)
    {
        // getting input string from user
        string input;
        int lineNumber = 0;
        while(true)
        {
            cout << internal << setw(5) << lineNumber << setw(3) << "| ";
            string line;
            getline(cin, line);
            if (line.compare("run") == 0) break;
            else if (line.compare("exit") == 0) exit(0);
            else
            {
                input += line;
                input += '\n';
            }
            lineNumber++;
        }

        //cout << input;

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
            ExecutionResult execution = tree.execute(env);
            if (execution.type == ExecutionResult::Type::Return)
            {
                cout << endl << "Program returned " << execution.value;
            }
        }
        catch (runtime_error error)
        {
            cout << endl << error.what();
        }
        cout << endl;
    }
}
