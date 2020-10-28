#include <string>
#define BOOST_NO_AUTO_PTR
#include <boost/interprocess/mapped_region.hpp>
#ifdef _WIN32
#include <boost/interprocess/windows_shared_memory.hpp>
#define OS_shared_memory_object windows_shared_memory
#else
#include <boost/interprocess/shared_memory_object.hpp>
#define OS_shared_memory_object shared_memory_object
#endif

class SharedObjectClass
{
private:
    
    //The shared object resources
    //boost::interprocess::OS_shared_memory_object*
    boost::interprocess::OS_shared_memory_object* sharedMemoryHandle = nullptr;
    //boost::interprocess::mapped_region*
    boost::interprocess::mapped_region* mappedRegionHandle = nullptr;
    void* shared_ptr = nullptr;

    std::string key;
    /*The true allocated size of the shared memory.
    It is greater or equal than the required size*/
    size_t size;
    //Wheter the object is allowed to release the memory
    bool ownData = false;
    void openSharedMemoryHandle();
    void freeSharedMemoryHandle();
    void freeMappedRegionHandle();
    size_t getSharedMemorySize();

public:
    static bool hasSharedMemory(std::string key);
    static void freeSharedMemory(std::string key);

public:
    SharedObjectClass(std::string key, size_t size_in_byte = 0);
    ~SharedObjectClass();
    void allocateSharedMemory();
    void *mapSharedMemory();
    void unmapSharedMemory();
    //This function may or may not free the memory depending on <ownData>
    void freeSharedMemory();
    bool getOwnership();
    void setOwnership(bool own);
    size_t getSize();
    void flush();
    
    bool hasSharedMemoryHandle();
    bool hasMappedRegionHandle();
    bool hasPointer();
};