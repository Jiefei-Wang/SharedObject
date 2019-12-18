#include <Rcpp.h>


int32_t getLastIndex();

//Auto generate memroy ID
uint32_t allocateSharedMemory(size_t size_in_byte);
void* mapSharedMemory(uint32_t id);
void unmapSharedMemory(uint32_t id);
bool freeSharedMemory(uint32_t id);

//Named shared memory
void allocateSharedMemory(const char* name, size_t size_in_byte);
void* mapSharedMemory(const char* name);
void unmapSharedMemory(const char* name);
bool freeSharedMemory(const char* name);


bool hasSharedMemory(uint32_t id);
bool hasSharedMemory(const char* name);

double getSharedMemorySize(uint32_t id);
double getSharedMemorySize(const char* name);

