#ifndef _FUNCTION_H
#define _FUNCTION_H

#include <vector>
#include "basic_block.h"
#include "type.h"

#define HASH_TABLE_ORDER 6

using namespace std;

class BasicBlock;

class Function {
    private:
        ADDR mapStart;
        ADDR mapEnd;
        ADDR virStart;
        char *funcName;
        vector<BasicBlock*> basicBlocks;
        BasicBlock* hashTableBB[1<<HASH_TABLE_ORDER];
        ADDR hash(ADDR addr) {
            return addr & ((1<<HASH_TABLE_ORDER)-1);
        }
        void addBasicBlock(BasicBlock* bb);

    public:
        Function (ADDR start, ADDR end, ADDR vStart, const char *name); 
        Function (const Function &func);
        Function & operator= (const Function &func);
        ~Function ();
        ADDR getStart() const { return mapStart; }
        ADDR getEnd() const { return mapEnd; }
        ADDR getVirAddr(ADDR mapAddr) const;
        ADDR getMapAddr(ADDR virAddr) const;
        char *toString(char *buf, unsigned size) const;
        void disasm();
        BasicBlock *findBasicBlock(ADDR addr);
        char *getFuncName() { return funcName; }
        void dumpBiref() const;
        const vector<BasicBlock *> &getBasicBlocks() { return basicBlocks; }
};
#endif
