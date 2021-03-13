// Need to identify the final state and non final states
// Add field to tables row
// Change check to be if was in fianl then went back its done

#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include "helpers.h"

using namespace std;

struct CurrentMatch
{
    /* data */
    string key;
    int tokenLength;
    int startCharacterIndex = 0;
    int endingCharacterIndex = 0;
    int lineSpecificCharacterIndex = 1;
};
map<string, string> hexadecemilValues;
map<string, string> tokenMappings;

string scanFile, sourceFile, tokensFile;
string tokensHex;
vector<string> asciiHeader;
vector<string> preConverted;

deque<Table> transitionTables;

struct ScannerLookup
{
    vector<vector<string>> rows;
};

ScannerLookup scannerLookup;
vector<char> inputStream;

void loadScanFile(string filePath)
{
    bool firstLine = true;
    string line;
    ifstream inputFile(filePath);
    if (!inputFile)
    {
        exit(1);
    }
    while (getline(inputFile, line))
    {
        if (line.empty())
        {
            continue;
        }

        if (firstLine)
        {
            firstLine = false;
            tokensHex = line;
            continue;
        }

        vector<string> row;
        istringstream ss(line);
        for (string s; ss >> s;)
        {
            row.push_back(s);
        }
        scannerLookup.rows.push_back(row);
    }
    if (firstLine)
    {
        exit(1);
    }
    inputFile.close();
}

Table readTable(string filePath, string token)
{
    Table currentTable;
    string line;
    // filePath = "../" + filePath;
    ifstream inputFile(filePath);

    if (!inputFile)
    {
        exit(1);
    }
    while (getline(inputFile, line))
    {
        vector<string> fullRow;
        istringstream ss(line);
        for (string s; ss >> s;)
        {
            fullRow.push_back(s);
        }

        // Easier to get the full row from the file than only save columns 2 - end
        // to the table
        int key = stoi(fullRow[1]);
        vector<string> &row = currentTable.rows[key];
        for (int i = 0; i < fullRow.size(); i++)
        {
            row.push_back(fullRow[i]);
        }
    }
    currentTable.token = token;
    return currentTable;
    inputFile.close();
}

void loadTables()
{
    for (auto row : scannerLookup.rows)
    {
        string tablePath = row[0];
        string token = row[1];
        if (row.size() == 3)
        {
            // Add alternative
            tokenMappings.insert(pair<string, string>(token, row[2]));
        }

        auto transitionTable = readTable(tablePath, token);
        transitionTables.push_back(transitionTable);
    }
}

void loadSource(string filePath)
{
    ifstream sourceFile;
    sourceFile.open(filePath);
    if (!sourceFile)
    {
        exit(1);
    }
    char data;
    while (sourceFile.get(data))
    {
        // if (data == ' ')
        // {
        //     cout << "Space" << endl;
        // }
        // else if (data == '\n')
        // {
        //     cout << "Newline" << endl;
        // }
        // else
        // {
        //     cout << data << endl;
        // }
        inputStream.push_back(data);
    }
}

void convertHeader()
{
    for (int i = 0; i < tokensHex.size(); i++)
    {
        if (tokensHex[i] == ' ')
        {
            continue;
        }
        string asciiCharacter;
        if (tokensHex[i] == 'x')
        {
            string hexCharacter;
            hexCharacter += tokensHex[i + 1];
            hexCharacter += tokensHex[i + 2];
            i += 2;
            char tempAscii = stoul(hexCharacter, nullptr, 16);
            asciiCharacter += tempAscii;
            hexadecemilValues.insert(pair<string, string>(asciiCharacter, "x" + hexCharacter));
        }
        else
        {
            asciiCharacter = tokensHex[i];
            hexadecemilValues.insert(pair<string, string>(asciiCharacter, asciiCharacter));

            // preConverted.push_back(asciiCharacter);
        }

        asciiHeader.push_back(asciiCharacter);
    }
    // // Print
    // for (auto character : asciiHeader)
    // {
    //     cout << character << ", ";
    // }
    // cout << endl;
}

int getHeaderColumn(char character)
{
    for (int i = 0; i < asciiHeader.size(); i++)
    {
        string tmp;
        tmp += character;
        if (asciiHeader[i] == tmp)
        {
            return i + 2; // Two offset since transistion table has is first row as terminal indicator and second as id
        }
    }
    return -1;
}

void updateCounter(string token)
{
    for (auto &table : transitionTables)
    {
        if (table.token == token)
        {
            table.counter++;
        }
    }
}

void resetCounters()
{
    for (auto &table : transitionTables)
    {
        table.counter = 0;
    }
}

void resetSingleCounter(string token)
{
    for (auto &table : transitionTables)
    {
        if (table.token == token)
        {
            table.counter = 0;
        }
    }
}

bool needsConversion(string character)
{
    for (auto preSet : preConverted)
    {
        if (preSet == character)
        {
            return false;
        }
    }
    return true;
}

bool hasAlternative(string token)
{
    if (tokenMappings.count(token) > 0)
    {
        return true;
    }
    return false;
}

