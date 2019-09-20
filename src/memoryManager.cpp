#ifdef _WIN32
#define WINDOWS_OS
#endif

/*
We will call:
managed shared object-> segment
shared object -> shared object
mapped_region -> region

The usage of each macro:
OS_SHARED_OBJECT_PKG_SPACE: The name of the segment that manages process data (managed_shared_memory object)
OS_DATA_INFO_MAP_NAME: The name of the shared memory that stores a map that maps from data id to data info
					    It is a member of the segment in OS_SHARED_OBJECT_PKG_SPACE


first initialize process:
R process -> 
initial OS_SHARED_OBJECT_PKG_SPACE segment -> 
register the data info map under the OS_SHARED_OBJECT_PKG_SPACE segment->
allocate process sharedMemoryList, segmentList to store opened object.


When crash, the OS_SHARED_OBJECT_PKG_SPACE segment is still in the memory and can be retrived

*/


#ifdef  WINDOWS_OS 
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#define OS_managed_shared_memory managed_windows_shared_memory
#define OS_shared_memory_object windows_shared_memory 
#define OS_SHARED_OBJECT_PKG_SPACE ("Local\\shared_object_package_space"+OS_ADDRESS_SIZE).c_str()
#define OS_DATA_INFO_MAP_NAME ("Local\\data_info_map"+OS_ADDRESS_SIZE).c_str()
#else
#include <boost/interprocess/managed_shared_memory.hpp>
#define OS_managed_shared_memory managed_shared_memory
#define OS_shared_memory_object shared_memory_object 
#define OS_SHARED_OBJECT_PKG_SPACE ("shared_object_package_space"+OS_ADDRESS_SIZE).c_str()
#define OS_DATA_INFO_MAP_NAME ("data_info_map"+OS_ADDRESS_SIZE).c_str()
#endif //  WINDOWS_OS 

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/foreach.hpp>
#include "tools.h"
#include "memoryManager.h"
#include <map>
#include <vector>
using namespace boost::interprocess;
using std::string;
using std::pair;
using std::to_string;


#define DATA_LIST_SIZE 1024*1024*8

// managed_shared_memory or shared_memory_object
// Two classes need different method to access them.
enum sharedSystem { MSM, SMO };

//Define dataID to dataInfo map
typedef std::pair<const DID, dataInfo> dataInfoPair;
typedef allocator<dataInfoPair, OS_managed_shared_memory::segment_manager> dataInfoAllocator;
typedef map<DID, dataInfo, std::less<DID>, dataInfoAllocator> sharedDataInfoMap;


//local variables to keep track of the opened 
//shared memory object and mapped region object 
std::map<DID, OS_shared_memory_object*> sharedMemoryList;
std::map<DID, mapped_region*> segmentList;

//A map from data id to data info
//Shared by all processes
OS_managed_shared_memory* processInfoSegment;
sharedDataInfoMap* dataInfoMap = nullptr;

//Delete the data pointer in vec[key] if it exist.
#define removeVectorKeyAndValueIfExist(vec,key)\
if (vec.find(key) != vec.end()) {\
delete(vec.at(key));\
vec.erase(key);\
}\

/*  Check whether the shared memory is readable
	There is no API to test if a shared memory exist,
	this function is an alternative way to test the existance*/
bool hasSharedMemory(const char* name, sharedSystem ss)
{
	try
	{
		if (ss == sharedSystem::MSM) {
			OS_managed_shared_memory sharedData(open_only, name);
			return true;
		}
		if (ss == sharedSystem::SMO) {
			OS_shared_memory_object sharedData(open_only, name, read_write);
			return true;
		}
		errorHandle("incorrect shared system");
	}
	catch (const std::exception & ex) {
		//errorHandle(std::string("managed_shared_memory ex: ")+boost::diagnostic_information(ex));
	}
	return false;
}

/* Remove the shared memory
   For windows there is no API to remove the memory, alway return True.*/
bool removeSharedMemory(const char* name) {
#ifdef WINDOWS_OS 
	return true;
#else
	return OS_shared_memory_object::remove(name);
#endif
}


bool removeDataID(DID did) {
	dataInfoMap->erase(did);
	return true;
}

