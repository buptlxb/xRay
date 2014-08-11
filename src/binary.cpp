#include "binary.h"
#include "function.h"
#include <elf.h>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <cassert>
#include <fcntl.h>
#include <vector>
#include <cstdio>
#include <cstring>

BinInfo::BinInfo(const char *file_name):file_name(file_name), mapStart(NULL)
{
    fprintf(stderr, "Start processing %s\n", file_name);
    // 1.open file
    int fd = open(file_name, O_RDONLY);
    assert(fd!=-1);
    // 2.calculate elf size
    struct stat statbuf;
    assert(fstat(fd, &statbuf)==0);
    elf_size = statbuf.st_size;
    // 3.mmap elf
    mapStart = mmap(NULL, elf_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(mapStart==MAP_FAILED) {
        perror("Map ELF: ");
        abort();
    }
    // 4.isElf
    assert(isElf(mapStart));
    // 5.judgeIsSharedObject
    Elf64_Ehdr *elf_header = (Elf64_Ehdr*)mapStart;
    Elf64_Half elf_type = elf_header->e_type;
    if(elf_type==ET_EXEC)
        isSharedObjectFile = false;
    else if(elf_type==ET_DYN)
        isSharedObjectFile = true;
    else{
        cerr<<"xRay can only handle execute file and shared object file!"<<endl;
        abort();
    }

    Elf64_Half section_num = elf_header->e_shnum;
    Elf64_Shdr *section_table = (Elf64_Shdr*)((ADDR)mapStart+elf_header->e_shoff);
    char *shstrtab = (char *)((ADDR)mapStart + section_table[elf_header->e_shstrndx].sh_offset);
    char *strtab = NULL;
    for(int i=0; i<section_num; i++) {
        if (!strcmp(shstrtab + section_table[i].sh_name, ".strtab")) {
            strtab = (char *)((ADDR)mapStart + section_table[i].sh_offset);
            //printf("0x%lx\n", section_table[i].sh_offset);
        }
    }
    for(int i=0; i<section_num; i++) {
        if (section_table[i].sh_type != SHT_SYMTAB)
            continue;
        Elf64_Sym *sym_table = (Elf64_Sym *)((ADDR)mapStart+section_table[i].sh_offset);
        Elf64_Xword sym_num = section_table[i].sh_size / section_table[i].sh_entsize;

        for (int i = 0; i < sym_num; i++) {
            if (ELF64_ST_TYPE(sym_table[i].st_info) != STT_FUNC)
                continue;
            if (sym_table[i].st_shndx == SHN_UNDEF || sym_table[i].st_shndx > elf_header->e_shnum)
                continue;
            if (sym_table[i].st_size == 0)
                continue;
            Elf64_Shdr *reside_section = section_table + sym_table[i].st_shndx;
            assert(reside_section->sh_flags & SHF_EXECINSTR);

            ADDR funcVirStart = sym_table[i].st_value;
            ADDR funcMapStart = funcVirStart - reside_section->sh_addr + reside_section->sh_offset + (ADDR)mapStart;
            ADDR funcMapEnd = funcMapStart + sym_table[i].st_size;
            char *funcName = strtab + sym_table[i].st_name;
            //puts(strtab + sym_table[i].st_name);
            Function func(funcMapStart, funcMapEnd, funcVirStart, funcName);
            funcInfo.push_back(func);
        }
    }
}

void BinInfo::dumpFunc()
{
    fprintf(stderr, "%-18s\t%-16s\t%-16s\t%-16s\t\n", "[Function Name]", "[Virtual Addr]", "[Map Start]", "[Map End]");
    char buf[128];
    for (FunctionIter iter = funcInfo.begin(); iter != funcInfo.end(); iter++) {
        fprintf(stderr, "%s\n", iter->toString(buf, sizeof(buf)));
    }
}

void BinInfo::disasm()
{
    long pcount = 0;
    long bcount = 0;
    for (FunctionIter iter = funcInfo.begin(); iter != funcInfo.end(); iter++) {
        iter->disasm();
        //iter->dumpBiref();
        iter->reduction();
        iter->findCandidatePair();
        //iter->dumpBiref();
        pcount += iter->getCandidatePairs().size();
        bcount += iter->mergeBB();
#if 0
        const vector<BasicBlock *> vBB = iter->getBasicBlocks();
        for (BasicBlock::ConstBBPtrIter iter = vBB.begin(); iter != vBB.end(); iter++) {
            (*iter)->dump();
        }
#endif
    }
    printf("Count of Pairs: %ld\n", pcount);
    printf("Count of Equal Pairs: %ld\n", bcount);
}
