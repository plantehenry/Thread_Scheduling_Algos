#ifndef _ext2_h_
#define _ext2_h_

#include "ide.h"
#include "atomic.h"
#include "debug.h"

extern char GDTBuff[]; 

// A wrapper around an i-node
class Node : public BlockIO { // we implement BlockIO because we
                              // represent data
    uint32_t block_group;
    uint32_t index;
    uint32_t containing_block;
    uint32_t block_bitmap_addr;
    uint32_t inode_bitmap_addr;
    uint32_t itable_addr;
    char* inodeBuff;
    uint32_t type;
 
    uint32_t hard_links;
    uint32_t total_blocks;
    Ide* ide;
public:
   uint32_t size;

    // i-number of this node
    const uint32_t number;

    Node(uint32_t block_size, uint32_t number, uint32_t inodes_per_group, Ide* ide, uint32_t total_blocks) : BlockIO(block_size), number(number) {
        this->ide = ide;
        this->total_blocks = total_blocks;
        block_group = (number - 1) / inodes_per_group;
        index = (number - 1) % inodes_per_group;
        containing_block = (index * 128) / block_size;
        block_bitmap_addr = ((int*)GDTBuff)[8 * block_group];
        inode_bitmap_addr = ((int*)GDTBuff)[8 * block_group + 1];
        itable_addr = ((int*)GDTBuff)[8 * block_group + 2];
        //itable_addr * block_size + containing_block * block_size - 1
        inodeBuff = new char[128];
        //correct offset? dont use containing block
        //ide->read_all(itable_addr * block_size + (containing_block) * block_size + index * 128, 128, inodeBuff);
        ide->read_all(itable_addr * block_size + index * 128, 128, inodeBuff);
        // type calulation prolly wrong
        // mask out 12 bitss
        

        type = ((unsigned short)((short*)inodeBuff)[0])>>12;
        // size calulation might bw wrong should be lower 32 bits?
        size = ((int*)inodeBuff)[1]; 
        hard_links = ((short*)inodeBuff)[13];
        // Debug::printf("*** type %d\n", type);
        // Debug::printf("*** size %d\n", size);
        // Debug::printf("*** inodespergroup %d\n", inodes_per_group);
        // Debug::printf("*** block_group %d\n", block_group);
        // Debug::printf("*** index %d\n", index);
        // Debug::printf("*** containing_block %d\n", containing_block);
        // Debug::printf("*** block_bitmap_addr %d\n", block_bitmap_addr);
        // Debug::printf("*** itable_addr %d\n", itable_addr);
        // Debug::printf("*** inodebuff");
        // for(int i = 0; i < 128; i++){
        //     Debug::printf("*** %c\n", inodeBuff[i]);
        // }
        
    }

    virtual ~Node() {}

    // How many bytes does this i-node represent
    //    - for a file, the size of the file
    //    - for a directory, implementation dependent
    //    - for a symbolic link, the length of the name
    uint32_t size_in_bytes() override {
        return size;
    }

    // read the given block (panics if the block number is not valid)
    // remember that block size is defined by the file system not the device
    void read_block(uint32_t number, char* buffer) override{
        uint32_t max_block = size/block_size;
        // ******* just going to assume width is 4 can change later if want
        // int width = log(total_blocks);
        // int true_width = pow(2, ceil(log(width))/log(2)));
        int width  = 4;
        int entres_per_block = block_size / width;
        
        if(number <= max_block){
            if(number < 12){
                ide->read_all(((int*)inodeBuff)[10 + number] * block_size, block_size, buffer);
            } else if (number <=  (uint32_t)(12 + entres_per_block - 1)){
                char*  tempBuff =  new  char[block_size];
                //  ***** check  22
                ide->read_all(((int*)inodeBuff)[22] * block_size, block_size, tempBuff);
                int entry = number - 12;
                int offset = entry % entres_per_block;
                int blockNum  = ((int*) tempBuff)[offset];
                ide->read_all( blockNum * block_size, block_size, buffer);
                delete[] tempBuff;
            } else if (number <=  (uint32_t)(12 + entres_per_block + entres_per_block * entres_per_block - 1)){
                char*  tempBuff =  new  char[block_size];
                char*  tempBuff2 =  new  char[block_size];
                ide->read_all(((int*)inodeBuff)[23] * block_size, block_size, tempBuff);
                int entry = number - entres_per_block - 12;
                int which_block = entry / entres_per_block;
                int offset = entry % entres_per_block;
                int tier2_block = which_block / entres_per_block;
                //int tier2_offset = which_block % entres_per_block;
                ide->read_all(((int*)tempBuff)[tier2_block], block_size, tempBuff2);
                int blockNum = ((int*)tempBuff2)[offset];
                ide->read_all(blockNum, block_size, buffer); 
                delete[] tempBuff;
                delete[] tempBuff2;
                               
            } else {
                char*  tempBuff =  new  char[block_size];
                char*  tempBuff2 =  new  char[block_size];
                char*  tempBuff3 =  new  char[block_size];
                ide->read_all(((int*)inodeBuff)[24] * block_size, block_size, tempBuff);
                int entry = number - entres_per_block*entres_per_block - entres_per_block - 12;
                int which_block = entry / entres_per_block;
                int which_tier2_block = which_block / (entres_per_block * entres_per_block);
                ide->read_all(((int*)tempBuff)[which_tier2_block], block_size, tempBuff2);
                int blocks_before = which_tier2_block * entres_per_block * entres_per_block;
                int which_tier3_block = (which_block - blocks_before) /entres_per_block;
                ide->read_all(((int*)tempBuff)[which_tier3_block], block_size, tempBuff3);
                int offset = entry % entres_per_block;
                int blockNum = ((int*)tempBuff3)[offset];
                ide->read_all(blockNum, block_size, buffer);
                delete[] tempBuff;
                delete[] tempBuff2;
                delete[] tempBuff3; 
            }
        } 
    }

