#ifndef HEADER_SHARED_MEMORY
#define HEADER_SHARED_MEMORY
#include <string>

/* 
Allocate the shared memory, return a data id corresponding to the shared memory.
If you do not provide the data id, it will be automatically generated.
If you explicitly specify the data id, no exception will be given if 
the id has been used and the its size is not less than your request.
*/
std::string allocateSharedMemory(unsigned long long int size_in_byte, std::string id = "") noexcept(false);
/* 
Map the shared memory to the current process memory space
return a pointer that points to the shared memory
*/
void* mapSharedMemory(std::string id) noexcept(false);
/*
Unmap the shared memory
The memory may still exist after unmapping unless you have auto release on.
*/
void unmapSharedMemory(std::string id);
// Unmap and destroy the shared memory. The data will no longer exist after it.
void freeSharedMemory(std::string id);
/*
The size of the shared memory in bytes
It can be greater than your request size!
*/
double getSharedMemorySize(std::string id) noexcept(false);
bool hasSharedMemory(std::string id);
bool isSharedMemoryMapped(std::string id);

/*
Allow the shared memory to free itself after ummapping.
This function should be called after <mapSharedMemory>
*/
bool autoReleaseAfterUse(std::string id) noexcept(false);
void autoReleaseAfterUse(std::string id, bool releaseAfterUse) noexcept(false);

//The index of the last auto-generated id
uint64_t getLastIndex() noexcept(false);
/*
This function will be automatically called by the package.
You do not need to call it unless you have manually released
the package data
*/
void initialPkgData() noexcept(false);
void releasePkgData();

#endif
