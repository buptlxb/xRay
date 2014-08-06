#include "function.h"
#include "basic_block.h"
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
            if ((*iter)->isDisasm() || (*iter)->isDisasmPending())
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
    printf("\t\033[32m%-18s\033[m 0x%llx ~ 0x%llx %ld basic blocks\n", funcName, virStart, getVirAddr(mapEnd), basicBlocks.size());
}
