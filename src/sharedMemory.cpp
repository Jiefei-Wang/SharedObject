#ifdef _WIN32
#define WINDOWS_OS
#endif

#ifdef WINDOWS_OS
#include <boost/interprocess/windows_shared_memory.hpp>
#define OS_shared_memory_object windows_shared_memory
#define OS_SHARED_OBJECT_PKG_SPACE ("Local\\shared_object_package_space" + OS_ADDRESS_SIZE).c_str()
#else
#include <boost/interprocess/shared_memory_object.hpp>
#define OS_shared_memory_object shared_memory_object
#define OS_SHARED_OBJECT_PKG_SPACE ("shared_object_package_space" + OS_ADDRESS_SIZE).c_str()
#endif //  WINDOWS_OS

#include <Rcpp.h>
#include <boost/interprocess/mapped_region.hpp>
#include <map>
#include "sharedMemory.h"
#include "tools.h"

#define SHARED_OBJECT_COUNTER "sharedObjectCounter"
using std::pair;
using std::string;
using std::to_string;
using namespace boost::interprocess;

static std::map<uint32_t, OS_shared_memory_object *> sharedMemoryList;
static std::map<string, OS_shared_memory_object *> sharedNamedMemoryList;
static std::map<uint32_t, mapped_region *> segmentList;
static std::map<string, mapped_region *> namedSegmentList;
static std::map<uint32_t, uint32_t> segmentObjectCount;
static std::map<string, uint32_t> namedSegmentObjectCount;

static uint32_t *last_id = nullptr;

template <class T1, class T2>
void allocateSharedMemoryInternal(const T1 &id, size_t size_in_byte, T2 &sharedMemoryList);
template <class T1, class T2, class T3>
static void *mapSharedMemoryInternal(const T1 &id, T2 &sharedMemoryList, T3 &segmentList, bool isInitial = false);
/*
 Initialize the variable last_id which is located in the shared memory
 the variable serves as a hint for what the next id should be
 */
void initialSharedmemory()
{
	if (last_id == nullptr)
	{
		string name = SHARED_OBJECT_COUNTER;
		if (!hasSharedMemory(name.c_str()))
		{
			allocateSharedMemoryInternal(name, sizeof(uint32_t), sharedNamedMemoryList);
			last_id = (uint32_t *)mapSharedMemoryInternal(name, sharedNamedMemoryList, namedSegmentList, true);
			*last_id = 0;
		}
		else
		{
			last_id = (uint32_t *)mapSharedMemoryInternal(name, sharedNamedMemoryList, namedSegmentList, true);
		}
	}
}

template <class T1, class T2>
bool keyInMap(T1 map, T2 key)
{
	return map.find(key) != map.end();
}

//Get the key to access a shared data that is used in R
string getDataMemoryKey(uint32_t id)
{
	//common name + id
	return string(OS_SHARED_OBJECT_PKG_SPACE).append("_id_") + to_string(id);
}

string getDataMemoryKey(const string &name)
{
	//common name + name
	return string(OS_SHARED_OBJECT_PKG_SPACE).append("_id_") + name;
}

/*
 Get the next id
 The id will be searched starting from *last_id + 1
 until find an unused one
 */
uint32_t getNextId()
{
	uint32_t initial = *last_id;
	do
	{
		*last_id = *last_id + 1L;
		if (!hasSharedMemory(*last_id))
			return *last_id;
	} while (*last_id != initial);
	Rf_error("Unable to find an available key for creating a shared memory, all keys are in used.");
}

