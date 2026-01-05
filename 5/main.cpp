#include <iostream>
#include <fstream>
#include <iomanip>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "Environment.hpp"
using namespace std;

bool debug = false;

void execute(const string& input, Environment& env)
{
    // tokenizing the input string, generating the token list
    Tokenizer tokenizer(input);
    if (debug)
    {
        cout << "Tokenization result:" << endl;
        tokenizer.printTokensTo(cout);
        cout << endl;
    }

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
        cerr << endl << "\033[31m" << error.what() << "\033[0m" << endl;
    }
    cout << endl;
}

int main(int argc, char* argv[])
{
    srand(time(0));
    Environment env; // main environment

    if (argc >= 2) // file mode
    {
        ifstream file(argv[1]);
        if (!file.is_open())
        {
            cerr << "\033[31m" << "\nError: Could not read the file " << argv[1] << "\033[31m" << endl << endl;
            return 1;
        }

        string input;
        string line;
        while (getline(file, line))
        {
            input += line += '\n';
        }

        execute(input, env);
        file.close();
    }
    else // interactive mode
    {
        cout << endl << "INTERACTIVE MODE: Write code snippet and enter run" << endl << endl;
        // REPL
        DynamicList<string> lines;
        DynamicList<string> pastLines;
        while (true)
        {
            int lineNumber = 0;
            while (true)
            {
                cout << internal << setw(5) << lineNumber << setw(3) << "| ";
                bool refresh = false; // print lines again to user, possibly after a change
                string line;
                getline(cin, line);
                if (line.compare("run") == 0) break;
                else if (line.compare("exit") == 0) exit(0);
                else if (line.compare("help") == 0)
                {
                    // todo: add help
                }
                else if (line.compare("back") == 0)
                {
                    if (!lines.empty())
                    {
                        lines.pop();
                        lineNumber--;
                    }
                    refresh = true;
                }
                else if (line.compare("edit") == 0) // edit a previously entered line
                {
                    int n;
                    cout << "which line? ";
                    cin >> n;
                    cin.ignore();
                    if (n >= 0 && n < lines.size())
                    {
                        cout << "\033[" << lineNumber - n + 2 << "A";
                        cout << "\033[2K\r";
                        cout << internal << setw(5) << n << setw(3) << "| ";
                        string newEdit;
                        getline(cin, newEdit);
                        if (!(newEdit.compare("cancel") == 0)) lines[n] = newEdit;
                        cout << "\033[" << lineNumber - n + 1 << "B";
                    }
                    else
                    {
                        cout << "\033[31m" << "invalid line" << "\033[0m" << endl;
                    }
                    refresh = true;
                }
                else if (line.compare("recall") == 0) // recall last executed code
                {
                    while (!lines.empty()) lines.pop();
                    lines = pastLines;
                    lineNumber = pastLines.size();
                    refresh = true;
                }
                else if (line.compare("discard") == 0) // delete current snippet
                {
                    while (!lines.empty()) lines.pop();
                    lineNumber = 0;
                    cout << endl;
                }
                else if (line.compare("showtokens") == 0) // show tokenizer output
                {
                    debug = !debug;
                    refresh = true;
                }
                else // push new line
                {
                    lines.push(line);
                    lineNumber++;
                }

                if (refresh)
                {
                    //cout << "\033[H\033[2J";
                    cout << endl;
                    for (int i = 0; i < lines.size(); i++)
                    {
                        cout << internal << setw(5) << i << setw(3) << "| " << lines[i] << endl;
                    }
                }
            }
            
            string input;
            for (string line : lines)
            {
                input += line;
                input += '\n';
            }

            execute(input, env);

            pastLines = lines; // save last execution to be able to be recalled
            while (!lines.empty()) lines.pop(); // clear lines
        }
    }
}