//Get the key to access a shared data that is used in R
string getDataMemoryKey(DID did) {
	//common name + DID
	return(string(OS_SHARED_OBJECT_PKG_SPACE).append("DID") + to_string(did));
}


OS_managed_shared_memory* openOrCreateSharedSegment(const char* name, size_t size) {
	OS_managed_shared_memory* segment;

	boost::interprocess::permissions perm;
	perm.set_unrestricted();
	if (!hasSharedMemory(name, sharedSystem::MSM)) {
		removeSharedMemory(name);
		segment = new OS_managed_shared_memory(create_only, name, size, 0, perm);
	}
	else {
		segment = new OS_managed_shared_memory(open_only, name);
	}
	return(segment);
}

template<typename alloc, typename mapType, typename mapKey>
mapType* openOrCreateSharedMap(OS_managed_shared_memory * segment, const char* name) {
	//segment->destroy<mapType>(name);
	mapType* infoMap = segment->find<mapType>(name).first;
	//Create named shared memory object if not exist
	if (infoMap == 0) {
		alloc alloc_inst(segment->get_segment_manager());
		infoMap = segment->construct<mapType>(name)
			(std::less<mapKey>(), alloc_inst);
	}
	return(infoMap);
}


//This function will be called when the package is loaded
void initialSharedMemory() {
	try
	{
		//If it is the first time to run the program, check if the shared memory exist
		//If it does not exist, create it
		if (processInfoSegment == nullptr) {
			//printf("not initialized\n");
			//Create shared memory space if not exist
			processInfoSegment = openOrCreateSharedSegment(OS_SHARED_OBJECT_PKG_SPACE, DATA_LIST_SIZE);
			dataInfoMap = openOrCreateSharedMap< dataInfoAllocator, sharedDataInfoMap, DID>(processInfoSegment, OS_DATA_INFO_MAP_NAME);
		}
	}
	catch (const std::exception & ex) {
		errorHandle("error in initialize the shared memory\n%s", ex.what());
	}
}


/*
#####################above is internal API###########################
*/

/* Check if the data ID exist in the data info map*/
bool hasDataID(DID did) {
	//dataInfoMap->find(did) != dataInfoMap->end()
	if (dataInfoMap->contains(did)) {
		return true;
	}
	else {
		return false;
	}
}
// Check the availablility of the key
// If the key is in used, return a new key, otherwise return the same key
DID findAvailableDataId(DID dataID) {
	initialSharedMemory();
	while (hasDataID(dataID)) {
		dataID += 1;
	}
	return dataID;
}

/*
  Nasty code to create a shared memory object that works for both Windows and Linux
*/
#ifdef WINDOWS_OS
#define CREATE_SHARED_MEM(obj,openType,name,mode,permission,size) \
OS_shared_memory_object* obj=new OS_shared_memory_object(openType, name, mode,size, permission);
#else
#define CREATE_SHARED_MEM(obj,openType,name,mode,permission,size) \
OS_shared_memory_object* obj=new OS_shared_memory_object(openType, name, mode, permission); \
obj->truncate(size);
#endif


void* createSharedObject(const void* data, dataInfo DI) {
	initialSharedMemory();
	//Allocate memory in the shared space
	void* dataPtr = allocateSharedMemory(DI.dataId, DI.totalSize);
	//Write to shared memory
	copyRData(dataPtr, data, DI.typeId, DI.totalSize);
	//Record the data info
	insertDataInfo(DI);
	DEBUG(printf("A shared object is created with id: %llu\n", DI.dataId));
	return dataPtr;
}

/*
allocate a shared memory given the ID and size

return the data pointer to the shared memory
*/

void* allocateSharedMemory(DID dataID, ULLong size) {
	if (hasDataID(dataID)) {
		errorHandle("Cannot allocate shared memory with the ID %lld: The data ID exists", dataID);
	}
	//Get the string key for the data key
	string dataKey = getDataMemoryKey(dataID);
	try
	{
		//printf("key:%llu\n", dataID);
		boost::interprocess::permissions perm;
		perm.set_unrestricted();
		CREATE_SHARED_MEM(sharedData, create_only, dataKey.c_str(), read_write, perm, size);
		sharedMemoryList.insert(pair<DID, OS_shared_memory_object*>(dataID, sharedData));
		return(readSharedObject(dataID, dataKey.c_str()));
	}
	catch (const std::exception & ex) {
		removeSharedMemory(dataKey.c_str());
		errorHandle("Can't create a shared object, data ID %llu, has key:%d \n%s", dataID, hasDataID(dataID), ex.what());
	}
	return nullptr;
}