static bool hasSharedMemoryInternal(const char *name)
{
	try
	{
		OS_shared_memory_object sharedData(open_only, name, read_write);
		return true;
	}
	catch (const std::exception &ex)
	{
		return false;
	}
}
static bool hasSharedMemory(const string &name)
{
	return hasSharedMemoryInternal(getDataMemoryKey(name).c_str());
}
bool hasSharedMemory(const char *name)
{
	return hasSharedMemoryInternal(getDataMemoryKey(name).c_str());
}
/*  Check whether the shared memory exist*/
bool hasSharedMemory(uint32_t id)
{
	return hasSharedMemoryInternal(getDataMemoryKey(id).c_str());
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

template <class T1, class T2>
void allocateSharedMemoryInternal(const T1 &id, size_t size_in_byte, T2 &sharedMemoryList)
{
	boost::interprocess::permissions perm;
	perm.set_unrestricted();
	string key = getDataMemoryKey(id);

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
		boost will not throw an error, I need to manually check if
		the memory exist or not. Sad
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
				memset(ptr, 0, size_in_byte);
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
	sharedMemoryList.insert(pair<T1, OS_shared_memory_object *>(id, shm));
}

uint32_t allocateSharedMemory(size_t size_in_byte)
{
	initialSharedmemory();
	uint32_t id = getNextId();
	allocateSharedMemoryInternal(id, size_in_byte, sharedMemoryList);
	return id;
}
void allocateSharedMemory(const char *name, size_t size_in_byte)
{
	initialSharedmemory();
	allocateSharedMemoryInternal(string(name), size_in_byte, sharedNamedMemoryList);
}

template <class T1, class T2>
static uint32_t getObjectCountInternal(const T1 &id, T2 &objectCountList)
{
	if (!keyInMap(objectCountList, id))
	{
		return 0;
	}
	else
	{
		return objectCountList[id];
	}
}
static uint32_t getObjectCount(const string &id)
{
	return getObjectCountInternal(id, namedSegmentObjectCount);
}
static uint32_t getObjectCount(const uint32_t &id)
{
	return getObjectCountInternal(id, segmentObjectCount);
}
template <class T1, class T2>
static void changeObjectCount(const T1 &id, T2 &objectCountList, int offset)
{
	if (!keyInMap(objectCountList, id))
	{
		if (offset < 0)
		{
			Rf_error("The object count is less than 0");
		}
		objectCountList.insert(pair<T1, uint32_t>(id, offset));
	}
	else
	{
		objectCountList[id] = objectCountList[id] + offset;
		if (objectCountList[id] == 0)
		{
			objectCountList.erase(id);
		}
	}
}
static void increaseObjectCount(const string &id)
{
	changeObjectCount(id, namedSegmentObjectCount, 1);
}
static void increaseObjectCount(const uint32_t &id)
{
	changeObjectCount(id, segmentObjectCount, 1);
}
static void decreaseObjectCount(const string &id)
{
	changeObjectCount(id, namedSegmentObjectCount, -1);
}
static void decreaseObjectCount(const uint32_t &id)
{
	changeObjectCount(id, segmentObjectCount, -1);
}
template <class T1, class T2>
static void zeroOutObjectCountInternal(const T1 &id, T2 &objectCountList)
{
	if (keyInMap(objectCountList, id))
	{
		objectCountList.erase(id);
	}
}
static void zeroOutObjectCount(const string &id)
{
	zeroOutObjectCountInternal(id, namedSegmentObjectCount);
}
static void zeroOutObjectCount(const uint32_t &id)
{
	zeroOutObjectCountInternal(id, segmentObjectCount);
}
// Open the shared memory and return the data pointer
// The memory must have been allocated!!
template <class T1, class T2, class T3>
static void *mapSharedMemoryInternal(const T1 &id, T2 &sharedMemoryList, T3 &segmentList, bool isInitial)
{
	/*
     the function initialSharedmemory will call mapSharedMemoryInternal
     so the argument isInitial is for preventing the infinite loop.
     */
	if (!isInitial)
		initialSharedmemory();
	bool allocSharedObject = false;
	bool allocSegmentObject = false;
	bool ObjectCount = false;
	OS_shared_memory_object *shm = nullptr;
	mapped_region *region = nullptr;
	try
	{
		if (keyInMap(segmentList, id))
		{
			increaseObjectCount(id);
			return segmentList[id]->get_address();
		}

		// Check if the segment has been opened
		if (keyInMap(sharedMemoryList, id))
		{
			shm = sharedMemoryList[id];
		}
		else
		{
			string key = getDataMemoryKey(id);
			shm = new OS_shared_memory_object(open_only, key.c_str(), read_write);
			allocSharedObject = true;
			sharedMemoryList.insert(pair<T1, OS_shared_memory_object *>(id, shm));
		}

		region = new mapped_region(*shm, read_write);
		allocSegmentObject = true;
		void *ptr = region->get_address();
		segmentList.insert(pair<T1, mapped_region *>(id, region));
		increaseObjectCount(id);
		ObjectCount = true;
		return ptr;
	}
	catch (const std::exception &ex)
	{
		if (allocSharedObject)
		{
			sharedMemoryList.erase(id);
			delete shm;
		}
		if (allocSegmentObject)
		{
			segmentList.erase(id);
			delete region;
		}
		if (ObjectCount)
		{
			decreaseObjectCount(id);
		}
		Rf_warning("An error has occured in mapping shared memory: %s", ex.what());
		return nullptr;
	}
}

void *mapSharedMemory(uint32_t id)
{
	return mapSharedMemoryInternal(id, sharedMemoryList, segmentList);
}
void *mapSharedMemory(const char *name)
{
	return mapSharedMemoryInternal(string(name), sharedNamedMemoryList, namedSegmentList);
}

//Unmap the region, but not releasing the memory
template <class T1, class T2>
static bool removeSegmentInternal(const T1 &id, T2 &segmentList)
{
	initialSharedmemory();
	try
	{
		if (keyInMap(segmentList, id))
		{
			delete segmentList[id];
			segmentList.erase(id);
		}
	}
	catch (const std::exception &ex)
	{
		Rf_warning("An error has occured in closing the shared memory object: %s", ex.what());
		return false;
	}
	return true;
}

//remove the shared memory object from the record
//but the data may be still in the shared memory
template <class T1, class T2>
bool removeSharedMemoryInternal(const T1 &id, T2 &sharedMemoryList)
{
	initialSharedmemory();
	try
	{
		if (keyInMap(sharedMemoryList, id))
		{
			delete sharedMemoryList[id];
			sharedMemoryList.erase(id);
		}
	}
	catch (const std::exception &ex)
	{
		Rf_warning("An error has occured in closing the shared memory object: %s", ex.what());
		return false;
	}
	return true;
}

bool unmapSharedMemory(uint32_t id)
{
	DEBUG(Rprintf("Unmap shared memory, id:%d, count:%d\n", id, getObjectCount(id)));
	if (getObjectCount(id) >= 1)
	{
		decreaseObjectCount(id);
		return (true);
	}
	bool result1 = removeSegmentInternal(id, segmentList);
	bool result2 = removeSharedMemoryInternal(id, sharedMemoryList);
	return result1 && result2;
}

bool unmapSharedMemory(const string &name)
{
	DEBUG(Rprintf("Unmap shared memory, id:%s, count:%d\n", name.c_str(), getObjectCount(name)));
	if (getObjectCount(name) > 1)
	{
		decreaseObjectCount(name);
		return (true);
	}
	bool result1 = removeSegmentInternal(name, namedSegmentList);
	bool result2 = removeSharedMemoryInternal(name, sharedNamedMemoryList);
	return result1 && result2;
}
bool unmapSharedMemory(const char *name)
{
	return unmapSharedMemory(string(name));
}

// Close and destroy the shared memory
// It is user's responsibility to make sure the memory will not be used
// After the free
template <class T1, class T2>
bool freeSharedMemoryInternal(const T1 &id, T2 &sharedMemoryList)
{
	DEBUG(Rprintf("free shared memory, reference count:%d\n", getObjectCount(id)));
	initialSharedmemory();
	bool success = unmapSharedMemory(id);
	if (!success)
	{
		return false;
	}
	zeroOutObjectCount(id);
#ifdef WINDOWS_OS
	return true;
#else
	try
	{
		string key = getDataMemoryKey(id);
		return OS_shared_memory_object::remove(key.c_str());
	}
	catch (const std::exception &ex)
	{
		Rf_warning("An error has occured in removing the shared memory: %s", ex.what());
		return false;
	}
#endif
}

bool freeSharedMemory(uint32_t id)
{
	return freeSharedMemoryInternal(id, sharedMemoryList);
}
bool freeSharedMemory(const char *name)
{
	return freeSharedMemoryInternal(string(name), sharedNamedMemoryList);
}

int32_t getLastIndex()
{
	initialSharedmemory();
	return *last_id;
}

template <class T1, class T2>
double getSharedMemorySizeInternal(const T1 &id, T2 &sharedMemoryList)
{
	initialSharedmemory();
	if (hasSharedMemory(id))
	{
		offset_t size;
		if (keyInMap(sharedMemoryList, id))
		{
#ifdef WINDOWS_OS
			size = sharedMemoryList[id]->get_size();
#else
			sharedMemoryList[id]->get_size(size);
#endif
		}
		else
		{
			OS_shared_memory_object sharedMemory(open_only, getDataMemoryKey(id).c_str(), read_write);
#ifdef WINDOWS_OS
			size = sharedMemory.get_size();
#else
			sharedMemory.get_size(size);
#endif
		}
		return size;
	}
	return 0;
}

double getSharedMemorySize(uint32_t id)
{
	return getSharedMemorySizeInternal(id, sharedMemoryList);
}

double getSharedMemorySize(const char *name)
{
	return getSharedMemorySizeInternal(string(name), sharedNamedMemoryList);
}
