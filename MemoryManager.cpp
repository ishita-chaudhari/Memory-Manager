#include "MemoryManager.h"
#include<bits/stdc++.h>
#include<ctime>

using namespace std;

bool flag;
int M, V, P, pagesMain, pagesVirtual;
int pidCounter = 1;
vector<bool> isFreeMain;
int freePagesMain;
vector<bool> isFreeVirtual;
int freePagesVirtual;
unordered_map<int, Process *> processMap;
vector<pair<bool, int>> mainMemory;
vector<pair<bool, int>> virtualMemory;
vector<int> lru;

int getFirstFreeIndex(vector<bool> &vec){
    int size = vec.size();
    for(int i=0;i<size;i++){
        if(vec[i] == true){
            vec[i] = false;
            return i;
        }
    }
    return -1;
}

int getPhyicalAddress(int virAdd,vector<int> &pageTable){
    int vpn = virAdd/P;
    int offset = virAdd - vpn*P;
    int pfn =pageTable[vpn];
    int phyAd=pfn*P + offset;
    return phyAd;
}

void swapout(int pid, ofstream &outputFile){ 
    if((processMap.find(pid) != processMap.end()) == false){
        outputFile<<"Error given pid doesn't exits";
        return;
    }
    if(processMap[pid]->inMain == false){
        outputFile<<"Error given pid doesn't exits in main Memory"<<endl;
        return;
    }

    int pages = processMap[pid]->pageTable.size();

    if(freePagesVirtual >= pages){
        processMap[pid]->inMain = false;
        vector<int> temp;
        for(auto itr:processMap[pid]->pageTable){
            temp.push_back(itr);
        }
        processMap[pid]->pageTable.clear();
        for(auto itr:temp){
            isFreeMain[itr] = true;
        }

        for(int i=0;i<temp.size();i++){
            int index = getFirstFreeIndex(isFreeVirtual);
            isFreeVirtual[index] = false;
            processMap[pid]->pageTable.push_back(index);
        }

        for(int i=0;i<processMap[pid]->size;i++){
            int oldPhyAd = getPhyicalAddress(i,temp);
            int newPhyAd = getPhyicalAddress(i,processMap[pid]->pageTable);
            virtualMemory[newPhyAd] = mainMemory[oldPhyAd];
            mainMemory[oldPhyAd] ={false,0};
        }

        freePagesMain += pages;
        freePagesVirtual -= pages;

        int index = -1;
        for(int i=0;i<lru.size();i++){
            if(lru[i] == pid){
                index = i;
                break;
            }
        }
        lru.erase(lru.begin() + index);

        outputFile<<"swapout Done for Pid "<<pid<<endl;
    }
    else{
        //assumption nothing mentioned in question hence we are giving error.
        outputFile<<"Error virtual memory is full"<<endl;
    } 
}


void swap_out(int pid, ofstream &outputFile){ 
    if((processMap.find(pid) != processMap.end()) == false){
        outputFile<<"Error given pid doesn't exits";
        return;
    }
    if(processMap[pid]->inMain == false){
        outputFile<<"Error given pid doesn't exits in main Memory"<<endl;
        return;
    }

    int pages = processMap[pid]->pageTable.size();

    if(freePagesVirtual >= pages){
        processMap[pid]->inMain = false;
        vector<int> temp;
        for(auto itr:processMap[pid]->pageTable){
            temp.push_back(itr);
        }
        processMap[pid]->pageTable.clear();
        for(auto itr:temp){
            isFreeMain[itr] = true;
        }

        for(int i=0;i<temp.size();i++){
            int index = getFirstFreeIndex(isFreeVirtual);
            isFreeVirtual[index] = false;
            processMap[pid]->pageTable.push_back(index);
        }

        for(int i=0;i<processMap[pid]->size;i++){
            int oldPhyAd = getPhyicalAddress(i,temp);
            int newPhyAd = getPhyicalAddress(i,processMap[pid]->pageTable);
            virtualMemory[newPhyAd] = mainMemory[oldPhyAd];
            mainMemory[oldPhyAd] ={false,0};
        }

        freePagesMain += pages;
        freePagesVirtual -= pages;

        int index = -1;
        for(int i=0;i<lru.size();i++){
            if(lru[i] == pid){
                index = i;
                break;
            }
        }
        lru.erase(lru.begin() + index);
    }
    else{
        //assumption nothing mentioned in question hence we are giving error.
        outputFile<<"Error virtual memory is full"<<endl;
    } 
}


