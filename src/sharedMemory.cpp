#include <Rcpp.h>
#include <boost/interprocess/mapped_region.hpp>
#include <map>
#include <string>
#include "sharedMemory.h"
#include "tools.h"

#ifdef _WIN32
#define WINDOWS_OS
#endif

#ifdef WINDOWS_OS
#include <boost/interprocess/windows_shared_memory.hpp>
#define OS_shared_memory_object windows_shared_memory
#define OS_SHARED_OBJECT_PKG_SPACE ("Local\\SO_" + OS_ADDRESS_SIZE).c_str()
#else
#include <boost/interprocess/shared_memory_object.hpp>
#define OS_shared_memory_object shared_memory_object
#define OS_SHARED_OBJECT_PKG_SPACE ("SO_" + OS_ADDRESS_SIZE).c_str()
#endif //  WINDOWS_OS

#define SHARED_OBJECT_COUNTER "sharedObjectCounter"
using std::pair;
using std::string;
using namespace boost::interprocess;

static std::map<string, OS_shared_memory_object *> sharedMemoryList;
static std::map<string, mapped_region *> segmentList;
static std::map<string, uint32_t> segmentObjectCount;

// This last_id attributes is shared
static uint32_t *last_id = nullptr;



/*
===================================================================
Utilities
===================================================================
*/
template <class T>
bool keyInMap(T map, std::string key)
{
	return map.find(key) != map.end();
}
//Get the key to access a shared data that is used in R
string getIdKey(uint32_t id)
{
	//common name + id
	return string(OS_SHARED_OBJECT_PKG_SPACE).append("_id_") + std::to_string(id);
}
string getNamedKey(const string name)
{
	//common name + name
	return string(OS_SHARED_OBJECT_PKG_SPACE).append("_nm_") + name;
}
/*
 Get the next id
 The id will be searched starting from *last_id + 1
 until find an unused one
 */
void initialSharedmemory();
uint32_t getNextId()
{
	initialSharedmemory();
	uint32_t initial = *last_id;
	do
	{
		*last_id = *last_id + 1L;
		if (!hasSharedMemory(*last_id))
			return *last_id;
	} while (*last_id != initial);
	Rf_error("Unable to find an available key for creating a shared memory, all keys are in used.");
}

int32_t getLastIndex()
{
	initialSharedmemory();
	return *last_id;
}
/*
===================================================================
Code for manage the reference count of the shared memory object
===================================================================
*/
static uint32_t getObjectCount(const string key)
{
	if (!keyInMap(segmentObjectCount, key))
	{
		return 0;
	}
	else
	{
		return segmentObjectCount[key];
	}
}

static void setObjectCount(string key, int offset)
{
	if (!keyInMap(segmentObjectCount, key))
	{
		if (offset < 0)
		{
			Rf_error("The object count is less than 0");
		}
		segmentObjectCount.insert(pair<string, uint32_t>(key, offset));
	}
	else
	{
		segmentObjectCount[key] = segmentObjectCount[key] + offset;
		if (segmentObjectCount[key] == 0)
		{
			segmentObjectCount.erase(key);
		}
	}
}
static void increaseObjectCount(const string key)
{
	setObjectCount(key, 1);
}
static void decreaseObjectCount(const string key)
{
	setObjectCount(key, -1);
}

static void zeroOutObjectCount(const string key)
{
	if (keyInMap(segmentObjectCount, key))
	{
		segmentObjectCount.erase(key);
	}
}


/*
===================================================================
Code for the internal functions
If a function is internal, it means the function will use the key as-is.
===================================================================
*/
/*
 Initialize the variable last_id which is located in the shared memory
 the variable serves as a hint for what the next id should be
 */
void allocateSharedMemoryInternal(const string key, size_t size_in_byte);
static void *mapSharedMemoryInternal(const string key);
static bool hasSharedMemoryInternal(const string key);

void initialSharedmemory()
{
	if (last_id == nullptr)
	{
		try
		{
			string name = SHARED_OBJECT_COUNTER;
			if (!hasSharedMemoryInternal(name))
			{
				allocateSharedMemoryInternal(name, sizeof(uint32_t));
				last_id = (uint32_t *)mapSharedMemoryInternal(name);
				*last_id = 0;
			}
			else
			{
				last_id = (uint32_t *)mapSharedMemoryInternal(name);
			}
		}
		catch (std::exception &ex)
		{
			Rf_error("An error has occured in initializing shared memory object: %s\n", ex.what());
		}
	}
}



