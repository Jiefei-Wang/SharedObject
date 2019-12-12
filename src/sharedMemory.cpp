#ifndef _WIN32
#define WINDOWS_OS
#endif

#ifdef  WINDOWS_OS
#define OS_shared_memory_object windows_shared_memory
#define OS_SHARED_OBJECT_PKG_SPACE ("Local\\shared_object_package_space"+OS_ADDRESS_SIZE).c_str()
#else
#define OS_shared_memory_object shared_memory_object
#define OS_SHARED_OBJECT_PKG_SPACE ("shared_object_package_space"+OS_ADDRESS_SIZE).c_str()
#endif //  WINDOWS_OS

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <map>
#include "sharedMemory.h"
#include "tools.h"

using std::string;
using std::to_string;
using std::pair;
using namespace boost::interprocess;

static std::map<uint32_t, OS_shared_memory_object*> sharedMemoryList;
static std::map<uint32_t, mapped_region*> segmentList;
static uint32_t* last_id = nullptr;


/*
Initialize the variable last_id which is located in the shared memory
the variable serves as a hint for what the next id should be
*/
void initialSharedmemory() {
	if (last_id == nullptr) {
		allocateSharedMemoryInternal(0, sizeof(uint32_t));
		last_id = (uint32_t*)mapSharedMemory(0);
	}
}


template<class T1, class T2>
bool keyInMap(T1 map, T2 key) {
	return map.find(key) != map.end();
}

//Get the key to access a shared data that is used in R
string getDataMemoryKey(uint32_t id) {
	//common name + DID
	return(string(OS_SHARED_OBJECT_PKG_SPACE).append("_id") + to_string(id));
}

/*
Get the next id
The id will be searched starting from *last_id + 1
until find an unused one
*/
uint32_t getNextId() {
	uint32_t initial = *last_id;
	do {
		*last_id = *last_id + 1L;
		if (*last_id == 0L) * last_id = 1L;

		string key = getDataMemoryKey(*last_id);
		if (!hasSharedMemory(key.c_str()))
			return *last_id;
	} while (*last_id != initial);
	Rf_error("Unable to find an available key for creating a shared memory, all keys are in used.");

}

/*  Check whether the shared memory exist*/
bool hasSharedMemory(uint32_t id)
{
	return hasSharedMemory(getDataMemoryKey(id).c_str());
}
bool hasSharedMemory(const char* name)
{
	try
	{
		OS_shared_memory_object sharedData(open_only, name, read_write);
		return true;
	}
	catch (const std::exception & ex) {
		return false;
	}
}



//allocate shared memory without doing any memory check
void allocateSharedMemoryInternal(uint32_t id, size_t size_in_byte) {
	boost::interprocess::permissions perm;
	perm.set_unrestricted();
	string key = getDataMemoryKey(id);
	try {
#ifdef WINDOWS_OS
		windows_shared_memory* shm= new windows_shared_memory(create_only, key.c_str(), read_write, size_in_byte, perm);
#else
		shared_memory_object* shm = new shared_memory_object(create_only, key.c_str(), read_write, perm);
		shm->truncate(size_in_byte);
#endif
		sharedMemoryList.insert(pair<uint32_t, OS_shared_memory_object*>(id, shm));
	}
	catch (const std::exception& ex) {
		Rf_error("An error has occured in allocating shared memory: %s", ex.what());
	}
}



uint32_t allocateSharedMemory(size_t size_in_byte) {
	initialSharedmemory();
	uint32_t id = getNextId();
	allocateSharedMemoryInternal(id, size_in_byte);
	return id;
};
void* mapSharedMemory(uint32_t id) {
	initialSharedmemory();
	try {
		if (keyInMap(segmentList, id)) {
			return segmentList[id]->get_address();
		}

		mapped_region* region;
		if (keyInMap(sharedMemoryList, id)) {
			OS_shared_memory_object* shm = sharedMemoryList[id];
			region = new mapped_region(*shm, read_write);
			segmentList.insert(pair<uint32_t, mapped_region*>(id, region));
		}
		else {
			string key = getDataMemoryKey(id);
			OS_shared_memory_object* shm = new OS_shared_memory_object(open_only, key.c_str(), read_write);
			region = new mapped_region(*shm, read_write);
			sharedMemoryList.insert(pair<uint32_t, OS_shared_memory_object*>(id, shm));
			segmentList.insert(pair<uint32_t, mapped_region*>(id, region));
		}
		return region->get_address();
	}
	catch (const std::exception & ex) {
		Rf_error("An error has occured in mapping shared memory: %s", ex.what());
		return nullptr;
	}
	
}

void unmapSharedMemory(uint32_t id) {
	initialSharedmemory();
	if (keyInMap(segmentList, id)) {
		delete segmentList[id];
		segmentList.erase(id);
	}
};
bool freeSharedMemory(uint32_t id) {
	initialSharedmemory();
#ifdef WINDOWS_OS
	return true;
#else
	try {
	string key = getDataMemoryKey(id);
	unmapSharedMemory(id);
	if (keyInMap(sharedMemoryList, id)) {
		delete sharedMemoryList[id];
		sharedMemoryList.erase(id);
	}
	return OS_shared_memory_object::remove(key.c_str());
	}
	catch (const std::exception& ex) {
		Rf_error("An error has occured in deallocating shared memory: %s", ex.what());
		return false;
	}
#endif
};