void swap_in(int pid, ofstream &outputFile){
    if((processMap.find(pid) != processMap.end()) == false ){
        outputFile<<"Error given pid doesn't exist";
        return;
    }
    if(processMap[pid]->inMain == true){
        outputFile<<"Error given pid doesn't exist in Virtual Memory"<<endl;
        return;
    }

    int pages = processMap[pid]->pageTable.size();

    if(freePagesMain >= pages){
        processMap[pid]->inMain = true;
        vector<int> temp;
        for(auto itr:processMap[pid]->pageTable){
            temp.push_back(itr);
        }
        processMap[pid]->pageTable.clear();
        for(auto itr:temp){
            isFreeVirtual[itr] = true;
        }

        for(int i=0;i<temp.size();i++){
            int index = getFirstFreeIndex(isFreeMain);
            isFreeMain[index] = false;
            processMap[pid]->pageTable.push_back(index);
        }

        for(int i=0;i<processMap[pid]->size;i++){
            int oldPhyAd = getPhyicalAddress(i,temp);
            int newPhyAd = getPhyicalAddress(i,processMap[pid]->pageTable);
            mainMemory[newPhyAd] = virtualMemory[oldPhyAd];
            virtualMemory[oldPhyAd] ={false,0};
        }

        freePagesMain -= pages;
        freePagesVirtual += pages;

        lru.push_back(pid);
    }
    else{
        int curFreeSpace = freePagesMain;
        int toSwap = 0;
        vector<int> toSwapPids;
        for(int i=0;i<lru.size();i++){
            int temp = (processMap[lru[i]]->pageTable.size());
            
            curFreeSpace += temp;
            toSwap += temp;
            toSwapPids.push_back(lru[i]);
            if(curFreeSpace >= pages){
                break;
            }
        }
   
        if(toSwap > freePagesVirtual){
            outputFile<<"Error LRU method fails"<<endl;
            return ;
        }
        if(curFreeSpace < pages){
            outputFile<<"Error LRU method fails"<<endl;
            return ;
        }

        for(auto itr:toSwapPids){
            swap_out(itr,outputFile);
        }
        swap_in(pid,outputFile);
    }
}


void swapin(int pid, ofstream &outputFile){
    if((processMap.find(pid) != processMap.end()) == false ){
        outputFile<<"Error given pid doesn't exist";
        return;
    }
    if(processMap[pid]->inMain == true){
        outputFile<<"Error given pid doesn't exist in Virtual Memory"<<endl;
        return;
    }

    int pages = processMap[pid]->pageTable.size();

    if(freePagesMain >= pages){
        processMap[pid]->inMain = true;
        vector<int> temp;
        for(auto itr:processMap[pid]->pageTable){
            temp.push_back(itr);
        }
        processMap[pid]->pageTable.clear();
        for(auto itr:temp){
            isFreeVirtual[itr] = true;
        }

        for(int i=0;i<temp.size();i++){
            int index = getFirstFreeIndex(isFreeMain);
            isFreeMain[index] = false;
            processMap[pid]->pageTable.push_back(index);
        }

        for(int i=0;i<processMap[pid]->size;i++){
            int oldPhyAd = getPhyicalAddress(i,temp);
            int newPhyAd = getPhyicalAddress(i,processMap[pid]->pageTable);
            mainMemory[newPhyAd] = virtualMemory[oldPhyAd];
            virtualMemory[oldPhyAd] ={false,0};
        }

        freePagesMain -= pages;
        freePagesVirtual += pages;

        lru.push_back(pid);
    }
    else{
        int curFreeSpace = freePagesMain;
        int toSwap = 0;
        vector<int> toSwapPids;
        for(int i=0;i<lru.size();i++){
            int temp = (processMap[lru[i]]->pageTable.size());
            
            curFreeSpace += temp;
            toSwap += temp;
            toSwapPids.push_back(lru[i]);
            if(curFreeSpace >= pages){
                break;
            }
        }

        if(toSwap > freePagesVirtual){
            outputFile<<"Error LRU method fails"<<endl;
            return ;
        }
        if(curFreeSpace < pages){
            outputFile<<"Error LRU method fails"<<endl;
            return ;
        }

        for(auto itr:toSwapPids){
            swap_out(itr,outputFile);
        }
        swap_in(pid,outputFile);
    }

    outputFile<<"Swapin Done for pid "<<pid<<endl;
    flag = true;
}

