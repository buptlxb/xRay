#include "taint_block.h"
#include <cstring>
#include <cstdio>

#define IS_WRITE_GPR(arg) ((arg).AccessMode == WRITE && (arg).ArgType & GENERAL_REG)
#define IS_READ_GPR(arg) ((arg).AccessMode == READ && (arg).ArgType & GENERAL_REG)

bool operator< (const ARGTYPE &left, const ARGTYPE &right)
{
#if 1
    return left.ArgType < right.ArgType || (left.ArgType == right.ArgType && left.ArgSize < right.ArgSize) || (left.ArgType == right.ArgType && left.ArgSize == right.ArgSize && left.ArgPosition < right.ArgPosition);
#else
    return (left.ArgSize < right.ArgSize) || (left.ArgSize == right.ArgSize && left.ArgPosition < right.ArgPosition);
#endif
}

bool operator== (const ARGTYPE &left, const ARGTYPE &right)
{
    return (left.ArgType == right.ArgType && left.ArgSize == right.ArgSize && left.ArgPosition == right.ArgPosition);
}

bool TaintBlock::operator== (const TaintBlock &that) const
{
    return predecessors == that.predecessors && use == that.use && define == that.define;
}

TaintBlock::TaintBlock(BasicBlock *bb)
{
    if (bb->isVisited())
        return;
    bb->setVisited(true);
    bBlocks.push_back(bb);
    const vector<DISASM> &insts = bb->getInsts();
    for (BasicBlock::ConstRevDisasmIter iter = insts.rbegin(); iter != insts.rend(); iter++) {

//        printf("%s (%d %d %d) (%d %d %d) (%d %d %d)\n", iter->CompleteInstr, iter->Argument1.ArgType, iter->Argument1.ArgSize, iter->Argument1.ArgPosition, iter->Argument2.ArgType, iter->Argument2.ArgSize, iter->Argument2.ArgPosition, iter->Argument3.ArgType, iter->Argument3.ArgSize, iter->Argument3.ArgPosition);
        if (IS_WRITE_GPR(iter->Argument1)) {
            define.insert(iter->Argument1);
        } else if (IS_READ_GPR(iter->Argument1)) {
            use.insert(iter->Argument1);
        }

        if (IS_WRITE_GPR(iter->Argument2)) {
            define.insert(iter->Argument2);
        } else if (IS_READ_GPR(iter->Argument2)) {
            use.insert(iter->Argument2);
        }

        if (IS_WRITE_GPR(iter->Argument3)) {
            define.insert(iter->Argument3);
        } else if (IS_READ_GPR(iter->Argument3)) {
            use.insert(iter->Argument3);
        }
    }
}

char *TaintBlock::toString(char *buf, int size) const
{
    snprintf(buf, size, "\t\t\ttBlock(0x%llx)", (ADDR)this);
    if (!bBlocks.empty()) {
        snprintf(buf+strlen(buf), size-strlen(buf), " Basic Blocks: {");
        for (BasicBlock::ConstBBPtrIter iter = bBlocks.begin(); iter != bBlocks.end(); iter++) {
            snprintf(buf+strlen(buf), size-strlen(buf), "0x%llx, ", (*iter)->getFirstInstAddr());
        }
        snprintf(buf+strlen(buf)-2, size-strlen(buf)+2, "}\n");
    } else {
        snprintf(buf+strlen(buf), size-strlen(buf), "\n");
    }


    if (!use.empty()) {
        snprintf(buf+strlen(buf), size-strlen(buf), "\t\t\tuse: {");
        for (ConstARGIter iter = use.begin(); iter != use.end(); iter++)
        {
            snprintf(buf+strlen(buf), size-strlen(buf), "%s, ", iter->ArgMnemonic);
        }
        snprintf(buf+strlen(buf)-2, size-strlen(buf)+2, "}\n");
    }

    if (!define.empty()) {
        snprintf(buf+strlen(buf), size-strlen(buf), "\t\t\tdefine: {");
        for (ConstARGIter iter = define.begin(); iter != define.end(); iter++)
        {
            snprintf(buf+strlen(buf), size-strlen(buf), "%s, ", iter->ArgMnemonic);
        }
        snprintf(buf+strlen(buf)-2, size-strlen(buf)+2, "}\n");
    }
    return buf;
}

void TaintBlock::dump() const
{
    char buf[512];
    toString(buf, sizeof(buf));
    puts(buf);
}
