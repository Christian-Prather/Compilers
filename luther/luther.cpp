#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include "helpers.h"

using namespace std;

string scanFile, sourceFile, tokensFile;
string tokensHex;
vector<string> asciiHeader;

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
    inputFile.close();
}

Table readTable(string filePath, string token)
{
    Table currentTable;
    string line;
    filePath = "../" + filePath;
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
        for (int i = 2; i < fullRow.size(); i++)
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
        auto transitionTable = readTable(tablePath, token);
        transitionTables.push_back(transitionTable);
    }
}

void loadSource(string filePath)
{
    ifstream sourceFile;
    sourceFile.open(filePath);
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
        }
        else
        {
            asciiCharacter = tokensHex[i];
        }

        asciiHeader.push_back(asciiCharacter);
    }

    // Print
    for (auto character : asciiHeader)
    {
        cout << character << ", ";
    }
    cout << endl;
}

int getHeaderColumn(char character)
{
    for (int i = 0; i < asciiHeader.size(); i++)
    {
        string tmp;
        tmp += character;
        if (asciiHeader[i] == tmp)
        {
            return i;
        }
    }
    return -1;
}

void processSource()
{
    // Copy I hope??
    deque<Table> masterList = transitionTables;

    for (auto character : inputStream)
    {
        int column = getHeaderColumn(character);
        if (column < 0)
        {
            cout << "Index error" << endl;
            exit(1);
        }
        deque<Table> matchingList;

        for (int i = 0; i < masterList.size(); i++)
        {
            auto potentialTable = masterList[i];
            vector<string> &row = potentialTable.rows[potentialTable.currentRowId];
            if (row[column] != "E")
            {
                // Is a match update the current row id and add to matching list
                potentialTable.currentRowId = stoi(row[column]);
                matchingList.push_back(potentialTable);
            }
        }
        if (matchingList.size() == 0)
        {
            // We found longest token
            Table longestTokenTable = masterList[0];
            cout << "Longest match: " << longestTokenTable.token << endl;
            masterList = transitionTables;
        }
        else
        {
            masterList = matchingList;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 4)
        // {
        //     cout << "Not enough arguments..." << endl;
        //     return -1;
        // }

        // scanFile = argv[1];
        // sourceFile = argv[2];
        // tokensFile = argv[3];
        scanFile = "../wiki/scan.u";
    sourceFile = "../wiki/program.src";
    loadScanFile(scanFile);
    loadTables();
    printTables(transitionTables);
    loadSource(sourceFile);
    convertHeader();

    processSource();
}