void load(string fileName, ofstream &outputFile){
    ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open() == false){
        outputFile <<fileName<< " could not be loaded - file does not exist" << endl;
        inputFile.close();
        return;
    }
    string data;
    getline(inputFile,data);
    int size = stoi(data);
    int pages = (size * 1024)/P;
    
    if(freePagesMain >= pages){
        int pid = pidCounter;
        pidCounter ++;

        Process * newProcess = new Process;
        newProcess->fileName = fileName;
        newProcess->pid = pid;
        newProcess->inMain = true;
        newProcess->size = size*1024;
        int count=pages;
        while(count--){
            int freeIndex = getFirstFreeIndex(isFreeMain);
            newProcess->pageTable.push_back(freeIndex);
        }
        processMap[pid] = newProcess;
        freePagesMain -= pages;
        lru.push_back(pid);
        outputFile<<fileName<<" is loaded in main memory and is assigned process id "<<pid<<endl;
    }
    else if(freePagesVirtual >= pages){
        int pid = pidCounter;
        pidCounter ++;

        Process * newProcess = new Process;
        newProcess->fileName = fileName;
        newProcess->pid = pid;
        newProcess->inMain = false;
        newProcess->size = size*1024;
        int count=pages;
        while(count--){
            int freeIndex = getFirstFreeIndex(isFreeVirtual);
            newProcess->pageTable.push_back(freeIndex);
        }
        processMap[pid] = newProcess;
        freePagesVirtual -= pages;

        outputFile<<fileName<<" is loaded in virtual memory and is assigned process id "<<pid<<endl;
    }
    else{
        outputFile<<fileName<<" could not be loaded - memory is full" << endl;
        inputFile.close();
        return ;
    }
    
    inputFile.close();
    return ;
}

