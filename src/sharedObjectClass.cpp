#include <map>
#include <string>
#include "sharedObjectClass.h"
#include <Rinternals.h>
#include "utils.h"

using std::string;
using namespace boost::interprocess;
/*
========================================================
               utilities
========================================================
*/
static boost::interprocess::permissions initPermission()
{
    boost::interprocess::permissions permission;
    permission.set_unrestricted();
    return permission;
}
static boost::interprocess::permissions unrestricted_permission = initPermission();

static uint64_t get_size(boost::interprocess::OS_shared_memory_object *obj)
{
#ifdef _WIN32
    return obj->get_size();
#else
    offset_t size;
    obj->get_size(size);
    return size;
#endif
}

#ifdef __linux__
/*Checking the memory status on Linux
		If the shared memory is not allocated successfully,
		boost will not throw an error, we need to manually check if
		the memory can be allocated or not.
*/
//Check with long jump
#include <signal.h>
#include <setjmp.h>
#include <sys/statvfs.h>
static jmp_buf reset;
static void (*old_handle)(int);
static void termination_handler(int signum)
{
    longjmp(reset, 1);
}

//Check with free size
size_t getFreeMemorySize()
{
    std::string path = getSharedMemoryPath();
    if (path!="")
    {
        struct statvfs buf;
        int status = statvfs(path.c_str(), &buf);
        if (status == 0)
        {
            size_t free_Bytes = buf.f_bsize * buf.f_bfree;
            return free_Bytes;
        }
    }
    return SIZE_MAX;
}

static void validate_shared_memory(std::string key,
                                   boost::interprocess::OS_shared_memory_object *sharedMemoryHandle)
{
    boost::interprocess::offset_t size;
    sharedMemoryHandle->get_size(size);
    size_t free_size = getFreeMemorySize();
    if (size > free_size)
    {
        throwError("Insufficient memory size(request: %lluB, available: %lluB)",
                   (uint64_t)size, (uint64_t)free_size);
    }
    mapped_region region(*sharedMemoryHandle, read_write);
    void *ptr = region.get_address();
    if (setjmp(reset) == 0)
    {
        old_handle = signal(SIGBUS, termination_handler);
        if (old_handle != SIG_ERR)
        {
            memset(ptr, 0, size);
            signal(SIGBUS, old_handle);
        }
    }
    else
    {
        signal(SIGBUS, old_handle);
        Rf_error(
            "Testing shared memory failed, key: %s, size: %lluB.\n"
            "This is a fatal error, please consider saving your data "
            "and restarting R to avoid the possible data loss.",
            key.c_str(), (uint64_t)size);
    }
}
#endif
/*
========================================================
               private functions
========================================================
*/
void SharedObjectClass::openSharedMemoryHandle()
{
    sharedMemoryPrint("opening existing shared memory, key:%s, size:%llu\n", key.c_str(), size);
    if (hasSharedMemoryHandle())
    {
        throwError("The shared memory has been opened, this should not happen.");
    }
    try
    {
        sharedMemoryHandle = new OS_shared_memory_object(open_only, key.c_str(), read_write);
    }
    catch (const std::exception &ex)
    {
        freeSharedMemoryHandle();
        throwError("An error has occured in opening shared memory:\n %s", ex.what());
    }
    size = get_size(sharedMemoryHandle);
}

void SharedObjectClass::freeSharedMemoryHandle()
{
    if (sharedMemoryHandle != nullptr)
        delete sharedMemoryHandle;
    sharedMemoryHandle = nullptr;
}
void SharedObjectClass::freeMappedRegionHandle()
{
    if (mappedRegionHandle != nullptr)
        delete mappedRegionHandle;
    mappedRegionHandle = nullptr;
    shared_ptr = nullptr;
}
size_t SharedObjectClass::getSharedMemorySize()
{
    if (hasSharedMemoryHandle())
    {
        return get_size(sharedMemoryHandle);
    }
    else
    {
        try
        {
            OS_shared_memory_object sharedMemory(open_only, key.c_str(), read_write);
            return get_size(&sharedMemory);
        }
        catch (const std::exception &ex)
        {
            throwError("Failed to get the size of the shared memory:\n %s", ex.what());
            return 0;
        }
    }
}

