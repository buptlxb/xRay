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
        bool disasmed;
        bool disasm_pending;
        bool visited;
        bool removed;
    public:
        typedef vector<DISASM>::const_iterator ConstDisasmIter;
        typedef vector<DISASM>::const_reverse_iterator ConstRevDisasmIter;
        typedef vector<BasicBlock *>::iterator BBPtrIter;
        typedef vector<BasicBlock *>::const_iterator ConstBBPtrIter;
        typedef vector<BasicBlock *>::const_reverse_iterator ConstRevBBPtrIter;
        BasicBlock(ADDR eip, ADDR vAddr, Function *func);
        //BasicBlock(const BasicBlock &bb);
        //BasicBlock & operator= (const BasicBlock &bb);
        void disasm();
        void addSuccessor(BasicBlock *);
        const vector<BasicBlock *> &getSuccessors() const {
            return successors;
        }
        void addPredecessor(BasicBlock *);
        const vector<BasicBlock *> &getPredecessors() const {
            return predecessors;
        }
        BasicBlock *getHashNext() const {return hashNext;};
        void setHashNext(BasicBlock *next) { hashNext = next;}
        ADDR getFirstInstAddr() const { return insts[0].VirtualAddr;}
        bool isDisasmed() { return disasmed; }
        void setDisasmed(bool isDisasmed) { disasmed = isDisasmed; }
        bool isDisasmPending() { return disasm_pending; }
        void setDisasmPending(bool pending) { disasm_pending = pending; }
        bool isVisited() { return visited; }
        void setVisited(bool visit) { visited = visit; }
        bool isRemoved() { return removed; }
        void setRemoved(bool rm) { removed = rm; }
        char *toString(char *buf, int size) const;
        void dumpBiref() const;
        void dump() const;
        bool isLeaf() const {
            return insts.back().Instruction.BranchType == RetType;
        }
        const vector<DISASM> &getInsts() const { return insts; }
        void merge(BasicBlock *);
};

#endif