void run(int pid, ofstream &outputFile){
    flag = false;
    if((processMap.find(pid) != processMap.end()) == false){
        outputFile<<"Error given pid doesn't exits"<<endl;
        return;
    }

    if(processMap[pid]->inMain == false){

        if(freePagesMain >= (processMap[pid]->pageTable.size())){
            swapin(pid,outputFile);
        }
        else{
            swapin(pid,outputFile);
            if(flag == false){
                return;
            }
        }

    }
    else{
        //if main memory
        int index = -1;
        for(int i=0;i<lru.size();i++){
            if(lru[i] == pid){
                index = i;
                break;
            }
        }
        if(index != -1){
            lru.erase(lru.begin() + index);
        }
        lru.push_back(pid);
    }

    string processName = processMap[pid]->fileName;
    ifstream inputFile;
    inputFile.open(processName);
    
    string currentLine;
    getline(inputFile,currentLine);
    
    while(getline(inputFile,currentLine)){
        stringstream toDivide(currentLine);
        string data;
        getline(toDivide,data,' ');
        if(data == "load"){
            int integer,address;
            getline(toDivide,data,' ');
            int len=data.length();
            string temp = "";
            for(int i=0;i<len-1;i++){
                temp += data[i];
            }
            integer = stoi(temp);
            getline(toDivide,data,' ');
            address=stoi(data);

            if(address >= processMap[pid]->size){
                outputFile<<"Invalid Memory Address "<< address << " specified for process id "<<pid<<endl;

                inputFile.close();
                return;
            }

            int vpn = address/P;
            int offset = address - vpn*P;
            int pfn = processMap[pid]->pageTable[vpn];
            int phyAd=pfn*P + offset;
            mainMemory[phyAd] = {true,integer};

            outputFile<<"Command: load a, y; Result: Value of "<<integer<<" is now stored in addr "<<address<<endl;
        }
        else if(data == "add"){
            int ad1,ad2,ad3;
            getline(toDivide,data,' ');
            int len=data.length();
            string temp = "";
            for(int i=0;i<len-1;i++){
                temp += data[i];
            }
            ad1=stoi(temp);
            getline(toDivide,data,' ');
            len=data.length();
            temp = "";
            for(int i=0;i<len-1;i++){
                temp += data[i];
            }
            ad2=stoi(temp);
            getline(toDivide,data,' ');
            ad3 = stoi(data);

            if(ad1 >= processMap[pid]->size){
                outputFile<<"Invalid Memory Address "<< ad1 << " specified for process id "<<pid<<endl;

                inputFile.close();
                return;
            }
            if(ad2 >= processMap[pid]->size){
                outputFile<<"Invalid Memory Address "<< ad2 << " specified for process id "<<pid<<endl;

                inputFile.close();
                return;
            }
            if(ad3 >= processMap[pid]->size){
                outputFile<<"Invalid Memory Address "<< ad3 << " specified for process id "<<pid<<endl;

                inputFile.close();
                return;
            }

            int vpn = ad1/P;
            int offset = ad1 - vpn*P;
            int pfn = processMap[pid]->pageTable[vpn];
            int phyAd=pfn*P + offset;

            int x = mainMemory[phyAd].second;

            vpn = ad2/P;
            offset = ad2-vpn*P;
            pfn = processMap[pid]->pageTable[vpn];
            phyAd = pfn*P + offset;

            int y = mainMemory[phyAd].second;

            vpn = ad3/P;
            offset = ad3-vpn*P;
            pfn = processMap[pid]->pageTable[vpn];
            phyAd = pfn*P + offset;
            mainMemory[phyAd] = {true,(x+y)};

            outputFile<<"Command: add x, y, z; Result: Value in addr x = "<<x<<", addr y = "<<y<<", addr z = "<<(x+y)<<endl;
        }
        else if(data == "sub"){
            int ad1,ad2,ad3;
            getline(toDivide,data,' ');
            int len=data.length();
            string temp = "";
            for(int i=0;i<len-1;i++){
                temp += data[i];
            }
            ad1=stoi(temp);
            getline(toDivide,data,' ');
            len=data.length();
            temp = "";
            for(int i=0;i<len-1;i++){
                temp += data[i];
            }
            ad2=stoi(temp);
            getline(toDivide,data,' ');
            ad3 = stoi(data);

            if(ad1 >= processMap[pid]->size){
                outputFile<<"Invalid Memory Address "<< ad1 << " specified for process id "<<pid<<endl;

                inputFile.close();
                return;
            }
            if(ad2 >= processMap[pid]->size){
                outputFile<<"Invalid Memory Address "<< ad2 << " specified for process id "<<pid<<endl;

                inputFile.close();
                return;
            }
            if(ad3 >= processMap[pid]->size){
                outputFile<<"Invalid Memory Address "<< ad3 << " specified for process id "<<pid<<endl;

                inputFile.close();
                return;
            }

            int vpn = ad1/P;
            int offset = ad1 - vpn*P;
            int pfn = processMap[pid]->pageTable[vpn];
            int phyAd=pfn*P + offset;

            int x = mainMemory[phyAd].second;

            vpn = ad2/P;
            offset = ad2-vpn*P;
            pfn = processMap[pid]->pageTable[vpn];
            phyAd = pfn*P + offset;

            int y = mainMemory[phyAd].second;

            vpn = ad3/P;
            offset = ad3-vpn*P;
            pfn = processMap[pid]->pageTable[vpn];
            phyAd = pfn*P + offset;
            mainMemory[phyAd] = {true,(x-y)};

            outputFile<<"Command: sub x, y, z; Result: Value in addr x = "<<x<<", addr y = "<<y<<", addr z = "<<(x-y)<<endl;
        }
        else if(data == "print"){
            int ad;
            getline(toDivide,data,' ');
            ad = stoi(data);

            if(ad >= processMap[pid]->size){
                outputFile<<"Invalid Memory Address "<< ad << " specified for process id "<<pid<<endl;

                inputFile.close();
                return;
            }

            int vpn = ad/P;
            int offset = ad - vpn*P;
            int pfn = processMap[pid]->pageTable[vpn];
            int phyAd=pfn*P + offset;
            
            outputFile<<"Command: print x ; Result: Value in addr "<<ad<<" = "<<mainMemory[phyAd].second<<endl;
        }
        else{
            inputFile.close();
            return;
        }
    }

    inputFile.close();
    return;
}

