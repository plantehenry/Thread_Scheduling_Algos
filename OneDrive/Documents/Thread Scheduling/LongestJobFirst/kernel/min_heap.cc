#include "min_heap.h"
struct tester{
    int predTime;
    tester(int val){
        predTime = val;
    }
};
void testHeap()
{	

    MinHeap<tester> h(11);
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
    Debug::printf(" %x\n", h.extractMin());
	tester three = {3};
	tester two = {2};
	tester one = {1};
	tester fifteen = {15};
	tester five = {5};
	tester four = {4};
	tester fortyfive = {45};
    h.insertKey(&three);
    h.printHeap();
    h.insertKey(&two);
    h.printHeap();
    //h.deleteKey(&one,1);
    h.insertKey(&fifteen);
    h.printHeap();
    h.insertKey(&five);
    h.printHeap();
    h.insertKey(&four);
    h.printHeap();
    h.insertKey(&two);
    h.printHeap();
    h.insertKey(&fortyfive);
    h.printHeap();
    h.insertKey(&one);
    h.printHeap();
    h.insertKey(&one);
    h.printHeap();
    h.insertKey(&one);
    h.printHeap();
    Debug::printf(" %d\n", *h.extractMin());
    h.printHeap();
    Debug::printf("%d\n", *h.extractMin());
    h.printHeap();
    Debug::printf("%d\n", *h.extractMin());
    h.printHeap();
    Debug::printf("%d\n", *h.extractMin());
    h.printHeap();
    Debug::printf("%d\n", *h.extractMin());
    h.printHeap();
    Debug::printf("%d\n", *h.extractMin());
    Debug::printf("%d\n", *h.extractMin());
    Debug::printf("%d\n", *h.extractMin());
    Debug::printf("%d\n", *h.extractMin());
    Debug::printf("%d\n", *h.extractMin());
    Debug::printf("%d\n", h.extractMin());
    Debug::printf("%d\n", h.extractMin());
    Debug::printf("%d\n", h.extractMin());
    Debug::printf("%d\n", h.extractMin());
    Debug::printf("%d\n", h.extractMin());
    Debug::printf("%d\n", h.extractMin());
}