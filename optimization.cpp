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
    vector<int> states;
    vector<int> alphabet; // Not the alphabet is a numercal column value
};

stack<Grouping> L;

vector<vector<char>> dfa;

void load_file(string filePath)
{
    string line;
    ifstream inputFile(filePath);
    while (getline(inputFile, line))
    {
        // cout << line << endl;
        vector<char> row;
        for (char c : line)
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
    for (auto row : dfa)
    {
        for (char c : row)
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
    for (auto row : dfa)
    {
        // Check if first char is + (accepting state)
        if (row[0] == '+')
        {
            // Add state id
            acceptinStates.states.push_back(row[1] - 48);
        }
        else
        {
            nonAcceptingStates.states.push_back(row[1] - 48);
        }
    }
    // Add sudo alphabet to both
    for (int i = 0; i < dfa[0].size() - 2; i++)
    {
        acceptinStates.alphabet.push_back(i);
        nonAcceptingStates.alphabet.push_back(i);
    }
    L.push(acceptinStates);
    L.push(nonAcceptingStates);
}

void seg()
{
    Grouping potential = L.top();
    L.pop();
    vector<int> states = potential.states;
    vector<int> alphabet = potential.alphabet;
    cout << "Top poped" << endl;
    // Iterate over every row in dfa if its in states and see if its column (c)
    // has an entry
    // If so make a set
    // if not add to empty set

    // Get first letter in set alphabet and remove it from set
    int letter = alphabet[0];
    alphabet.erase(alphabet.begin());
    vector<int> notTransitions;
    cout << "Initial letter..." << endl;
    struct matches
    {
        int id;
        vector<int> stateIds;
    };
    cout << "matches" << endl;
    vector<matches> pastMatches;
    pastMatches.clear();

    for (int state : states)
    {
        cout << "State.." << state << endl;
        vector<char> row = dfa[state];
        if (row[letter] == 'E')
        {
            // Not a transition
            notTransitions.push_back(state);
        }
        else
        {
            cout << "Not seen before..." << pastMatches.size() << endl;
            bool seen = false;
            //TODO:  Add to a set with the same row[letter]
            for (matches match : pastMatches)
            {
                if (match.id == row[letter])
                {
                    match.stateIds.push_back(state);
                    seen = true;
                    break;
                }
            }
            if (!seen)
            {
                matches newMatch;
                newMatch.id = row[letter];
                newMatch.stateIds.push_back(state);
                pastMatches.push_back(newMatch);
            }
        }
    }
    // At this point should have all our partitions
    // For all partitions Xi (transitions, nonTransistions)
    for (auto partition : pastMatches)
    {
        if (partition.stateIds.size() > 1)
        {
            if (alphabet.size() == 0)
            {
                // add S to M
            }
            else
            {
                Grouping temp;
                temp.states = partition.stateIds;
                temp.alphabet = alphabet;
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "No input file given please provide \
        absolute path to file"
             << endl;
        exit(1);
    }
    load_file(argv[1]);
    printDFA();
    initialize();
    while (!L.empty())
    {
        seg();
        cout << "Run.." << endl;
    }
}
