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
#include <sstream>

using namespace std;

struct Grouping
{
    vector<int> states;
    vector<int> alphabet; // Not the alphabet is a numercal column value
};

struct matches
{
    int id;
    vector<int> stateIds;
};
stack<Grouping> L;
vector<vector<int>> M;

vector<vector<string>> dfa;

void load_file(string filePath)
{
    string line;
    ifstream inputFile(filePath);
    vector<string> entries;
    while (getline(inputFile, line))
    {
        // cout << line << endl;
        vector<string> row;

        istringstream ss(line);
        for (string s; ss >> s;)
        {
            entries.push_back(s);
            row.push_back(s);
        }

        // vector<char> row;
        // char entry;
        // for (char c : line)
        // {
        //     if (c != ' ')
        //     {
        //         // Not a space add to matrix
        //         entry = entry + c;
        //     }
        // }
        // cout << entry << " " << endl;
        dfa.push_back(row);
    }
    // for (auto thing: entries)
    // {
    //     cout << thing << " ";
    // }

    inputFile.close();
}

void printDFA()
{
    for (auto row : dfa)
    {
        for (string c : row)
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
        if (row[0] == "+")
        {
            // Add state id
            acceptinStates.states.push_back(stoi(row[1]));
        }
        else
        {
            nonAcceptingStates.states.push_back(stoi(row[1]));
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

void merging()
{
    cout << "M: " << M.size() << endl;
    // Merge things...
    for (vector<int> subset : M)
    {
        dfa.erase(dfa.begin() + (subset[1] - 1));
        for (auto row : dfa)
        {
            for (int i = 2; i < row.size(); i++)
            {
                // cout << "Element " << row[i] << endl;
                if (row[i] != "E")
                {
                    if (stoi(row[i]) == subset[1])
                    {
                        subset[1] = subset[0];
                    }
                }
            }
        }
    }
}

void seg()
{
    Grouping potential = L.top();
    L.pop();
    vector<int> states = potential.states;
    vector<int> alphabet = potential.alphabet;
    cout << "Top poped" << endl;
    cout << "Alph len " << alphabet.size() << endl;
    // Iterate over every row in dfa if its in states and see if its column (c)
    // has an entry
    // If so make a set
    // if not add to empty set

    // Get first letter in set alphabet and remove it from set
    int letter = alphabet[0] + 2;
    alphabet.erase(alphabet.begin());
    vector<int> notTransitions;
    cout << "Initial letter..." << letter << endl;

    vector<matches> pastMatches;
    pastMatches.clear();

    for (int state : states)
    {
        cout << "State.." << state << endl;
        vector<string> row = dfa[state];
        if (row[letter] == "E")
        {
            cout << "Error transition" << endl;
            // Not a transition
            notTransitions.push_back(state);
        }
        else
        {
            cout << "Valid Transition..." << pastMatches.size() << endl;
            bool seen = false;
            for (matches &match : pastMatches)
            {
                if (match.id == stoi(row[letter]))
                {
                    cout << "Seen before ... " << match.id << row[letter] << endl;
                    match.stateIds.push_back(state);
                    seen = true;
                    cout << "match len " << match.stateIds.size() << endl;
                    // pastMatches.push_back(match);
                    break;
                }
            }
            if (!seen)
            {
                cout << "Not seen before " << row[letter] << endl;
                matches newMatch;
                newMatch.id = stoi(row[letter]);
                newMatch.stateIds.push_back(state);
                pastMatches.push_back(newMatch);
            }
        }
    }
    // At this point should have all our partitions
    // For all partitions Xi (transitions, nonTransistions)
    for (matches partition : pastMatches)
    {
        cout << "PARTITION:" << endl;
        cout << partition.id << endl;
        cout << partition.stateIds.size() << endl;
        if (partition.stateIds.size() > 1)
        {
            cout << "Greater " << endl;
            if (alphabet.size() == 0)
            {
                cout << "Alphabet size 0" << endl;
                // add S to M
                M.push_back(partition.stateIds);
            }
            // else
            // {
            //     cout << "Adding a set back to L " << endl;
            //     Grouping temp;
            //     temp.states = partition.stateIds;
            //     temp.alphabet = alphabet;
            //     L.push(temp);
            // }
        }
    }
    if (notTransitions.size() > 1)
    {
        if (alphabet.size() != 0)
        {
            cout << "Pushing non transition set back to L" << notTransitions.size() << endl;
            Grouping temp;
            temp.alphabet = alphabet;
            temp.states = notTransitions;
            L.push(temp);
        }
    }

    // merging();
    // cout << "Otimized...." << endl;
    // printDFA();
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
    merging();
}
