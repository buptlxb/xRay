#include "binary.h"
#include <cstdlib>
#include <cstdio>
using namespace std;

int main(int argc, char* argv [])
{
    // 1.judge illegal
    if(argc<=1){
        fprintf(stderr, "Usage: ./rop FileList\n");
        abort();
    }
    // 2.read file List and find Ret Code
    vector<BinInfo> vBin;
    puts("[1] BinInfo parse");
    for(int i=1; i<argc; i++){
        BinInfo bin(argv[i]);
        //bin.findRetCodeInElf();
        vBin.push_back(bin);
    }

#if 0
    puts("");
    puts("[2] Dump Function Information");
    for (vector<BinInfo>::iterator iter = vBin.begin(); iter != vBin.end(); iter++) {
        iter->dumpFunc();
    }
#endif
    puts("");
    puts("[2] Disasm Function");
    for (vector<BinInfo>::iterator iter = vBin.begin(); iter != vBin.end(); iter++) {
        iter->disasm();
    }
    return 0;
}
