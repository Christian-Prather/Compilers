#ifndef LL_TABLE_H
#define LL_TABLE_H

#include <vector>
#include <string>
#include <map>

using namespace std;

struct Table
{
    vector<string> headerRow;
    int currentRowId = 0;
    bool isFinal = false;
    map<int, vector<string>> rows;
    string token;
    int counter = 0;
};

#endif