// update dataInfoMap and usedKeySet
void insertDataInfo(dataInfo DI) {
	try
	{
		//Insert the data info into the record
		dataInfoMap->insert(dataInfoPair(DI.dataId, DI));
	}
	catch (const std::exception & ex) {
		errorHandle("error in recording a shared memory info\n");
	}
}

/*
Copy the data from an R object to a shared memory space
If the R object is of string type, the pointer should be the SEXP
Otherwise, the pointer is raw data.
*/
void copyRData(void* target, const void* RData, int typeID, ULLong size) {
	try
	{
		switch (typeID) {
		case STR_TYPE:
			strCpy(target, RData);
			break;
		default:
			memcpy(target, RData, size);
		}
	}
	catch (const std::exception & ex) {
		errorHandle(string("Can't assign values to the shared memory,\n") + ex.what());
	}
}



void* readSharedObject(DID dataID) {
	string signature = getDataMemoryKey(dataID);
	return(readSharedObject(dataID, signature.c_str()));
}

/*
Read a shared object by ID
If the shared oject is not recorded in sharedMemoryList or segmentList, it will be recorded
If the shared object does not exist, an error will be thrown.
*/
void* readSharedObject(DID dataID, const char* signature) {
	initialSharedMemory();
	try
	{
		OS_shared_memory_object* sharedData;
		mapped_region* region;
		if (sharedMemoryList.find(dataID) == sharedMemoryList.end()) {
			sharedData = new OS_shared_memory_object(open_only, signature, read_write);
			sharedMemoryList.insert(pair<DID, OS_shared_memory_object*>(dataID, sharedData));
		}
		else {
			sharedData = sharedMemoryList[dataID];
		}
		if (segmentList.find(dataID) == segmentList.end()) {
			region = new mapped_region(*sharedData, read_write);
			//Record the shared memory and region
			segmentList.insert(pair<DID, mapped_region*>(dataID, region));
		}
		else {
			region = segmentList[dataID];
		}

		return region->get_address();
	}
	catch (const std::exception & ex) {
		removeVectorKeyAndValueIfExist(sharedMemoryList, dataID);
		removeVectorKeyAndValueIfExist(segmentList, dataID)
		errorHandle("Can't read shared object:%s\n", ex.what());
	}
	return(NULL);
}


void destroyObject(DID dataID) {
	initialSharedMemory();
	try
	{
		if (!hasDataID(dataID)) {
			warningHandle("The key %llu does not exist in the data info map\n", dataID);
		}
		else {
			string dataKey = getDataMemoryKey(dataID);
			DEBUG(printf("removing data %llu\n", dataID));
			//printf("removing memory mapped address\n");
			removeVectorKeyAndValueIfExist(segmentList, dataID);
			//printf("removing share memory object\n");
			removeVectorKeyAndValueIfExist(sharedMemoryList, dataID);
			//remove the data
			bool removed = removeSharedMemory(dataKey.c_str());
			if (!removed) printf("fail to remove the data\n");
			//remove the key from the data info map
			//printf("removing data key\n");
			removeDataID(dataID);
		}
	}
	catch (const std::exception & ex)
	{
		errorHandle("Unable to remove the shared memory %llu:\n%s", dataID, ex.what());
	}
}

dataInfo& getDataInfo(DID dataID) {
	initialSharedMemory();
	try {
		if (!hasDataID(dataID)) {
			errorHandle("Cannot find the information of data %llu\n", dataID);
		}
		return dataInfoMap->at(dataID);
	}
	catch (const std::exception & ex) {
		errorHandle("Unexpected error while trying to open %llu\n", dataID);
	}

	//This return is only for keeping the annoying warning silent
	dataInfo* di = new dataInfo();
	return *di;
}



std::vector<double> getDataIdList() {
	initialSharedMemory();
	std::vector<double> v;
	auto iter = dataInfoMap->begin();
	for (; iter != dataInfoMap->end(); iter++)
	{
		v.push_back(iter->first);
	}
	return v;
}

