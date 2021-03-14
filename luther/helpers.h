#include <deque>
#include <iostream>
#include "ll-table.h"
using namespace std;

void printTables(deque<Table> tables)
{
    for (int i = 0; i < tables.size(); i++)
    {
        auto table = tables[i];
        map<int, vector<string>>::iterator it;
        for (it = table.rows.begin(); it != table.rows.end(); it++)
        {
            cout << it->first << " ";
            for (auto column : it->second)
            {
                cout << column << " ";
            }
            cout << endl;
        }
        cout << "/////////////////////////////////" << endl;
    }
}