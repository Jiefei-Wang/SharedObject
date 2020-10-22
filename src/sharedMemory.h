#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#ifdef __cplusplus
extern "C"{
#else
#include <stdbool.h>
#endif
int32_t getLastIndex();

// Unamed shared memory
// Allocate the shared memory, return data ID
uint32_t allocateSharedMemory(size_t size_in_byte);
// Map the shared memory to the current process memory space
// return the pointer to the shared memory
void *mapSharedMemory(uint32_t id);
// Unmap the shared memory but the memory may still exist.
// return a bool value indicating whether the operation is success
bool unmapSharedMemory(uint32_t id);
// Unmap and destroy the shared memory. The data will no longer exist after it.
// return a bool value indicating whether the operation is success
bool freeSharedMemory(uint32_t id);
bool hasSharedMemory(uint32_t id);
// The size of the shared memory in bytes
// It can be larger than what your required size!
double getSharedMemorySize(uint32_t id);

//Named shared memory
void allocateNamedSharedMemory(const char *name, size_t size_in_byte);
void *mapNamedSharedMemory(const char *name);
bool unmapNamedSharedMemory(const char *name);
bool freeNamedSharedMemory(const char *name);
bool hasNamedSharedMemory(const char *name);
double getNamedSharedMemorySize(const char *name);

/*
This function will be automatically called by the package.
You do not need to call it unless you have manually released
the package data
*/
void initialPkgData();
void releasePkgData();


#ifdef __cplusplus
}
#endif

#endif
