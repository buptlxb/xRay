#include "BeaEngine.h"
#include "basic_block.h"
#include <cstdio>
#include <cstring>

BasicBlock::BasicBlock(ADDR eip, ADDR vAddr, Function *func):
    hashNext(NULL), func(func), disasmed(false), disasm_pending(false)
{
    DISASM MyDisasm;
    memset(&MyDisasm, 0, sizeof(DISASM));
    MyDisasm.EIP = eip;
    MyDisasm.VirtualAddr = vAddr;
    MyDisasm.Archi = 64;
    MyDisasm.Options = ATSyntax|PrefixedNumeral|Tabulation;
    insts.push_back(MyDisasm);
}

#if 0
BasicBlock::BasicBlock(const BasicBlock &bb)
{
    this->insts = bb.insts;
    this->successors = bb.successors;
    this->predecessors = bb.predecessors;
    this->hashNext = bb.hashNext;
}

BasicBlock & operator= (const BasicBlock &bb)
{
    this->insts = bb.insts;
    this->successors = bb.successors;
    this->predecessors = bb.predecessors;
    this->hashNext = bb.hashNext;
}
#endif

void BasicBlock::disasm()
{
    if (isDisasmed())
        return;
    setDisasmed(true);

    DISASM MyDisasm = insts[0];
    insts.pop_back();
    //printf("\t\tStart BasicBlock::disasm 0x%llx\n", (ADDR)this);
    while (1){
        // Fix SecurityBlock 
        MyDisasm.SecurityBlock = func->getEnd() - MyDisasm.EIP;
        // disasm
        int len = Disasm(&MyDisasm);
        if (len == OUT_OF_BLOCK) {
            puts("\t\t\tOUT_OF_BLOCK : Disasm finished");
            return;
        }else if (len == UNKNOWN_OPCODE) {
            fprintf(stderr, "unknown opcode\n");
            return;
        }else {
            if((MyDisasm.Instruction.Category&0xffff)==CONTROL_TRANSFER && MyDisasm.Instruction.BranchType) {
                switch (MyDisasm.Instruction.BranchType) {
                    case CallType:
                        break;
                    case JO:
                    case JC:
                    case JE:
                    case JA:
                    case JS:
                    case JP:
                    case JL:
                    case JG:
                    //case JB:
                    case JECXZ:
                    case JNO:
                    case JNC:
                    case JNE:
                    case JNA:
                    case JNS:
                    case JNP:
                    case JNL:
                    case JNG:
                    //case JNB:
                        {
                            ADDR mAddr = MyDisasm.EIP + len;
                            if ((ADDR)-1 != func->getVirAddr(mAddr)) {
                                BasicBlock *succ = func->findBasicBlock(mAddr);
                                addSuccessor(succ);
                                succ->addPredecessor(this);
                            }
                        }
                    case JmpType:
                        if (MyDisasm.Instruction.AddrValue != 0) {
                            ADDR mAddr = func->getMapAddr(MyDisasm.Instruction.AddrValue);
                            if ((ADDR)-1 != mAddr) {
                                BasicBlock *succ = func->findBasicBlock(mAddr);
                                addSuccessor(succ);
                                succ->addPredecessor(this);
                            }
                        }
                    case RetType:
                        //puts("\t\t\tCONTROL_TRANSFER : Disasm finished");
                        //char buf[1024];
                        //printf("\t\t%s\n\n", toString(buf, sizeof(buf)));
                        insts.push_back(MyDisasm);
                        return;
                    default:
                        fprintf(stderr, "unknown branchtype %d (0x%llx) in Basic Block 0x%llx\n", MyDisasm.Instruction.BranchType, (ADDR)MyDisasm.VirtualAddr, getFirstInstAddr());
                        return;
                }
            }

            insts.push_back(MyDisasm);
            MyDisasm.EIP = MyDisasm.EIP + len;
            MyDisasm.VirtualAddr = MyDisasm.VirtualAddr + len;
	
            if(MyDisasm.EIP >= func->getEnd()){
                //puts("\t\t\tFUNCTION END : Disasm finished");
                char buf[1024];
                //printf("\t\t%s\n\n", toString(buf, sizeof(buf)));
                return;
            }
        }
    }
}

void BasicBlock::addSuccessor(BasicBlock *bb)
{
    successors.push_back(bb);
}

void BasicBlock::addPredecessor(BasicBlock *bb)
{
    predecessors.push_back(bb);
}

const vector<BasicBlock *>& BasicBlock::getSuccessors() const
{
    return successors;
}

char *BasicBlock::toString(char *buf, int size) const
{
    snprintf(buf, size, "\t\t\t0x%llx(0x%llx) %ld instructions", (ADDR)this, getFirstInstAddr(), insts.size());

    if (!successors.empty()) {
        snprintf(buf+strlen(buf), size-strlen(buf), "\n\t\t\tsucc{");
        for (ConstBBPtrIter iter = successors.begin(); iter != successors.end(); iter++) {
            snprintf(buf+strlen(buf), size-strlen(buf), "0x%llx(0x%llx), ", (ADDR)(*iter), (*iter)->getFirstInstAddr());
        }
        snprintf(buf+strlen(buf)-2, size-strlen(buf)+2, "}");
    }

    if (!predecessors.empty()) {
        snprintf(buf+strlen(buf), size-strlen(buf), "\n\t\t\tpred{");
        for (ConstBBPtrIter iter = predecessors.begin(); iter != predecessors.end(); iter++) {
            snprintf(buf+strlen(buf), size-strlen(buf), "0x%llx(0x%llx), ", (ADDR)(*iter), (*iter)->getFirstInstAddr());
        }
        snprintf(buf+strlen(buf)-2, size-strlen(buf)+2, "}");
    }
    return buf;
}

void BasicBlock::dumpBiref() const
{
    char buf[1024];
    printf("%s\n", toString(buf, sizeof(buf)));
}

void BasicBlock::dump() const
{
    dumpBiref();
    for (ConstDisasmIter iter = insts.begin(); iter != insts.end(); iter++) {
    printf("\t\t\t0x%.8lx\t%s\n", iter->VirtualAddr, (char *)&iter->CompleteInstr);
    }
    puts("");
}
