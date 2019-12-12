#include <Rcpp.h>


bool hasSharedMemory(uint32_t id);
uint32_t allocateSharedMemory(size_t size_in_byte);
void* mapSharedMemory(uint32_t id);
void unmapSharedMemory(uint32_t id);
bool freeSharedMemory(uint32_t id);




