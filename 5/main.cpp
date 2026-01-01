#include <iostream>
#include <fstream>
#include <iomanip>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "Environment.hpp"
using namespace std;

void execute(const string& input, Environment& env)
{
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
            // getting input string from user

            int lineNumber = 0;
            while (true)
            {
                cout << internal << setw(5) << lineNumber << setw(3) << "| ";
                string line;
                getline(cin, line);
                if (line.compare("run") == 0) break;
                else if (line.compare("exit") == 0) exit(0);
                else if (line.compare("help") == 0)
                {

                    // !!! PLACEHOLDER, ADD HELP LATER !!!

                    for (int i = 0; i < lines.size(); i++)
                    {
                        cout << internal << setw(5) << i << setw(3) << "| " << lines[i] << endl;
                    }
                }
                else if (line.compare("back") == 0)
                {
                    if (!lines.empty())
                    {
                        lines.pop();
                        lineNumber--;
                    }

                    cout << endl;
                    for (int i = 0; i < lines.size(); i++)
                    {
                        cout << internal << setw(5) << i << setw(3) << "| " << lines[i] << endl;
                    }
                }
                else if (line.compare("edit") == 0)
                {
                    int n;
                    cout << "which line? ";
                    cin >> n;
                    cin.ignore();
                    if (n >= 0 && n < lines.size())
                    {
                        cout << endl;
                        for (int i = 0; i < n; i++)
                        {
                            cout << internal << setw(5) << i << setw(3) << "| " << lines[i] << endl;
                        }
                        cout << endl;
                        int count = 1;
                        for (int i = n + 1; i < lines.size(); i++)
                        {
                            count++;
                            cout << internal << setw(5) << i << setw(3) << "| " << lines[i] << endl;
                        }
                        cout << "\033[" << count << "A";
                        cout << internal << setw(5) << n << setw(3) << "| ";
                        string newEdit;
                        getline(cin, newEdit);
                        if (!(newEdit.compare("cancel") == 0)) lines[n] = newEdit;
                        cout << "\033[" << count << "B";
                    }
                    else
                    {
                        cout << "\033[31m" << "invalid line" << "\033[0m" << endl;
                    }

                    cout << endl;
                    for (int i = 0; i < lines.size(); i++)
                    {
                        cout << internal << setw(5) << i << setw(3) << "| " << lines[i] << endl;
                    }
                }
                else if (line.compare("recall") == 0) // recall last executed code
                {
                    while (!lines.empty()) lines.pop();
                    lineNumber = 0;
                    lines = pastLines;
                    cout << endl;
                    for (int i = 0; i < lines.size(); i++)
                    {
                        cout << internal << setw(5) << i << setw(3) << "| " << lines[i] << endl;
                        lineNumber++;
                    }
                }
                else if (line.compare("discard") == 0) // delete current snippet
                {
                    while (!lines.empty()) lines.pop();
                    lineNumber = 0;
                    cout << endl;
                }
                else
                {
                    lines.push(line);
                    lineNumber++;
                }
            }
            
            string input;
            for (string line : lines)
            {
                input += line;
                input += '\n';
            }

            execute(input, env);

            pastLines = lines;
            while (!lines.empty()) lines.pop();
        }
    }
}
