#ifndef _TAINT_BLOCK_H
#define _TAINT_BLOCK_H

class TaintBlock {
    private:
        vector<BasicBlock *> bBlocks;
        vector<TaintBlock *> successors;
        vector<TaintBlock *> predecessors;
        bool memAccess;
        bool callFork;

    public:
};

#endif