static bool hasSharedMemoryInternal(const string key)
{
	try
	{
		OS_shared_memory_object sharedData(open_only, key.c_str(), read_write);
		return true;
	}
	catch (const std::exception &ex)
	{
		return false;
	}
}

#ifndef WINDOWS_OS
// This code is for checking the memory status on Linux
#include <signal.h>
#include <setjmp.h>
static jmp_buf reset;
static void (*old_handle)(int);
static void termination_handler(int signum)
{
	longjmp(reset, 1);
}
#endif

void allocateSharedMemoryInternal(const string key, size_t size_in_byte)
{
	DEBUG_SHARED_MEMORY(
		Rprintf("allocating shared memory, key:%s, size:%llu\n", key.c_str(), size_in_byte);
	)
	boost::interprocess::permissions perm;
	perm.set_unrestricted();

	bool allocSharedObject = false;
	OS_shared_memory_object *shm;
	try
	{
#ifdef WINDOWS_OS
		shm = new windows_shared_memory(create_only, key.c_str(), read_write, size_in_byte, perm);
		allocSharedObject = true;
#else
		shm = new shared_memory_object(create_only, key.c_str(), read_write, perm);
		allocSharedObject = true;
		shm->truncate(size_in_byte);
		/*Checking the memory status on Linux
		If the shared memory is not allocated successfully,
		boost will not throw an error, we need to manually check if
		the memory can be allocated or not.
		*/
		mapped_region region(*shm, read_write);
		void *ptr = region.get_address();
		if (setjmp(reset) != 0)
		{
			shared_memory_object::remove(key.c_str());
			delete shm;
			signal(SIGBUS, old_handle);
			Rf_error("Testing shared memory failed, the shared memory size is %llu bytes.", (unsigned long long int)size_in_byte);
		}
		else
		{
			old_handle = signal(SIGBUS, termination_handler);
			if (old_handle != SIG_ERR)
			{
				memset(ptr, UCHAR_MAX, size_in_byte);
				signal(SIGBUS, old_handle);
			}
		}
#endif
	}
	catch (const std::exception &ex)
	{
		if (allocSharedObject)
		{
#ifndef WINDOWS_OS
			shared_memory_object::remove(key.c_str());
#endif
			delete shm;
		}
		Rf_error("An error has occured in allocating shared memory: %s", ex.what());
	}
	sharedMemoryList.insert(pair<string, OS_shared_memory_object *>(key, shm));
}


// Open the shared memory and return the data pointer
// The memory must have been allocated!!
static void *mapSharedMemoryInternal(const string key)
{
	DEBUG_SHARED_MEMORY(
		Rprintf("mapping shared memory, key:%s\n", key.c_str());
	)
	/*
     the function initialSharedmemory will call mapSharedMemoryInternal
     so the argument isInitial is for preventing the infinite loop.
     */
	bool allocSharedObject = false;
	bool allocSegmentObject = false;
	bool ObjectCount = false;
	OS_shared_memory_object *shm = nullptr;
	mapped_region *region = nullptr;
	try
	{
		if (keyInMap(segmentList, key))
		{
			increaseObjectCount(key);
			return segmentList[key]->get_address();
		}

		// Otherwise, check if the shared memory object has been created
		if (keyInMap(sharedMemoryList, key))
		{
			shm = sharedMemoryList[key];
		}
		else
		{
			shm = new OS_shared_memory_object(open_only, key.c_str(), read_write);
			allocSharedObject = true;
			sharedMemoryList.insert(pair<string, OS_shared_memory_object *>(key, shm));
		}

		region = new mapped_region(*shm, read_write);
		allocSegmentObject = true;
		void *ptr = region->get_address();
		segmentList.insert(pair<string, mapped_region *>(key, region));
		increaseObjectCount(key);
		ObjectCount = true;
		return ptr;
	}
	catch (const std::exception &ex)
	{
		if (allocSharedObject)
		{
			sharedMemoryList.erase(key);
			delete shm;
		}
		if (allocSegmentObject)
		{
			segmentList.erase(key);
			delete region;
		}
		if (ObjectCount)
		{
			decreaseObjectCount(key);
		}
		Rf_error("An error has occured in mapping shared memory: %s", ex.what());
		return nullptr;
	}
}

