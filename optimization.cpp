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
#include <map>

using namespace std;

struct TransistionRow
{
    bool isStart;
    bool isAccept;
    vector<string> states;
    map<string, vector<string>> transitions;
};

vector<TransistionRow> transitionTable;

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
vector<Grouping> L;
vector<vector<int>> M;

vector<vector<string>> dfa;
vector<vector<string>> nfa;

vector<string> firstRow;
vector<string> acceptingStates;
vector<string> alphabet;
vector<string> emptySet = {"E"};

void load_file(string filePath)
{
    string line;
    ifstream inputFile(filePath);
    vector<string> entries;
    // bool firstLine = true;

    while (getline(inputFile, line))
    {
        // // Ge the first line
        // if (firstLine)
        // {

        //     istringstream ss(line);
        //     for (string s; ss >> s;)
        //     {
        //         firstRow.push_back(s);
        //         firstLine = false;
        //         continue;
        //     }
        // }
        // cout << line << endl;
        vector<string> row;

        istringstream ss(line);
        for (string s; ss >> s;)
        {
            entries.push_back(s);
            row.push_back(s);
        }

        // // Got next line now make the formatted row
        // vector<string> formattedRow;
        // formattedRow.push_back(row[0]);
        // formattedRow.push_back(row[1]);
        // for (int i = 1; i < firstRow.size(); i++)
        // {
        //     if (row[3] == firstRow[i])
        //     {
        //         // Mathcing symbol
        //         formattedRow.push_back(row[2]);
        //     }
        //     else
        //     {
        //         // Put an E
        //         formattedRow.push_back("E");
        //     }

        // }

        nfa.push_back(row);
        // dfa.push_back(formattedRow);
    }
    inputFile.close();
}

vector<int> findRow(string id)
{
    vector<int> indexes;
    for (int i = 0; i < dfa.size(); i++)
    {
        auto row = dfa[i];
        if (row[1] == id)
        {
            indexes.push_back(i);
        }
    }
    return indexes;
}

void followLabda(vector<string> &S)
{
    vector<string> lambdaM;

    for (auto id : S)
    {
        lambdaM.push_back(id);
    }

    for (auto id : lambdaM)
    {
        vector<int> indexes = findRow(id);
        for (auto index : indexes)
        {
            auto transition = dfa[index];
            if (transition[3] == firstRow[1])
            {
                // Lambda add the to state id to M and eye
                // lambdaM may be an issue adding to it like this
                // may need to convert it to a true stack
                S.push_back(transition[2]);
                lambdaM.push_back(transition[2]);
            }
        }
    }
}

vector<string> followChar(vector<string> S, string c)
{
    vector<string> F;
    for (auto state : S)
    {
        vector<int> indexes = findRow(state);
        for (auto index : indexes)
        {
            auto row = nfa[index];
            if (row[3] == firstRow[1])
            {
                F.push_back(row[2]);
            }
        }
    }
    return F;
}
void findAcceptingStates()
{
    for (auto row : nfa)
    {
        if (row[0] == "+")
        {
            // Accepting state
            acceptingStates.push_back(row[1]);
        }
    }
}

