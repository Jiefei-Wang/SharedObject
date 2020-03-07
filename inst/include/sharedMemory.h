int32_t getLastIndex();

//Auto generate memroy ID
uint32_t allocateSharedMemory(size_t size_in_byte);
//Map the shared memory to the current process memory space
void* mapSharedMemory(uint32_t id);
//Unmap the shared memory but the memory may still exist.
bool closeSharedMemory(uint32_t id);
// Unmap and destroy the shared memory. The data will no longer exist after it.
bool freeSharedMemory(uint32_t id);

//Named shared memory
void allocateSharedMemory(const char* name, size_t size_in_byte);
void* mapSharedMemory(const char* name);
bool closeSharedMemory(const char* name);
bool freeSharedMemory(const char* name);


bool hasSharedMemory(uint32_t id);
bool hasSharedMemory(const char* name);

double getSharedMemorySize(uint32_t id);
double getSharedMemorySize(const char* name);