/*
========================================================
               static public functions
========================================================
*/
bool SharedObjectClass::hasSharedMemory(string key)
{
    try
    {
        OS_shared_memory_object sharedMemory(open_only, key.c_str(), read_write);
        return true;
    }
    catch (const std::exception &ex)
    {
        return false;
    }
}
void SharedObjectClass::freeSharedMemory(std::string key)
{
    sharedMemoryPrint("freeing shared memory, key:%s\n",
                      key.c_str());
#ifndef _WIN32
    OS_shared_memory_object::remove(key.c_str());
#endif
}

/*
========================================================
               public member functions
========================================================
*/
SharedObjectClass::SharedObjectClass(std::string key, size_t size_in_byte) : key{key}, size{size_in_byte}
{
}
SharedObjectClass::~SharedObjectClass()
{
    freeSharedMemory();
}

void SharedObjectClass::allocateSharedMemory()
{
    sharedMemoryPrint("allocating shared memory, key:%s, size:%llu\n", key.c_str(), size);
    if (hasSharedMemoryHandle())
    {
        return;
    }
    try
    {
#ifdef _WIN32
        sharedMemoryHandle = new windows_shared_memory(create_only, key.c_str(),
                                                       read_write, size>0?size:1, unrestricted_permission);
#else
        sharedMemoryHandle = new shared_memory_object(create_only, key.c_str(),
                                                      read_write, unrestricted_permission);
        sharedMemoryHandle->truncate(size>0?size:1);
#endif

#ifdef __linux__
        validate_shared_memory(key, sharedMemoryHandle);
#endif
    }
    catch (const std::exception &ex)
    {
        if (sharedMemoryHandle != nullptr)
        {
            SharedObjectClass::freeSharedMemory(key);
            freeSharedMemoryHandle();
        }
        throwError("An error has occured in allocating shared memory:\n %s", ex.what());
    }
    size = get_size(sharedMemoryHandle);
}

void *SharedObjectClass::mapSharedMemory()
{
    sharedMemoryPrint("mapping shared memory, key:%s\n", key.c_str());
    if (hasPointer())
    {
        return shared_ptr;
    }
    else
    {
        if (hasMappedRegionHandle())
        {
            shared_ptr = mappedRegionHandle->get_address();
            return shared_ptr;
        }
    }

    try
    {
        // Check if the shared memory object has been created
        // If not, we just open the shared memory
        if (!hasSharedMemoryHandle())
        {
            openSharedMemoryHandle();
        }
        mappedRegionHandle = new mapped_region(*sharedMemoryHandle, read_write);
        shared_ptr = mappedRegionHandle->get_address();
        return shared_ptr;
    }
    catch (const std::exception &ex)
    {
        freeMappedRegionHandle();
        throwError("An error has occured in mapping shared memory:\n %s", ex.what());
        return nullptr;
    }
}

void SharedObjectClass::unmapSharedMemory()
{
    sharedMemoryPrint(
        "unmapping shared memory, key:%s\n",
        key.c_str());
    freeMappedRegionHandle();
}

void SharedObjectClass::freeSharedMemory()
{
    if (hasMappedRegionHandle())
    {
        freeMappedRegionHandle();
    }
    freeSharedMemoryHandle();
    if (ownData)
        SharedObjectClass::freeSharedMemory(key);
}

bool SharedObjectClass::getOwnership()
{
    return ownData;
}

void SharedObjectClass::setOwnership(bool own)
{
    ownData = own;
}

size_t SharedObjectClass::getSize()
{
    return size;
}
void SharedObjectClass::flush()
{
    if (hasMappedRegionHandle())
    {
        mappedRegionHandle->flush();
    }
}

bool SharedObjectClass::hasSharedMemoryHandle()
{
    return sharedMemoryHandle != nullptr;
}
bool SharedObjectClass::hasMappedRegionHandle()
{
    return mappedRegionHandle != nullptr;
}

bool SharedObjectClass::hasPointer()
{
    return shared_ptr != nullptr;
}
