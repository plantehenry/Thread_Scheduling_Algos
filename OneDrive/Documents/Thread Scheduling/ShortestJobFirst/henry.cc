#include "ide.h"
#include "ext2.h"
#include "libk.h"


// directory structure has a lot of directories
// so much so that is will fill up the first directory
// entry block
// checks to make sure that the file system
// checks the other direct block pointers for 
// figuring out how many entries there are 


void kernelMain(void) {
    // IDE device #1
    auto ide = new Ide(1);
    
    // We expect to find an ext2 file system there
    auto fs = new Ext2(ide);

    auto root = fs->root;
    Debug::printf("*** contains %d entries\n", root->entry_count());


    // disregard below
    // auto dir15 = fs->find(root,"pneumonoultramicroscopicsilicovolcanoconiosispneumonoultramicroscopicsilicovolcanoconiosis15");
    // auto file = fs->find(dir15, "test.txt");

    
    // auto sz = file->size_in_bytes();
    // auto buffer = new char[sz+1];
    // buffer[sz] = 0;
    // auto cnt = file->read_all(0,sz,buffer);
    // CHECK(sz == cnt);
    // CHECK(K::strlen(buffer) == cnt);
    // // can't just print the string because there is a 1000 character limit
    // // on the output string length.
    // for (uint32_t i=0; i<cnt; i++) {
    //     Debug::printf("%c",buffer[i]);
    // }
    // delete[] buffer;
    // Debug::printf("\n");

}