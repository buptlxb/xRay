#ifndef _TAINT_BLOCK_H
#define _TAINT_BLOCK_H

#include <vector>
#include <map>
#include <set>
#include "BeaEngine.h"
#include "basic_block.h"

using namespace std;

class TaintBlock {
    private:
        vector<BasicBlock *> bBlocks;
        vector<TaintBlock *> successors;
        vector<TaintBlock *> predecessors;
        map<ARGTYPE, ARGTYPE> UDPairs;
        set<ARGTYPE> use;
        set<ARGTYPE> define;
        bool memAccess;
        bool callFork;
    public:
        typedef set<ARGTYPE>::const_iterator ConstARGIter;
        TaintBlock(BasicBlock *bb);
        bool isMemAccess() const { return memAccess; }
        void setMemAccess(bool ma) { memAccess = ma; }
        bool iscallFork() const { return callFork; }
        void setCallFork(bool cf) { callFork = cf; }
        char *toString(char *buf, int size) const;
        void dump() const;
        bool operator== (const TaintBlock &that) const;
        const vector<BasicBlock *> &getBBlocks() const {
            return bBlocks;
        }
};

#endif
