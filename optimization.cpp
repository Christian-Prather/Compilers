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
    int id;
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
vector<int> finals;
void load_file(string filePath)
{
    string line;
    ifstream inputFile(filePath);
    if (!inputFile)
    {
        exit(1);
    }
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
    for (int i = 0; i < nfa.size(); i++)
    {
        auto row = nfa[i];
        if (row[1] == id)
        {
            indexes.push_back(i);
        }
    }
    return indexes;
}

bool checkInS(vector<string> &S, string potential)
{
    for (auto s : S)
    {
        if (s == potential)
        {
            return true;
        }
    }
    return false;
}

void followLabda(vector<string> &S)
{
    stack<string> lambdaM;

    for (auto id : S)
    {
        lambdaM.push(id);
    }

    while (lambdaM.size() > 0)
    {
        string id = lambdaM.top();
        lambdaM.pop();
        vector<int> indexes = findRow(id);
        for (auto index : indexes)
        {
            auto transition = nfa[index];
            for (int i = 3; i < transition.size(); i++)
            {
                if (transition[i] == firstRow[1])
                {
                    // Check if in S already
                    if (!checkInS(S, transition[2]))
                    {
                        // Lambda add the to state id to M and eye
                        // lambdaM may be an issue adding to it like this
                        // may need to convert it to a true stack
                        S.push_back(transition[2]);
                        lambdaM.push(transition[2]);
                    }
                }
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
            for (int i = 3; i < row.size(); i++)
            {
                if (row[i] == c)
                {
                    F.push_back(row[2]);
                }
            }
        }
    }
    if (F.size() == 0)
    {
        F = emptySet;
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
    for (int i = 2; i < firstRow.size(); i++)
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
TransistionRow *getRow(vector<string> S)
{
    for (int i = 0; i < transitionTable.size(); i++)
    {
        if (checkEntryRow(transitionTable[i].states, S))
        {
            return &transitionTable[i];
        }
    }
}
void buildDFA()
{

    int counter = 0;
    for (int i = 0; i < transitionTable.size(); i++)
    {
        transitionTable[i].id = counter;
        counter++;
    }

    for (auto transition : transitionTable)
    {
        // Map every row to an id

        vector<string> row;
        if (transition.isAccept)
        {
            row.push_back("+");
        }
        else
        {
            row.push_back("-");
        }

        row.push_back(to_string(transition.id));
        for (auto letter : alphabet)
        {
            if (transition.transitions[letter][0] == "E")
            {
                row.push_back("E");
            }
            else
            {
                TransistionRow *matchingRow = getRow(transition.transitions[letter]);
                row.push_back(to_string(matchingRow->id));
            }
        }
        dfa.push_back(row);
    }
}
void convertNFAtoDFA()
{
    bool start = true;
    firstRow = nfa[0];
    parseAlphabet();
    findAcceptingStates();
    stack<vector<string>> localL;
    vector<string> i = {"0"};

    TransistionRow startRow = TransistionRow();
    startRow.isStart = start;
    startRow.isAccept = isAccept(i);
    startRow.states = i;
    localL.push(i);
    start = false;
    transitionTable.push_back(startRow);
    followLabda(i);

    while (!localL.empty())
    {
        vector<string> S = localL.top();
        localL.pop();

        // TransistionRow currentRow;
        for (auto letter : alphabet)
        {
            TransistionRow *currentRow = getRow(S);

            vector<string> R = followChar(S, letter);
            followLabda(R);
            if (R[0] != "E")
            {
                // Fill in transisitonRow mapping letter -> R
                currentRow->transitions.insert(pair<string, vector<string>>(letter, R));
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
                currentRow->transitions.insert(pair<string, vector<string>>(letter, emptySet));
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

void findOptions(vector<string> row, stack<int> *options, vector<int> *past)
{

    for (int i = 2; i < row.size(); i++)
    {
        if (row[i] != "E")
        {
            bool already = false;
            for (auto seen : *past)
            {
                if (seen == stoi(row[i]))
                {
                    already = true;
                }
            }

            if (!already)
            {
                // Potential path
                options->push(stoi(row[i]));
                past->push_back(stoi(row[i]));
            }
        }
    }
}

vector<int> terminals()
{
    vector<int> finals;
    for (auto row : dfa)
    {
        if (row[0] == "+")
        {
            finals.push_back(stoi(row[1]));
        }
    }
    return finals;
}

bool inFinal(int potential)
{
    for (auto terminal : finals)
    {
        if (terminal == potential)
        {
            return true;
        }
    }
    return false;
}

void deadStateHandling(vector<vector<string>> &dfa)
{
    finals = terminals();

    for (int i = 0; i < dfa.size(); i++)
    {

        // Move the start row down incuding it in past iterate
        vector<string> startRow = dfa[i];
        stack<int> options;
        vector<int> past;
        past.push_back(stoi(startRow[1]));

        findOptions(startRow, &options, &past);
        while (options.size() > 0)
        {
            int option = options.top();
            options.pop();
            vector<string> row = dfa[option];
            findOptions(row, &options, &past);
        }
        bool dead = true;
        for (auto seen : past)
        {
            if (inFinal(seen))
            {
                // Fine path
                dead = false;
                break;
            }
        }
        if (dead)
        {
            // Remove current start path may need to remove all refernces
            // to it
            dfa.erase(dfa.begin() + i);
        }
    }
}

vector<int> getStates()
{
    vector<int> states;
    for (auto row : dfa)
    {
        states.push_back(stoi(row[1]));
    }
    return states;
}

void unreachableHandling(vector<vector<string>> &dfa)
{
    // Find list of all states that can be hit from start
    vector<string> startRow = dfa[0];
    stack<int> options;
    vector<int> past;
    vector<int> statesList = getStates();
    past.push_back(stoi(startRow[1]));

    findOptions(startRow, &options, &past);
    while (options.size() > 0)
    {
        int option = options.top();
        options.pop();
        vector<string> row = dfa[option];
        findOptions(row, &options, &past);
    }

    // past is lost of all states you can hit
    for (int i = 0; i < statesList.size(); i++)
    {
        int state = statesList[i];
        bool reachable = false;
        for (auto seen : past)
        {
            if (state == seen)
            {
                reachable = true;
                break;
            }
        }
        if (!reachable)
        {
            // Remove state
            dfa.erase(dfa.begin() + i);
        }
    }
}
int findAlpha(string input)
{
    int unreachable = 10000;
    // cout << "Token " << input << endl;
    for (int i = 2; i < firstRow.size(); i++)
    {
        if (input == firstRow[i])
        {
            // cout << "RETURNING " << i << endl;
            return i;
        }
    }
    return unreachable;
}
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cout << "incorrect number of arguments"
             << endl;
        exit(1);
    }
    string inputFile = argv[1];
    string outputFile = argv[2];
    // string inputFile = "/home/christian/Documents/Compilers/test-files/NF/cblock.nfa";
    load_file(inputFile);
    convertNFAtoDFA();
    // convertFormat();
    // printDFA();

    int lastSize = dfa.size();
    int currentSize = 0;
    int counter = 0;
    while (lastSize != currentSize)
    {
        counter++;
        // printDFA();
        initialize();
        // printL();
        lastSize = dfa.size();

        while (!L.empty())
        {
            seg();
            // printL();
            // cout << "Optimizing.." << endl;
        }
        merging(dfa);
        currentSize = dfa.size();
    }

    // deadStateHandling(dfa);
    // unreachableHandling(dfa);
    // printDFA();
    // cout << "Ran through " << counter << " time" << endl;
    for (int i = 3; i < argc; i++)
    {
        cout << "..." << endl;
        vector<string> input;

        istringstream ss(argv[i]);
        for (string s; ss >> s;)
        {
            input.push_back(s);
        }
        if (input.empty())
        {
            cout << "OUTPUT " << 0 << endl;
            continue;
        }
        int spot = 1;
        bool match = true;
        vector<string> row = dfa[0];

        for (auto token : input)
        {

            for (auto letter : token)
            {
                string sLetter;
                sLetter += letter;
                // cout << "ScREW: " << sLetter << endl;
                int alphabetIndex = findAlpha(sLetter);

                // int transition = stoi(dfa[0][alphabetIndex]);
                //         vector<string> row = dfa[transition];

                if (alphabetIndex == 10000 | row[alphabetIndex] == "E")
                {
                    cout << "OUTPUT " << spot << endl;
                    match = false;
                    break;
                }
                else
                {
                    // cout << "GGGGGGGGGGGGGGG"
                    //      << " " << alphabetIndex << " " << row[alphabetIndex];
                    row = dfa[stoi(row[alphabetIndex])];
                }

                spot++;
            }
            if (match)
            {
                if (row[0] == "+")
                {
                    cout << "OUTPUT :M:" << endl;
                }
                else
                {
                    cout << "OUTPUT " << spot << endl;
                }
            }
        }
    }
    cout << "..." << endl;
    saveFile(outputFile);
}