//remove the shared memory object from the record
//but the data may be still in the shared memory
static bool unmapSharedMemoryInternal(const string key)
{
	DEBUG_SHARED_MEMORY(
		Rprintf("ummapping shared memory, key:%s, reference count:%d\n", key.c_str(), getObjectCount(key));
	)
	bool success = true;
	if (getObjectCount(key) > 1)
	{
		decreaseObjectCount(key);
	}
	else
	{
		zeroOutObjectCount(key);
		// Try to release the segment
		try
		{
			if (keyInMap(segmentList, key))
			{
				delete segmentList[key];
				segmentList.erase(key);
			}
		}
		catch (const std::exception &ex)
		{
			Rf_warning("An error has occured in closing the shared memory object: %s\n", ex.what());
			success = false;
		}
		// Try to release the shared memory object handle
		try
		{
			if (keyInMap(sharedMemoryList, key))
			{
				delete sharedMemoryList[key];
				sharedMemoryList.erase(key);
			}
		}
		catch (const std::exception &ex)
		{
			Rf_warning("An error has occured in closing the shared memory object: %s\n", ex.what());
			success = false;
		}
	}
	return success;
}

/*
Close and destroy the shared memory
It is user's responsibility to make sure the memory will not be used
After the free
*/
bool freeSharedMemoryInternal(const string key)
{
	DEBUG_SHARED_MEMORY(
		Rprintf("freeing shared memory, key:%s, reference count:%d\n", key.c_str(), getObjectCount(key));
	)
	unmapSharedMemoryInternal(key);
	zeroOutObjectCount(key);
#ifdef WINDOWS_OS
	return true;
#else
	try
	{
		return OS_shared_memory_object::remove(key.c_str());
	}
	catch (const std::exception &ex)
	{
		Rf_warning("Fail to remove the shared memory: %s\n", ex.what());
		return false;
	}
#endif
}

double getSharedMemorySizeInternal(const string key)
{
	DEBUG_SHARED_MEMORY(Rprintf("get shared memory size, Key:%s, exist: %d", key.c_str(),hasSharedMemoryInternal(key)));
	if (hasSharedMemoryInternal(key))
	{
		try
		{
			offset_t size = 0;
			if (keyInMap(sharedMemoryList, key))
			{
#ifdef WINDOWS_OS
				size = sharedMemoryList[key]->get_size();
#else
				sharedMemoryList[key]->get_size(size);
#endif
			}
			else
			{
				OS_shared_memory_object sharedMemory(open_only, key.c_str(), read_write);
#ifdef WINDOWS_OS
				size = sharedMemory.get_size();
#else
				sharedMemory.get_size(size);
#endif
			}
			return size;
		}
		catch (const std::exception &ex)
		{
			Rf_warning("Fail to get the  size of the shared memory: %s\n", ex.what());
			return 0;
		}
	}
	return 0;
}



/*
===================================================================
Code for the public functions
===================================================================
*/

bool hasNamedSharedMemory(const char *name)
{
	return hasSharedMemoryInternal(getNamedKey(name));
}
/*  Check whether the shared memory exist*/
bool hasSharedMemory(uint32_t id)
{
	return hasSharedMemoryInternal(getIdKey(id));
}


uint32_t allocateSharedMemory(size_t size_in_byte)
{
	//initialSharedmemory();
	uint32_t id = getNextId();
	string key = getIdKey(id);
	allocateSharedMemoryInternal(key, size_in_byte);
	return id;
}
void allocateNamedSharedMemory(const char *name, size_t size_in_byte)
{
	//initialSharedmemory();
	string key = getNamedKey(name);
	allocateSharedMemoryInternal(key, size_in_byte);
}


void *mapSharedMemory(uint32_t id)
{
	string key = getIdKey(id);
	return mapSharedMemoryInternal(key);
}
void *mapNamedSharedMemory(const char *name)
{
	string key = getNamedKey(name);
	return mapSharedMemoryInternal(key);
}


bool unmapSharedMemory(uint32_t id)
{
	string key = getIdKey(id);
	return unmapSharedMemoryInternal(key);
}

bool unmapNamedSharedMemory(const char *name)
{
	string key = getNamedKey(name);
	return unmapSharedMemoryInternal(key);
}


bool freeSharedMemory(uint32_t id)
{
	string key = getIdKey(id);
	return freeSharedMemoryInternal(key);
}
bool freeNamedSharedMemory(const char *name)
{
	string key = getNamedKey(name);
	return freeSharedMemoryInternal(key);
}


double getSharedMemorySize(uint32_t id)
{
	string key = getIdKey(id);
	return getSharedMemorySizeInternal(key);
}

double getNamedSharedMemorySize(const char *name)
{
	string key = getNamedKey(name);
	return getSharedMemorySizeInternal(key);
}