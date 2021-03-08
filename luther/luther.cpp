#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include "helpers.h"

using namespace std;

string scanFile, sourceFile, tokensFile;
string tokensAscii;

deque<Table> transitionTables;

struct ScannerLookup
{
    vector<vector<string>> rows;
};

ScannerLookup scannerLookup;

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
            tokensAscii = line;
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

Table readTable(string filePath)
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
    return currentTable;
    inputFile.close();
}

void loadTables()
{
    for (auto row : scannerLookup.rows)
    {
        string tablePath = row[0];
        auto transitionTable = readTable(tablePath);
        transitionTables.push_back(transitionTable);
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

    loadScanFile(scanFile);
    loadTables();
    printTables(transitionTables);
}