bool isAccept(vector<string> B)
{
    for (auto id : B)
    {
        for (auto acceptingOption : acceptingStates)
        {
            if (id == acceptingOption)
            {
                return true;
            }
        }
    }
    return false;
}
void parseAlphabet()
{
    for (int i = 1; i < firstRow.size(); i++)
    {
        alphabet.push_back(firstRow[i]);
    }
}
bool checkEntryRow(vector<string> entry, vector<string> R)
{
    for (auto state : entry)
    {
        bool match = false;
        for (auto id : R)
        {
            if (id == state)
            {
                match = true;
            }
        }
        if (!match)
        {
            return false;
        }
    }
    return true;
}
bool checkForEntry(vector<string> R)
{
    for (auto transition : transitionTable)
    {
        bool matching = checkEntryRow(transition.states, R);
        if (matching)
        {
            return true;
        }
    }
    return false;
}
TransistionRow getRow(vector<string> S)
{
    for (auto row : transitionTable)
    {
        if (checkEntryRow(row.states, S))
            ;
        {
            return row;
        }
    }
}
void buildDFA()
{
    for (auto transition : transitionTable)
    {
        int counter = 1;
        for (auto state : transition.states)
        {
            // vector<string> row;
            string first;
            string second;

            if (transition.isAccept)
            {
                first = "+";
            }
            else
            {
                first = "-";
            }
            if (transition.isStart)
            {
                second = "0";
            }
            else
            {
                second = to_string(counter);
                counter++;
            }

            for (auto letter : alphabet)
            {
                if (transition.transitions[letter][0] == "E")
                {
                    vector<string> row;
                    row.push_back(first);
                    row.push_back(second);
                    row.push_back("E");
                    dfa.push_back(row);
                }
                else
                {
                    for (auto id : transition.transitions[letter])
                    {
                        vector<string> row;
                        row.push_back(first);
                        row.push_back(second);
                        row.push_back(id);
                        dfa.push_back(row);
                    }
                }
            }

            // map<string, vector<string>>::iterator itr;
            // for (itr = transition.transitions.begin(); itr != transition.transitions.end(); ++itr)
            // {

            // }
        }
    }
}
void convertNFAtoDFA()
{
    bool start = true;
    firstRow = nfa[0];
    stack<vector<string>> localL;

    vector<string> i = {0};
    followLabda(i);

    TransistionRow startRow = TransistionRow();
    startRow.isStart = start;
    startRow.isAccept = isAccept(i);
    startRow.states = i;
    localL.push(i);
    start = false;

    while (!localL.empty())
    {
        vector<string> S = localL.top();
        localL.pop();

        TransistionRow currentRow = getRow(S);

        // TransistionRow currentRow;
        for (auto letter : alphabet)
        {
            vector<string> R = followChar(S, letter);
            followLabda(R);
            if (R.size() > 0)
            {
                // Fill in transisitonRow mapping letter -> R
                currentRow.transitions.insert(pair<string, vector<string>>(letter, R));
                // Check if there is an entry in the transition table
                bool seen = checkForEntry(R);
                if (!seen)
                {
                    // initialize row
                    TransistionRow newTransition = TransistionRow();
                    newTransition.isStart = start;
                    newTransition.isAccept = isAccept(R);
                    newTransition.states = R;
                    localL.push(R);
                    transitionTable.push_back(newTransition);
                }
            }
            else
            {
                // Fill in transitonRow mapping of letter -> E
                currentRow.transitions.insert(pair<string, vector<string>>(letter, emptySet));
            }
        }
    }

    buildDFA();
}

