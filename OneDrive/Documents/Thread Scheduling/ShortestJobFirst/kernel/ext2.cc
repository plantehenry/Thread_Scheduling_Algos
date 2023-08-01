#include "ext2.h"
#include "block_io.h"

// Ext2

// uint32_t block_size;
// uint32_t total_inodes;
// uint32_t total_blocks;
// uint32_t blocks_per_group;
// uint32_t inodes_per_group;
// uint32_t block_groups;

char GDTBuff[1024] = {}; 

Ext2::Ext2(Ide* ide) {
    this->ide = ide;
    char* superBuff = new char[1024];
    ide->read_all(1024, 1024, superBuff);
    total_inodes = ((int*)superBuff)[0];
    total_blocks = ((int*)superBuff)[1];
    blocks_per_group = ((int*)superBuff)[8];
    inodes_per_group = ((int*)superBuff)[9];
    block_size = 1024 << ((int*)superBuff)[6];
    delete[] superBuff;
    if(block_size == 1024){
        ide->read_all(2 * 1024, 1024, GDTBuff);
    } else {
        ide->read_all(block_size, 1024, GDTBuff);
    }
    root = new Node(block_size, 2,inodes_per_group, ide, total_blocks);
    block_groups = (total_blocks + blocks_per_group - 1) / blocks_per_group;
}

Node* Ext2::find(Node* dir, const char* name) {
    uint32_t numBlocks = dir->size / block_size;
    
    bool found = false;
    char* tempBuff = new char[block_size];
    int inodeNum = -1;

    uint32_t blockNum = 0;
    while(blockNum < numBlocks && !found){
        // read dir block
        dir->read_block(blockNum, tempBuff);

        // traverse 
        uint32_t curByte = 0;
        while(curByte < block_size && !found){
            inodeNum = ((int *)tempBuff)[curByte / 4];
            char length = tempBuff[curByte + 6];
            short entrySize = ((short*)tempBuff)[curByte / 2 + 2];
            char* curName = tempBuff + curByte + 8;
            bool same = true;
            int  index = 0;

            

            //****** question issue 
            // hoe do I know when name ends -> can I assume there is a null
            // terminator? on both
            // otherwise could be lookig for  
            // billybob
            // and find billy and say thats the same
            // Debug::printf("*** thier Name ");
            // for(int i = 0; i < length; i++){
            //     Debug::printf("%c", name[i]);
            // }
            // Debug::printf("\n");
            // Debug::printf("*** cur Name ");
            // for(int i = 0; i < length; i++){
            //     Debug::printf("%c", curName[i]);
            // }
            // Debug::printf("\n");
            while (same && index < length){
                same = name[index] == curName[index];
                index++;
            }
            if(same && name[(int)length] == '\0'){
                found = true;
            }
            //curByte += 8 + length;
            curByte += entrySize;
        }


        blockNum++;
    }

    delete[] tempBuff;
    

    if(found)
    {
        Node * node = new Node(block_size, inodeNum ,inodes_per_group, ide, total_blocks);
        return node;
    }

    return nullptr;
}
