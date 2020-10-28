#ifndef HEADER_SHARED_MEMORY
#define HEADER_SHARED_MEMORY
#include <string>

/* 
Allocate the shared memory, return the data id
If you do not provide the data id, it will be generated automatically.
If you explicitly specify the data id, the data id must not have been used.
*/
std::string allocateSharedMemory(unsigned long long int size_in_byte, std::string id = "");
/* 
Map the shared memory to the current process memory space
return a pointer that points to the shared memory
*/
void* mapSharedMemory(std::string id);
/*
Unmap the shared memory
The memory may still exist after unmapping unless you turn auto release on.

*/
void unmapSharedMemory(std::string id);
// Unmap and destroy the shared memory. The data will no longer exist after it.
void freeSharedMemory(std::string id);

// The size of the shared memory in bytes
// It can be greater than what your required size!
double getSharedMemorySize(std::string id);
bool hasSharedMemory(std::string id);
bool isSharedMemoryMapped(std::string id);

/*
Allow the shared memory to free its memory after ummapping.
This function should be called after <mapSharedMemory>
*/
bool autoReleaseAfterUse(std::string id);
void autoReleaseAfterUse(std::string id, bool releaseAfterUse);

//The index of the last auto-generated id
uint64_t getLastIndex();
/*
This function will be automatically called by the package.
You do not need to call it unless you have manually released
the package data
*/
void initialPkgData();
void releasePkgData();

#endif
