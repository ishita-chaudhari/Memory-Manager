#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>

using namespace std;

class Process{
    public:
        string fileName;
        int pid;
        int size; //size in B
        bool inMain;
        vector<int> pageTable;

        bool isFileNameValid();
};

#endif // PROCESS_H
