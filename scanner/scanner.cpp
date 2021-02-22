#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <set>
using namespace std;

vector<vector<string>> cfg;
set<string> terminals;
set<string> nonTerminals;
vector<string> rules;

/**
 * @brief Printing of default loaded file
 * 
 */
void printCFG()
{
    for (auto row : cfg)
    {
        for (string c : row)
        {
            cout << c << " ";
        }
        cout << endl;
    }
}

/**
 * @brief Helper function for printing terminals and non terminals
 * 
 */
void printTerminals()
{
    cout << "Terminals: ";
    for (auto it = terminals.begin(); it != terminals.end(); ++it)
    {
        cout << *it << ", ";
    }
    cout << endl;
    cout << "Non-terminals: ";
    for (auto it = nonTerminals.begin(); it != nonTerminals.end(); it++)
    {
        cout << *it << ", ";
    }
    cout << endl;
}

/**
 * @brief Helper for printing rules
 * 
 */
void printRules()
{
    for (int i = 0; i < rules.size(); i++)
    {
        cout << "(" << i + 1 << ")"
             << "   " << rules[i] << endl;
    }
    cout << "Grammar Start Symbol " << rules[0][0] << endl;
}

/**
 * @brief Read in file row at a time
 * 
 * @param filePath 
 */
void load_file(string filePath)
{
    string line;
    if (filePath == "/dev/null")
    {
        exit(1);
    }
    ifstream inputFile(filePath);
    if (!inputFile)
    {
        exit(1);
    }
    vector<string> entries;

    while (getline(inputFile, line))
    {
        vector<string> row;

        istringstream ss(line);
        for (string s; ss >> s;)
        {
            entries.push_back(s);
            row.push_back(s);
        }

        cfg.push_back(row);
    }
    inputFile.close();
}

/**
 * @brief Collect list of non terminal characters
 * 
 */
void findNonTerminals()
{
    for (auto row : cfg)
    {
        if (row[0] == "|")
        {
            continue;
        }
        else
        {
            nonTerminals.insert(row[0]);
        }
    }
}

/**
 * @brief Collect list of terminal characters
 * 
 */
void findTerminals()
{
    for (auto row : cfg)
    {
        for (auto element : row)
        {
            if (nonTerminals.count(element) == 0)
            {
                if (element != "->" & element != "$" & element != "|")
                {
                    terminals.insert(element);
                }
            }
        }
    }
}

/**
 * @brief Compute list of grammar rules handling the | operator
 * 
 */
void grammarRules()
{
    string currentNonTerminal;
    string rule;
    for (auto row : cfg)
    {
        rule = "";
        if (nonTerminals.count(row[0]) != 0)
        {
            // Its a non terminal
            currentNonTerminal = row[0];
            rule += currentNonTerminal;
            for (int i = 1; i < row.size(); i++)
            {
                if (row[i] != "|")
                {
                    rule += " ";
                    rule += row[i];
                }
                else
                {
                    rules.push_back(rule);
                    rule = currentNonTerminal + " -> ";
                }
            }
            rules.push_back(rule);
        }

        else if (row[0] == "|")
        {
            rule += currentNonTerminal;
            rule += " -> ";
            for (int i = 1; i < row.size(); i++)
            {
                if (row[i] != "|")
                {
                    rule += " ";
                    rule += row[i];
                }
                else
                {
                    rules.push_back(rule);
                    rule = currentNonTerminal + " -> ";
                }
            }
            rules.push_back(rule);
        }
    }
}

/**
 * @brief Placeholder for finding first set
 * 
 */
void firstSet()
{
}

/**
 * @brief Placeholder for finding follow set
 * 
 */
void followSet()
{
}

/**
 * @brief Scanning process order of operations
 * 
 */
void scan()
{
    findNonTerminals();
    findTerminals();
    printTerminals();
    grammarRules();
    printRules();
    firstSet();
    followSet();
}

/**
 * @brief Entry point of program
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[])
{
    string inputFile = "/home/christian/Documents/Compilers/scanner/assignment.cfg";
    load_file(inputFile);
    printCFG();
    scan();
}