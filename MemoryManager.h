#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <fstream>
#include "Process.h"

using namespace std;


extern bool flag;
extern int M, V, P, pagesMain, pagesVirtual;
extern int pidCounter;
extern vector<bool> isFreeMain;
extern int freePagesMain;
extern vector<bool> isFreeVirtual;
extern int freePagesVirtual;
extern unordered_map<int, Process *> processMap;
extern vector<pair<bool, int>> mainMemory;
extern vector<pair<bool, int>> virtualMemory;
extern vector<int> lru;

// Function declarations
int getFirstFreeIndex(vector<bool> &vec);
int getPhysicalAddress(int virAdd, vector<int> &pageTable);
void swapout(int pid, ofstream &outputFile);
void swap_out(int pid, ofstream &outputFile);
void swap_in(int pid, ofstream &outputFile);
void swapin(int pid, ofstream &outputFile);
void load(string fileName, ofstream &outputFile);
void run(int pid, ofstream &outputFile);
void kill(int pid, ofstream &outputFile);
void listpr(ofstream &outputFile);
void pte(int pid, string fileName, ofstream &outputFile);
void pteall(string fileName, ofstream &outputFile);
void print(int memLocation, int len, ofstream &outputFile);


#endif // MEMORY_MANAGER_H
