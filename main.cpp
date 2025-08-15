#include<bits/stdc++.h>
#include "Process.h"
#include "MemoryManager.h"

int main(int argc, char * argv[]){
    if(argc != 11){
        cout<<"Invalid Arguments"<<endl;
        return 0;
    }

    string inFileName;
    string outFileName;
    M = atoi(argv[2]);
    V = atoi(argv[4]);
    P = atoi(argv[6]);
    inFileName = argv[8];
    inFileName += ".txt";
    outFileName = argv[10];
    outFileName += ".txt";

    pagesMain = (M * 1024)/P;
    pagesVirtual = (V * 1024)/P;
    freePagesMain = pagesMain;
    freePagesVirtual = pagesVirtual;
    for(int i=0; i<pagesMain; i++){
        isFreeMain.push_back(true);
    }
    for(int i=0;i<pagesVirtual;i++){
        isFreeVirtual.push_back(true);
    }
    for(int i=0; i<M*1024;i++){
        mainMemory.push_back({false,0});
    }
    for(int i=0;i<V*1024;i++){
        virtualMemory.push_back({false,0});
    }
    

    ofstream outputFile(outFileName);
    if(outputFile.is_open() == false){
        cout<<"Unable to open the Input file"<<endl;
    }

    ifstream inputFile;
    inputFile.open(inFileName);
    if(inputFile.is_open() == false){
        cout << "Unable to open the Input file" << endl;
        outputFile.close();
        inputFile.close();
        return 0;
    }
    string currentLine;
    while(getline(inputFile,currentLine)){
        stringstream toDivide(currentLine);
        string data;
        getline(toDivide,data,' ');
        if(data == "load"){
            vector<string> FileNames;
            while(getline(toDivide,data,' ')){
                data += ".txt";
                FileNames.push_back(data);
            }
            for(auto itr : FileNames){
                load(itr,outputFile);
            }
        }
        else if(data == "run"){
            getline(toDivide,data,' ');
            int processId = stoi(data);
            run(processId,outputFile);
        }
        else if(data == "kill"){
            getline(toDivide,data,' ');
            int processId = stoi(data);
            kill(processId,outputFile);
        }
        else if(data == "listpr"){
            listpr(outputFile);
        }
        else if(data == "pte"){
            getline(toDivide,data,' ');
            int processId = stoi(data);
            getline(toDivide,data,' ');
            data += ".txt";
            pte(processId,data,outputFile);
        }
        else if(data == "pteall"){
            getline(toDivide,data,' ');
            data += ".txt";
            pteall(data,outputFile);
        }
        else if(data == "swapin"){
            getline(toDivide,data,' ');
            int processId = stoi(data);
            swapin(processId,outputFile);
        }
        else if(data == "swapout"){
            getline(toDivide,data,' ');
            int processId = stoi(data);
            swapout(processId,outputFile);
        }
        else if(data == "print"){
            getline(toDivide,data,' ');
            int memloc = stoi(data);
            getline(toDivide,data,' ');
            int len = stoi(data);
            print(memloc,len,outputFile);
        }
        else{
            //exit case
            outputFile<<"Exited"<<endl;
            processMap.clear();
            isFreeMain.clear();
            isFreeVirtual.clear();
            mainMemory.clear();
            virtualMemory.clear();

            outputFile.close();
            inputFile.close();
            return 0;
        }
    }

    outputFile.close();
    inputFile.close();
}