void saveFile(string filePath)
{
    ofstream outputFile;
    cout << "Saving to " << filePath << endl;
    outputFile.open(filePath);
    for (auto row : dfa)
    {
        for (auto entry : row)
        {
            outputFile << entry;
            outputFile << " ";
        }
        outputFile << endl;
    }
    outputFile.close();
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

void printL()
{
    for (Grouping group : L)
    {
        cout << "----------------------------------" << endl;
        cout << "States {";
        for (auto state : group.states)
        {
            cout << state << " ";
        }
        cout << "} ";
        cout << "Alphabet {";
        for (auto letter : group.alphabet)
        {
            cout << letter << " ";
        }
        cout << "} " << endl;
    }
    cout << "--------------------------------" << endl;
}

void printM()
{
    for (auto set : M)
    {
        cout << "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM" << endl;
        cout << "States {";
        for (auto state : set)
        {
            cout << state << " ";
        }
        cout << "} " << endl;
    }
    cout << "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM" << endl;
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
    for (int i = 2; i < dfa[0].size(); i++)
    {
        acceptinStates.alphabet.push_back(i);
        nonAcceptingStates.alphabet.push_back(i);
    }
    L.insert(L.begin(), acceptinStates);
    L.insert(L.begin(), nonAcceptingStates);
}

void merging(vector<vector<string>> &dfa)
{
    // cout << "Initial Size " << dfa.size() << endl;
    // cout << "M: " << M.size() << endl;
    // Merge things...
    for (vector<int> subset : M)
    {
        for (int i = 0; i < dfa.size(); i++)
        {
            auto row = dfa[i];
            if (stoi(row[1]) == subset[1])
            {
                // cout << "Deletig " << subset[1] << endl;
                // cout << "New Size " << dfa.size() << endl;

                dfa.erase(dfa.begin() + i);
            }
        }
    }

    for (vector<int> subset : M)
    {
        for (int i = 0; i < dfa.size(); i++)
        {
            auto &row = dfa[i];

            for (int j = 2; j < row.size(); j++)
            {
                if (row[j] != "E")
                {
                    if (stoi(row[j]) == subset[1])
                    {
                        // cout << "Replaced " << row[j];

                        row[j] = to_string(subset[0]);
                        // cout << " with " << row[j] << endl;
                    }
                }
            }
        }
    }
    M.clear();
}

void seg()
{
    Grouping potential = L.front();
    L.erase(L.begin());
    vector<int> states = potential.states;
    vector<int> alphabet = potential.alphabet;

    // Iterate over every row in dfa if its in states and see if its column (c)
    // has an entry
    // If so make a set
    // if not add to empty set

    // Get first letter in set alphabet and remove it from set
    int letter = alphabet[0];
    alphabet.erase(alphabet.begin());
    vector<int> notTransitions;
    // cout << "Initial letter..." << letter << endl;

    vector<matches> pastMatches;
    pastMatches.clear();

    for (int state : states)
    {
        // cout << "State.." << state << endl;
        // Get Row
        vector<string> row;
        for (auto list : dfa)
        {
            if (stoi(list[1]) == state)
            {
                row = list;
                break;
            }
        }
        // vector<string> row = dfa[state];
        if (row[letter] == "E")
        {
            // cout << "Error transition" << endl;
            // Not a transition
            notTransitions.push_back(state);
        }
        else
        {
            // cout << "Valid Transition..." << row[letter] << endl;
            bool seen = false;
            for (matches &match : pastMatches)
            {
                if (match.id == stoi(row[letter]))
                {
                    // cout << "Seen before ... " << match.id << " " << row[letter] << endl;
                    match.stateIds.push_back(state);
                    seen = true;
                    // cout << "match len " << match.stateIds.size() << endl;
                    // pastMatches.push_back(match);
                    break;
                }
            }
            if (!seen)
            {
                // cout << "Not seen before " << row[letter] << endl;
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
        if (partition.stateIds.size() > 1)
        {
            // cout << "Greater " << endl;
            if (alphabet.size() == 0)
            {
                // add S to M
                // cout << "Addind to M" << endl;
                M.push_back(partition.stateIds);
                // printM();
            }
            else
            {
                // cout << "Adding a set back to L " << endl;
                Grouping temp;
                temp.states = partition.stateIds;
                temp.alphabet = alphabet;
                L.insert(L.begin(), temp);
                // printL();
            }
        }
    }
    if (notTransitions.size() > 1)
    {
        if (alphabet.size() != 0)
        {
            // cout << "Pushing non transition set back to L" << notTransitions.size() << endl;
            Grouping temp;
            temp.alphabet = alphabet;
            temp.states = notTransitions;
            L.insert(L.begin(), temp);
            // printL();
        }
        else
        {
            // add S to M
            // cout << "Addind to M" << endl;
            M.push_back(notTransitions);
            // printM();
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
    string inputFile = argv[1];
    load_file(inputFile);
    convertNFAtoDFA();
    // convertFormat();
    printDFA();

    // int lastSize = dfa.size();
    // int currentSize = 0;
    // int counter = 0;
    // while (lastSize != currentSize)
    // {
    //     counter++;
    //     // printDFA();
    //     initialize();
    //     // printL();
    //     lastSize = dfa.size();

    //     while (!L.empty())
    //     {
    //         seg();
    //         // printL();
    //         // cout << "Optimizing.." << endl;
    //     }
    //     merging(dfa);
    //     currentSize = dfa.size();
    // }

    // printDFA();
    // cout << "Ran through " << counter << " time" << endl;
    // saveFile("optimized.txt");
}