void kill(int pid, ofstream &outputFile){ //still have to clear the mainMemory;

    if((processMap.find(pid) != processMap.end()) == false){
        outputFile<<"Error given pid doesn't exits"<<endl;
        return;
    }

    if(processMap[pid]->inMain == true){
        int pages = (processMap[pid]->pageTable).size();
        for(int i=0;i<pages;i++){
            isFreeMain[(processMap[pid]->pageTable)[i]] = true;
            int phyAddrStart = ((processMap[pid]->pageTable)[i])*P;
            int phyAddrEnd = phyAddrStart + P - 1;
            for(int j=phyAddrStart;j<=phyAddrEnd;j++){
                mainMemory[j] = {false,0};
            }
        }
        freePagesMain += pages;

        int index = -1;
        for(int i=0;i<lru.size();i++){
            if(lru[i] == pid){
                index = i;
                break;
            }
        }
        lru.erase(lru.begin() + index);
    }
    else{
        int pages = (processMap[pid]->pageTable).size();
        for(int i=0;i<pages;i++){
            isFreeVirtual[(processMap[pid]->pageTable)[i]] = true;
            int phyAddrStart = ((processMap[pid]->pageTable)[i])*P;
            int phyAddrEnd = phyAddrStart + P - 1;
            for(int j=phyAddrStart;j<=phyAddrEnd;j++){
                virtualMemory[j] = {false,0};
            }
        }
        freePagesVirtual += pages;
    }
    processMap.erase(pid);

    outputFile<<"Killed the process of pid "<<pid<<endl;
    return;
}

void listpr(ofstream &outputFile){
    vector<int> temp1;
    vector<int> temp2;

    for(auto itr:processMap){
        if(itr.second->inMain == true){
            temp1.push_back(itr.first);
        }
        else{
            temp2.push_back(itr.first);
        }
    }

    sort(temp1.begin(),temp1.end());
    sort(temp2.begin(),temp2.end());

    outputFile<<"main memory : ";
    for(auto itr:temp1){
        outputFile<<itr<<" ";
    }
    outputFile<<"virtual memory : ";
    for(auto itr:temp2){
        outputFile<<itr<<" ";
    }
    outputFile<<endl;
}

void pte(int pid, string fileName, ofstream &outputFile){
    
    ofstream outFile(fileName,ios::app);

    if((processMap.find(pid) != processMap.end()) == false){
        outFile<<"Error given pid doesn't exits"<<endl;
        return;
    }

    time_t now = time(nullptr);
    tm * localTime = localtime(&now);
    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;  
    int day = localTime->tm_mday;
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;

    outFile<<"Current date : "<<day<<" "<<month<<" "<<year<<" time : "<<hour<<" "<<minute<<" "<<second<<endl;
    outFile<<"Pagetable of process pid "<<pid<<" :"<<endl;
    int i=0;
    for(auto itr:processMap[pid]->pageTable){
        outFile<<i<<" : "<<itr<<endl;
        i++;
    }

    outFile.close();
}

void pteall(string fileName, ofstream &outputFile){

    ofstream outFile(fileName,ios::app);

    time_t now = time(nullptr);
    tm * localTime = localtime(&now);
    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;  
    int day = localTime->tm_mday;
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;

    outFile<<"Current date : "<<day<<" "<<month<<" "<<year<<" time : "<<hour<<" "<<minute<<" "<<second<<endl;

    vector<int> temp;
    for(auto itr:processMap){
        temp.push_back(itr.first);
    }
    sort(temp.begin(),temp.end());

    for(auto itr:temp){
        outFile<<"Pagetable of process pid "<<itr<<" :"<<endl;
        int i=0;
        for(auto jtr:processMap[itr]->pageTable){
            outFile<<i<<" : "<<jtr<<endl;
            i++;
        }
    }

    outFile.close();
}


void print(int memLocation, int len, ofstream &outputFile){
    int end = memLocation + len - 1;
    if(end >= M*1024){
        outputFile<<"Error given memory locations are out of bounds"<<endl;
        return;
    }

    outputFile<<"Main Memory Entries :"<<endl;
    for(int i=memLocation;i<=end;i++){
        int content;
        if(mainMemory[i].first == true){
            content= mainMemory[i].second;
        }
        else{
            content=0;
        }
        outputFile<<"Value of "<<i<<" : "<<content<<endl;
    }
}
