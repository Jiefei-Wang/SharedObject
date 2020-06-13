#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H
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
void allocateSharedMemory(const char *name, size_t size_in_byte);
void *mapSharedMemory(const char *name);
bool unmapSharedMemory(const char *name);
bool freeSharedMemory(const char *name);
bool hasSharedMemory(const char *name);
double getSharedMemorySize(const char *name);

#endif
