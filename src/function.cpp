#include "function.h"
#include "basic_block.h"
#include "taint_block.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <queue>

Function::Function (ADDR start, ADDR end, ADDR vStart,
        const char *name) : mapStart(start), mapEnd(end),
    virStart(vStart)
{
    this->funcName = strdup(name);
}

Function::Function (const Function &func)
{
    this->mapStart = func.mapStart;
    this->mapEnd = func.mapEnd;
    this->virStart = func.virStart;
    this->funcName = strdup(func.funcName);
}

Function & Function::operator= (const Function &func)
{
    this->mapStart = func.mapStart;
    this->mapEnd = func.mapEnd;
    this->virStart = func.virStart;
    this->funcName = strdup(func.funcName);
}

Function::~Function ()
{
    free(funcName);
}

ADDR Function::getVirAddr(ADDR mapAddr) const
{
    if (mapAddr <= mapEnd && mapAddr >= mapStart)
        return mapAddr - mapStart + virStart;
    else
        return -1;
}

ADDR Function::getMapAddr(ADDR virAddr) const
{
    if (virAddr >=  virStart && virAddr <= virStart + mapEnd-mapStart)
        return virAddr-virStart+mapStart;
    else 
        return -1;
}

char *Function::toString(char *buf, unsigned size) const
{
    snprintf(buf, size, "%-18s\t0x%016llx\t0x%016llx\t0x%016llx",
            funcName, virStart, mapStart, mapEnd);
    return buf;
}

void Function::addBasicBlock(BasicBlock *bb)
{
    ADDR addr = bb->getFirstInstAddr();
    BasicBlock *pos = hashTableBB[hash(addr)];
    if (!pos) {
        hashTableBB[hash((ADDR)bb)] = bb;
    } else {
        while (pos->getHashNext()) {
            pos = pos->getHashNext();
        }
        pos->setHashNext(bb);
    }
}

void Function::disasm()
{
    //printf("\tStart Function::disasm %s\n", funcName);
    queue<BasicBlock *> que;
    BasicBlock *bb = findBasicBlock(mapStart);
    que.push(bb);
    bb->setDisasmPending(true);
    while (!que.empty()) {
        bb = que.front();
        bb->disasm();
        const vector<BasicBlock *> &succ = bb->getSuccessors();
        for (BasicBlock::ConstBBPtrIter iter = succ.begin(); iter != succ.end(); iter++) {
            if ((*iter)->isDisasmed() || (*iter)->isDisasmPending())
                continue;
            que.push(*iter);
            (*iter)->setDisasmPending(true);
        }
        que.pop();
        bb->setDisasmPending(false);
    }
}

BasicBlock *Function::findBasicBlock(ADDR addr)
{
    BasicBlock *pos = hashTableBB[hash(addr)];
    while (pos) {
        if (pos->getFirstInstAddr() == getVirAddr(addr))
            break;
        pos = pos->getHashNext();
    }

    if (!pos) {
        pos = new BasicBlock(addr, getVirAddr(addr), this);
        basicBlocks.push_back(pos);
        BasicBlock *bb = hashTableBB[hash(addr)];
        pos->setHashNext(bb);
        hashTableBB[hash(addr)] = pos;
    }
    return pos;
}

void Function::dumpBiref() const
{
    printf("\t\033[32m%-18s\033[m 0x%llx ~ 0x%llx %ld basic blocks candidate pairs %ld\n", funcName, virStart, getVirAddr(mapEnd), basicBlocks.size(), pairBBs.size());
}

long Function::mergeBB()
{
    long count = 0;
    for (set<BasicBlock *>::iterator iter = pairBBs.begin(); iter!=pairBBs.end(); iter++) {
        BasicBlock* bbptr = *iter;
        
        assert(bbptr->getSuccessors().size() == 2);
        TaintBlock *left = new TaintBlock(bbptr->getSuccessors().front());
        TaintBlock *right = new TaintBlock(bbptr->getSuccessors().back());
        if (*left == *right) {
            bbptr->dump();
            left->dump();
            bbptr->getSuccessors().front()->dump();
            right->dump();
            bbptr->getSuccessors().back()->dump();
            count++;
        }
        free(left);
        free(right);
    }
    return count;
}

void Function::findCandidatePair()
{
   // for (BasicBlock::ConstRevBBPtrIter iter = leafBBs.rbegin(); iter!=leafBBs.rend(); iter++) {
    for (BasicBlock::BBPtrIter iter = basicBlocks.begin(); iter!=basicBlocks.end(); iter++) {
        BasicBlock* bbptr = *iter;
        if (!bbptr->isLeaf())
            continue;
        if (bbptr->isVisited())
            continue;
        const vector<BasicBlock *> preds = bbptr->getPredecessors();
        if (preds.size() != 1)
            continue;
        BasicBlock *pred = preds.front();
        if (pred->getSuccessors().size() != 2)
            continue;
        pairBBs.insert(pred);
    }
}

void Function::reduction()
{
    bool change = false;
    do {
        change = false;
        for (BasicBlock::BBPtrIter iter = basicBlocks.begin(); iter!=basicBlocks.end(); iter++) {
            BasicBlock* bbptr = *iter;
            if (bbptr->isRemoved()) {
                basicBlocks.erase(iter);
                break;
            }
            const vector<BasicBlock *> succs = bbptr->getSuccessors();
            if (succs.size() != 1)
                continue;
            BasicBlock *succ = succs.front();
            if (succ->getPredecessors().size() != 1)
                continue;
            
            bbptr->merge(succ);
            succ->setRemoved(true);
            change = true;
        }
    } while (change);
}
