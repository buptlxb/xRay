#ifndef _BASIC_BLOCK_H
#define _BASIC_BLOCK_H

#include "BeaEngine.h"
#include "type.h"
#include "function.h"
#include <vector>

using namespace std;

class Function;

class BasicBlock {
    private:
        vector<DISASM> insts;
        vector<BasicBlock *> successors;
        vector<BasicBlock *> predecessors;
        BasicBlock *hashNext;
        Function *func;
        bool disasm_flags;
        bool disasm_pending;
    public:
        typedef vector<BasicBlock *>::iterator BBPtrIter;
        typedef vector<BasicBlock *>::const_iterator ConstBBPtrIter;
        BasicBlock(ADDR eip, ADDR vAddr, Function *func);
        //BasicBlock(const BasicBlock &bb);
        //BasicBlock & operator= (const BasicBlock &bb);
        void disasm();
        void addSuccessor(BasicBlock *);
        void addPredecessor(BasicBlock *);
        BasicBlock *getHashNext() const {return hashNext;};
        void setHashNext(BasicBlock *next) { hashNext = next;}
        const vector<BasicBlock *>& getSuccessors() const;
        ADDR getFirstInstAddr() const { return insts[0].VirtualAddr;}
        bool isDisasm() { return disasm_flags; }
        void setDisasm(bool isDisasmed) { disasm_flags = isDisasmed; }
        bool isDisasmPending() { return disasm_pending; }
        void setDisasmPending(bool pending) { disasm_pending = pending; }
        char *toString(char *buf, int size) const;
        void dump() const;
};

#endif
