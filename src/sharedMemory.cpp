#include <map>
#include <string>
#include <memory>
#include <atomic>
#include <Rcpp.h>
#define BOOST_NO_AUTO_PTR
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/thread/thread_time.hpp>
#include "sharedMemory.h"
#include "sharedObjectClass.h"
#include "utils.h"

#define atomic_uint64_type atomic<uint64_t>

#ifdef _WIN32
#define OS_SHARED_OBJECT_PKG_SPACE ("Local\\SO_" + OS_ADDRESS_SIZE).c_str()
#else
#define OS_SHARED_OBJECT_PKG_SPACE ("SO_" + OS_ADDRESS_SIZE).c_str()
#endif

#define keyInMap(map, x) (map.find(x) != map.end())

#define SHARED_OBJECT_COUNTER "sharedObjectCounter"
#define SEMAPHORE_NAME "sharedObjectSemaphore"
using std::pair;
using std::string;
using namespace boost::interprocess;

static std::map<string, int> mapCounter;
static std::map<string, std::unique_ptr<SharedObjectClass>> sharedObjectList;

// This lastId attributes is shared
static std::atomic_uint64_type *lastId;

/*
===================================================================
Process lock guard
===================================================================
*/
class auto_semophore
{
	named_semaphore *semaphore = nullptr;
	bool locked = false;

public:
	auto_semophore()
	{
		boost::interprocess::permissions perm;
		perm.set_unrestricted();
		semaphore = new named_semaphore(open_or_create_t(), SEMAPHORE_NAME, 1, perm);
	}
	void lock()
	{
		//Rprintf("internal lock\n");
		boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(3000);
		bool lock_status = semaphore->timed_wait(timeout);
		if (!lock_status)
		{
			Rf_warning("Something is wrong with the process lock, the package will proceed without lock\n");
		}
	}
	~auto_semophore()
	{
		if (locked)
			semaphore->post();
		delete semaphore;
		named_semaphore::remove(SEMAPHORE_NAME);
	}
};

/*
===================================================================
Utilities
===================================================================
*/

//Get the key that is used as the name of the shared memory object
string getKey(string id)
{
	return string(OS_SHARED_OBJECT_PKG_SPACE).append("_") + id;
}
/*
 Get the next id
 The id will be searched starting from *lastId + 1
 until find an unused one
 */
std::string getNextId()
{
	uint64_t initialId = *lastId;
	uint64_t id;
	//TODO: make this process-safe
	do
	{
		id = (++(*lastId));
		if (!hasSharedMemory(std::to_string(id)))
		{
			return std::to_string(id);
		}
	} while (id != initialId);
	throwError("Unable to find an available key for creating a shared memory, all keys are in used.\n");
	return "";
}

/*
===================================================================
Code for the internal functions
If a function is internal, it means the function will use the key as-is.
===================================================================
*/

void allocateSharedMemoryInternal(string key, uint64_t size_in_byte)
{
	if (!keyInMap(sharedObjectList, key))
	{
		SharedObjectClass *so = new SharedObjectClass(key, size_in_byte);
		sharedObjectList.emplace(key, so);
	}
	else
	{
		if (sharedObjectList.at(key)->getSize() < size_in_byte)
		{
			throwError("The shared memory size is smaller than you request(reqeust: %llu, actual:%llu)\n",
					 size_in_byte, (uint64_t)sharedObjectList.at(key)->getSize());
		}
	}
	sharedObjectList.at(key)->allocateSharedMemory();
}

// Open the shared memory and return the data pointer
// The memory must have been allocated!!
static void *mapSharedMemoryInternal(const string key)
{
	if (!keyInMap(sharedObjectList, key))
	{
		SharedObjectClass *so = new SharedObjectClass(key, 0);
		sharedObjectList.emplace(key, so);
		mapCounter[key] = 0;
	}
	void *ptr = sharedObjectList.at(key)->mapSharedMemory();
	mapCounter[key]++;
	return ptr;
}

//remove the mapping from the program virtual memory
//but the data may be still in the shared memory
static void unmapSharedMemoryInternal(const string key)
{
	if (keyInMap(sharedObjectList, key))
	{
		mapCounter[key]--;
		if (mapCounter[key] <= 0)
		{
			sharedObjectList.at(key)->unmapSharedMemory();
			sharedObjectList.erase(key);
		}
	}
}

/*
Close and destroy the shared memory
It is user's responsibility to make sure the memory will not be used
After the free
*/
void freeSharedMemoryInternal(const string key)
{
	if (keyInMap(sharedObjectList, key))
	{
		sharedObjectList.erase(key);
		mapCounter.erase(key);
	}
	SharedObjectClass::freeSharedMemory(key);
}

