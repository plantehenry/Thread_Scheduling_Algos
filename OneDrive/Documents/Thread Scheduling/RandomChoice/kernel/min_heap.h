#ifndef _min_heap_h_
#define _min_heap_h_

#include "atomic.h"
#include "debug.h"

// make ids
// possibly derefeecning null pointer

// A C++ program to demonstrate common Binary Heap Operations
extern "C" int random();


// help from https://www.geeksforgeeks.org/binary-heap/
// Prototype of a utility function to swap two integers
template <typename J>
void swap(J *x, J *y){
	J temp = *x;
	*x = *y;
	*y = temp;
}

template<typename N>
struct heapEntry
{
	N* tcb;
	int predTime;
	heapEntry(): tcb(nullptr), predTime(0){}
};


// A class for Min Heap
template <typename T>
class MinHeap
{
	T **harr; // pointer to array of elements in heap
	int capacity; // maximum possible size of min heap
	int heap_size; // Current number of elements in min heap
    InterruptSafeLock* lock;
public:
	// Constructor;
	MinHeap(int cap){
		heap_size = 0;
		capacity = cap;
		harr = new T*[cap];
		lock = new InterruptSafeLock();
	}

	// to heapify a subtree with the root at given index
	void MinHeapify(int i){
		int l = left(i);
		int r = right(i);
		int smallest = i;
		if (l < heap_size && harr[l]->predTime < harr[i]->predTime)
			smallest = l;
		if (r < heap_size && harr[r]->predTime < harr[smallest]->predTime)
			smallest = r;
		if (smallest != i)
		{
			swap(&harr[i], &harr[smallest]);
			MinHeapify(smallest);
		}
	}

	int parent(int i) { return (i-1)/2; }

	// to get index of left child of node at index i
	int left(int i) { return (2*i + 1); }

	// to get index of right child of node at index i
	int right(int i) { return (2*i + 2); }

	// to extract the root which is the minimum element
	
	T* extractMin(){
		LockGuard g{*lock};
		// Debug::printf("extract enter %d\n", heap_size);
		if (heap_size <= 0){
			// lock->unlock();
			// Debug::printf("extract exit %d\n", heap_size);
			return nullptr;
		}
		if (heap_size == 1)
		{
			heap_size--;
			T* returnVal = harr[0];
			// lock->unlock();
			// Debug::printf("exit %d\n", heap_size);
			// Debug::printf("extract exit\n");
			return returnVal;
		}

		// Store the minimum value, and remove it from heap
		T* root = harr[0];
		harr[0] = harr[heap_size-1];
		//Debug::printf("heapsize -1 %d",* (harr[heap_size - 1]->tcb));
		heap_size--;
		MinHeapify(0);
		T* returnVal = root;
		// lock->unlock();
		// Debug::printf("exit %d\n", heap_size);
		return returnVal;
	}

	// Decreases key value of key at index i to new_val
	void decreaseKey(int i, int new_val){
		harr[i]->predTime = new_val;
		while (i != 0 && harr[parent(i)]->predTime > harr[i]->predTime)
		{
		swap(&harr[i], &harr[parent(i)]);
		i = parent(i);
		}
	}

	// Returns the minimum key (key at root) from min heap
	T* getMin() { return harr[0]; }

	T* getRandom(){
		// lock->lock();
		LockGuard g{*lock};
		if(heap_size == 0){
			// lock->unlock();
			return nullptr;
		} else {
			uint32_t randomNum = (uint32_t)random();
			// Debug::printf("%u\n", randomNum);
			uint32_t max = 0xffffffff;
			uint32_t sizeOfSegment = max/heap_size;
			int index = randomNum / sizeOfSegment;
			// Debug::printf("*** index %d", index);
			if(index >= heap_size){
				index = heap_size - 1;
			}
			return deleteKey(index);
		}

		
	}

	T* extractMinNoLock(){
		if (heap_size <= 0){
			// lock->unlock();
			return nullptr;
		}
		if (heap_size == 1)
		{
			heap_size--;
			T* returnVal = harr[0];
			// delete harr[0];
			// lock->unlock();
			return returnVal;
		}

		// Store the minimum value, and remove it from heap
		T* root = harr[0];
		harr[0] = harr[heap_size-1];
		//Debug::printf("heapsize -1 %d",* (harr[heap_size - 1]->tcb));
		heap_size--;
		MinHeapify(0);
		T* returnVal = root;
		// delete root;
		return returnVal;
	}

	// Deletes a key stored at index i
	T* deleteKey(int i){
		decreaseKey(i, 	-2147483648);
		return extractMinNoLock();
	}

	// Inserts a new key 'k'
	void insertKey(T* k){
		LockGuard g{*lock};
		// Debug::printf("insert enter %d\n", heap_size);
		// lock->lock();
		if (heap_size == capacity)
		{
			// lock->unlock();
			// Debug::printf("insert exit %d\n", heap_size);
			return;
		}
		// First insert the new key at the end
		heap_size++;
		int i = heap_size - 1;
		harr[i] = k;

		// harr[i]->tcb = k;
		// harr[i]->predTime = predictedTime;
		// Fix the min heap property if it is violated
		
		while (i != 0 && harr[parent(i)]->predTime > harr[i]->predTime)
		{
			// for(int i = 0; i < heap_size; i++){
			// 	Debug::printf("%x ", (harr[i]->predTime));
			// }
			// Debug::printf("\n");
			swap(&harr[i], &harr[parent(i)]);
			i = parent(i);
		}
		// Debug::printf("insert exit %d\n", heap_size);
		// lock->unlock();
	}

	void printHeap(){
		for(int i = 0; i < heap_size; i++){
			Debug::printf("%x ", *(harr[i]->tcb));
		}
		Debug::printf("\n");
	}
};



extern void testHeap();


#endif