void processSource(string outputFile)
{
    ofstream tokenFile;
    tokenFile.open(outputFile);
    CurrentMatch currentMatch;
    int line = 1;
    // Copy I hope??
    deque<Table> masterList = transitionTables;
    int count = 0;
    for (int i = 0; i < inputStream.size(); i++)
    {
        auto character = inputStream[i];

        count++;
        int column = getHeaderColumn(character);
        if (column < 0)
        {
            // cout << "Index error" << endl;
            exit(1);
        }
        deque<Table> matchingList;

        // Go through the master list backwards to ensure priority
        for (int j = masterList.size() - 1; j >= 0; j--)
        {
            auto potentialTable = masterList[j];
            vector<string> &row = potentialTable.rows[potentialTable.currentRowId];
            if (row[column] != "E")
            {
                // // Is a valid transistion check if its from a final row to a non final
                // if (potentialTable.isFinal)
                // {
                //     if (row[0] == "+")
                //     {
                potentialTable.currentRowId = stoi(row[column]);
                matchingList.push_front(potentialTable);
                //         potentialTable.isFinal = true;
                //         // Need to update the transistion table counter for this table
                //         updateCounter(potentialTable.token);
                //     }
                //     else
                //     {
                //         resetSingleCounter(potentialTable.token);
                //     }
                // }
                // else
                // {
                //     // Is a match update the current row id and add to matching list
                //     potentialTable.currentRowId = stoi(row[column]);
                //     if (potentialTable.rows[potentialTable.currentRowId][0] == "+")
                //     {
                //         potentialTable.isFinal = true;
                //     }
                //     else
                //     {
                //         potentialTable.isFinal = false;
                //     }
                //     matchingList.push_back(potentialTable);
                //     updateCounter(potentialTable.token);
                // }

                // Check if next transition puts us in an accepting state
                if (potentialTable.rows[potentialTable.currentRowId][0] == "+")
                {
                    // Was a entrence to an accepting state can be the current highest length token
                    currentMatch.tokenLength = count; // single token lenght
                    currentMatch.key = potentialTable.token;
                    currentMatch.endingCharacterIndex = i;
                }
            }
            // else
            // {
            //     if (!potentialTable.isFinal)
            //     {
            //         // Not an option anymore
            //         resetSingleCounter(potentialTable.token);
            //     }
            // }
        }
        if (matchingList.size() == 0 || (i == inputStream.size() - 1))
        {
            // We found longest token
            // Table longestTokenTable = masterList[0];
            // cout << "Longest match: " << longestTokenTable.token << endl;

            // Table longestTable;
            // for (int i = transitionTables.size() - 1; i >= 0; i--)
            // {
            //     auto table = transitionTables[i];
            //     if (table.counter >= longestTable.counter)
            //     {
            //         longestTable = table;
            //     }
            // }

            // cout << "Longest match: " << longestTable.token << endl;

            // resetCounters();
            // masterList = transitionTables;
            int lineCounter = 1;
            for (int i = 0; i < currentMatch.startCharacterIndex; i++)
            {
                auto character = inputStream[i];
                if (character == '\n')
                {
                    lineCounter++;
                    currentMatch.lineSpecificCharacterIndex = currentMatch.startCharacterIndex - i;
                }
            }
            if (lineCounter == 1)
            {
                currentMatch.lineSpecificCharacterIndex = currentMatch.startCharacterIndex + 1;
            }
            tokenFile << currentMatch.key << " ";
            // cout << currentMatch.key << endl;
            // Not hard coded but list of ones with alternatives
            if (!hasAlternative(currentMatch.key))
            {
                for (int i = currentMatch.startCharacterIndex; i <= currentMatch.endingCharacterIndex; i++)
                {
                    string tmp = "";
                    tmp += inputStream[i];
                    // if (needsConversion(tmp))
                    // {
                    //     // Hex version
                    //     cout << "x" << int(inputStream[i]);
                    // }
                    // else
                    // {
                    //     cout << inputStream[i];
                    // }
                    tokenFile << hexadecemilValues[tmp];
                }
            }
            else
            {
                tokenFile << tokenMappings[currentMatch.key];
            }

            tokenFile << " " << lineCounter << " " << currentMatch.lineSpecificCharacterIndex << endl;

            i = currentMatch.endingCharacterIndex;
            currentMatch.startCharacterIndex += currentMatch.tokenLength;
            count = 0;
            masterList = transitionTables;
        }
        else
        {
            masterList = matchingList;
        }

        // if (character == '\n')
        // {
        //     line++;
        // }
    }
    tokenFile.close();
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        // cout << "Not enough arguments..." << endl;
        exit(1);
    }

    scanFile = argv[1];
    sourceFile = argv[2];
    tokensFile = argv[3];

    // cout << scanFile << " " << sourceFile << " " << tokensFile << endl;
    // scanFile = "../wiki/scan.u";
    // sourceFile = "../wiki/program.src";
    loadScanFile(scanFile);
    loadTables();
    // printTables(transitionTables);
    loadSource(sourceFile);
    convertHeader();

    processSource(tokensFile);
}