double getSharedMemorySizeInternal(const string key)
{
	if (keyInMap(sharedObjectList, key))
	{
		return sharedObjectList.at(key)->getSize();
	}
	else
	{
		SharedObjectClass so(key);
		so.mapSharedMemory();
		return so.getSize();
	}
}

/*
===================================================================
Code for the public functions
===================================================================
*/

/*  Check whether the shared memory exist*/
bool hasSharedMemory(std::string id)
{
	string key = getKey(id);
	return SharedObjectClass::hasSharedMemory(key);
}
bool isSharedMemoryMapped(std::string id)
{
	string key = getKey(id);
	if (!keyInMap(sharedObjectList, key))
	{
		return false;
	}
	return sharedObjectList.at(key)->hasMappedRegionHandle();
}

std::string allocateSharedMemory(unsigned long long int size_in_byte, std::string id)
{
	if (id == "")
	{
		id = getNextId();
	}
	string key = getKey(id);
	allocateSharedMemoryInternal(key, size_in_byte);
	return id;
}

void *mapSharedMemory(std::string id)
{
	string key = getKey(id);
	return mapSharedMemoryInternal(key);
}

void unmapSharedMemory(std::string id)
{
	string key = getKey(id);
	return unmapSharedMemoryInternal(key);
}

void freeSharedMemory(std::string id)
{
	string key = getKey(id);
	return freeSharedMemoryInternal(key);
}

double getSharedMemorySize(std::string id)
{
	string key = getKey(id);
	return getSharedMemorySizeInternal(key);
}

// [[Rcpp::export]]
Rcpp::DataFrame getSharedObjectList()
{
	using namespace Rcpp;
	int n = sharedObjectList.size();
	CharacterVector name(n);
	LogicalVector sharedMemoryHandle(n);
	LogicalVector mappedRegionHandle(n);
	NumericVector sharedMemoryCounter(n);
	int j = 0;
	for (auto &i : sharedObjectList)
	{
		name[j] = i.first;
		sharedMemoryHandle[j] = i.second->hasSharedMemoryHandle();
		mappedRegionHandle[j] = i.second->hasMappedRegionHandle();
		sharedMemoryCounter[j] = mapCounter[i.first];
		j++;
	}
	DataFrame df = DataFrame::create(Named("name") = name,
									 Named("sharedMemoryHandle") = sharedMemoryHandle,
									 Named("mappedRegionHandle") = mappedRegionHandle,
									 Named("sharedMemoryCounter") = sharedMemoryCounter);
	return df;
}

bool autoReleaseAfterUse(std::string id)
{
	string key = getKey(id);
	if (!keyInMap(sharedObjectList, key))
	{
		throwError("Error in <autoReleaseAfterUse>: The shared object have not been mapped to the current process(key: %s)\n",
				 key.c_str());
	}
	return sharedObjectList.at(key)->getOwnership();
}
void autoReleaseAfterUse(std::string id, bool releaseAfterUse)
{
	string key = getKey(id);
	if (!keyInMap(sharedObjectList, key))
	{
		throwError("Error in <autoReleaseAfterUse>: The shared object have not been mapped to the current process(key: %s)\n",
				 key.c_str());
	}
	sharedObjectList.at(key)->setOwnership(releaseAfterUse);
}

uint64_t getLastIndex()
{
	return *lastId;
}

/*
 Initialize the variable lastId which is located in the shared memory
 the variable serves as a hint for what the next id should be
 */

void initialPkgData()
{
	if (lastId == nullptr)
	{
		try
		{
			auto_semophore semophore;
			semophore.lock();
			ERROR_CATCHER catcher;
			bool hasSharedMemory = SharedObjectClass::hasSharedMemory(SHARED_OBJECT_COUNTER);
			void *ptr;
			if (hasSharedMemory)
			{
				ptr = mapSharedMemoryInternal(SHARED_OBJECT_COUNTER);
			}
			else
			{
				allocateSharedMemoryInternal(SHARED_OBJECT_COUNTER, sizeof(std::atomic_uint64_type));
				ptr = mapSharedMemoryInternal(SHARED_OBJECT_COUNTER);
				new (ptr) std::atomic_uint64_type(0);
			}
			lastId = (std::atomic_uint64_type *)ptr;
		}
		catch (std::exception &ex)
		{
			lastId = nullptr;
			throwError("An error has occured in initializing shared memory object: %s\n%s",
					 ex.what(),
					 "You must manually initial the package via <initialSharedObjectPackageData()>\n");
		}
	}
}

void releasePkgData()
{
	freeSharedMemoryInternal(SHARED_OBJECT_COUNTER);
	lastId = nullptr;
}