    // returns the ext2 type of the node
    uint32_t get_type() {
        // i dont know if this is formated right
        return type;
    }

    // true if this node is a directory
    bool is_dir() {
        return type == 4;
    }

    // true if this node is a file
    bool is_file() {
        return type == 8;
    }

    // true if this node is a symbolic link
    bool is_symlink() {
        return type == 10;
    }

    // If this node is a symbolic link, fill the buffer with
    // the name the link referes to.
    //
    // Panics if the node is not a symbolic link
    //
    // The buffer needs to be at least as big as the the value
    // returned by size_in_byte()
    void get_symbol(char* buffer){
        if(is_symlink()){
            if(size < 60){
                for(uint32_t i = 0; i < size; i++){
                    buffer[i] = inodeBuff[40 + i];
                }
            } else {
                this->read_all(0, size, buffer);
            }
        }
    }

    // Returns the number of hard links to this node
    uint32_t n_links() {
        return hard_links;
    }

    // Returns the number of entries in a directory node
    //
    // Panics if not a directory
    // do i nee to traverse the linked list ?
    uint32_t entry_count(){
        int blocks_to_read = size/block_size;
        if(is_dir()){
            int entries = 0;
            char* tempBuff = new char[block_size];
            for(int i = 0; i < blocks_to_read; i++){
                read_block(i, tempBuff);
                entries += read_dir_block(tempBuff);
            }
            delete[] tempBuff;
            return entries;
        }

        return -1;

        // int entries = 0;
        // int blocks = 12;
        // int blocks_to_read = size/block_size;
        // if(blocks_to_read < 12){
        //     blocks = size/block_size;
        // }
        // char* tempBuff = new char[block_size];
        // for(int j = 0; j < blocks; j++){
        //     ide->read_all(((int*)inodeBuff)[10 + j] * block_size, block_size, tempBuff);
        //     entries += read_dir_block(tempBuff);
        // }

        // // go through the trees
        // int width = log(total_blocks);
        // int true_width = pow(2, ceil(log(width))/log(2)));
        // //https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2#:~:text=next%20%3D%20pow(2%2C%20ceil,the%20nearest%20whole%20number%20power.

        // // first tree
        // if(blocks_to_read > 12){
        //     ide->read_all(((int*)inodeBuff)[22] * block_size, block_size, tempBuff);
        //     int entries_per_node = block_size/true_width;
        //     char *tempBuff2 = new char[block_size];
        //     for(int i = 0; i < min(entries_per_node,  blocks_to_read - 12)){
        //         // ********** assuming directory tree nodes store block nums at ints
        //         ide->read_all( ((int *)tempBuff)[i], block_size, tempBuff2);
        //         entries  += read_dir_block(tempBuff2);
        //     }
        // }  
    }

    // assumes full temp buff
    uint32_t read_dir_block(char * tempBuff){
        int entries = 0;
        uint32_t curByte = 0;
        while(curByte < block_size){

            // ******  question -> when I check that i node is not zero I get wrong answer but 
            // get right answerr when dont check 

            //int inodeNum = ((int *)tempBuff)[curByte];
            //int length = (tempBuff)[curByte + 6];
            short entrySize = ((short*)tempBuff)[curByte / 2 + 2];
            //curByte += 8 + length;
            curByte += entrySize;
            // if(inodeNum != 0){
            entries++;
            // }
        }
        return entries;
    }


};


// This class encapsulates the implementation of the Ext2 file system
class Ext2 {
    // The device on which the file system resides
    Ide* ide;
    uint32_t block_size;
    uint32_t total_inodes;
    uint32_t total_blocks;
    uint32_t blocks_per_group;
    uint32_t inodes_per_group;
    uint32_t block_groups;

    // in ext2 constructor should I access the gdt and save the 
    // two bitmats for future use?

public:
    // The root directory for this file system
    Node* root;
public:
    // Mount an existing file system residing on the given device
    // Panics if the file system is invalid
    Ext2(Ide* ide);

    // Returns the block size of the file system. Doesn't have
    // to match that of the underlying device
    uint32_t get_block_size() {
        return block_size;
    }

    // Returns the actual size of an i-node. Ext2 specifies that
    // an i-node will have a minimum size of 128B but could have
    // more bytes for extended attributes
    uint32_t get_inode_size() {
        return 128;
    }

    // If the given node is a directory, return a reference to the
    // node linked to that name in the directory.
    //
    // Returns a null reference if "name" doesn't exist in the directory
    //
    // Panics if "dir" is not a directory
    Node* find(Node* dir, const char* name);
};

#endif
