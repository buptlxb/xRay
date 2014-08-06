#ifndef _BIN_INFO_H
#define _BIN_INFO_H

#include "function.h"
#include "type.h"
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

class BinInfo {
    protected:
        string file_name;//file name
        int fd;//target elf fd num
        void *mapStart;//target file mmap start address
        size_t elf_size;//target file size
        vector<Function> funcInfo;//function information 
        bool isSharedObjectFile;
    protected:
        bool isElf(void *start){
            UINT32 *bs = (UINT32 *)start;
            return (*bs == 0x464c457f);
        }
    public:
        typedef vector<Function>::iterator FunctionIter;
        BinInfo(const char *file_name);
        bool isSharedObject()const {return isSharedObjectFile;}
        string getFileName()const{return file_name;}
        ADDR getMapStart()const{return reinterpret_cast<ADDR>(mapStart);}
        void dumpFunc();
        void disasm();
};

#endif
