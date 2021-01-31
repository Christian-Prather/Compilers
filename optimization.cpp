/**
 * @file optimization.cpp
 * @author Christian Prather
 * @brief This is a simple dfa optimization script, 
 * Steps:
 *  1) Load in file from argumnets
 *  2) Save the DFA in a 2D vector?
 *  3) Start at state[0][0] -> row zero column zero 
 *  4) Make an empy set 
 *  5) 2D stack L of (accepting states, alphabet) then non (accepting states, alphabet)
 *  6) Pop stack S,C 
 *  7) Get char c from C
 *  8) x1 set = states with transition for c
 *  9) x2 set = states that remain after x1
 *  10) check if char c was the last one then add S to M
 *  11) else push (x2, C) back onto L if x2 > 1
 *  12) Run until number of states in dfa are the same
 * @version 0.1
 * @date 2021-01-30
 * 
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <tuple>
#include <stack>

using namespace std;

struct Grouping
{
    set<int> states;
    set<int> alphabet; // Not the alphabet is a numercal column value
};

stack <Grouping> L;

vector<vector<char>> dfa;

void load_file(string filePath)
{
    string line;
    ifstream inputFile(filePath);
    while (getline(inputFile, line))
    {
        // cout << line << endl;
        vector<char> row;
        for (char c: line)
        {
            if (c != ' ')
            {
                // Not a space add to matrix
                row.push_back(c);
            }
        }
        dfa.push_back(row);

    }
    inputFile.close();
}

void printDFA()
{
    for (auto row: dfa)
    {
        for (char c: row)
        {
            cout << c << " ";
        }
        cout << endl;
    }
}

void initialize()
{
    Grouping acceptinStates;
    Grouping nonAcceptingStates;
    for (auto row: dfa)
    {
        // Check if first char is + (accepting state)
        if (row[0] == '+')
        {
            // Add state id
            acceptinStates.states.insert(row[1]); 
        }
        else
        {
            nonAcceptingStates.states.insert(row[1]);
        }
    }
    // Add sudo alphabet to both
    for (int i = 0; i < dfa[0].size() -2; i++)
    {
        acceptinStates.alphabet.insert(i);
        nonAcceptingStates.alphabet.insert(i);
    }
    L.push(acceptinStates);
    L.push(nonAcceptingStates);
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "No input file given please provide \
        absolute path to file" << endl;
        exit(1);
    }
    load_file(argv[1]);
    printDFA(); 
}
