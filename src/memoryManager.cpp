#ifdef _WIN32
#define WINDOWS_OS
#endif
#ifdef  WINDOWS_OS 
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#define OS_managed_shared_memory managed_windows_shared_memory
#define OS_shared_memory_object windows_shared_memory 
#define OS_SHARED_OBJECT_PKG_SPACE "Local\\shared_object_package_space"
#define OS_DATA_INFO_MAP_NAME "Local\\data_info_map"
#else
#include <boost/interprocess/managed_shared_memory.hpp>
#define OS_managed_shared_memory managed_shared_memory
#define OS_shared_memory_object shared_memory_object 
#define OS_SHARED_OBJECT_PKG_SPACE "shared_object_package_space"
#define OS_DATA_INFO_MAP_NAME "data_info_map"
#endif //  WINDOWS_OS 

#include <boost/interprocess/allocators/allocator.hpp>
//#include <boost/exception/diagnostic_information.hpp>
//#include <boost/exception_ptr.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/foreach.hpp>
#include "tools.h"
#include "memoryManager.h"
#include <map>
#include <vector>
using namespace boost::interprocess;
using std::string;
using std::pair;
using std::to_string;
//The name of the memoryManager system
#define SHARED_OBJECT_PKG_SPACE OS_SHARED_OBJECT_PKG_SPACE
//The name of data info map
#define DATA_INFO_MAP_NAME OS_DATA_INFO_MAP_NAME




#define DATA_LIST_SIZE 1024*1024*8


enum sharedSystem { MSM, SMO };

//Define dataID to dataInfo map
typedef std::pair<const DID, dataInfo> dataInfoPair;
typedef allocator<dataInfoPair, OS_managed_shared_memory::segment_manager> dataInfoAllocator;
typedef map<DID, dataInfo, std::less<DID>, dataInfoAllocator> sharedDataInfoMap;



std::map<DID, OS_shared_memory_object*> sharedMemoryList;
std::map<DID, mapped_region*> segmentList;
//The main segment that maps the shared memory into the current process
//The shared memory contains dataInfoMap
OS_managed_shared_memory* processInfoSegment = nullptr;
sharedDataInfoMap* dataInfoMap = nullptr;


#define removeVectorKeyAndValueIfExist(vec,key)\
if (vec.find(key) != vec.end()) {\
delete(vec.at(key));\
vec.erase(key);\
}\


bool valid(const char* name, sharedSystem ss)
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

bool removeSharedMemory(const char* name) {
#ifndef WINDOWS_OS 
	return OS_shared_memory_object::remove(name);
#endif
	return true;
}

bool hasDataID(DID did) {
	if (dataInfoMap->find(did) == dataInfoMap->end()) {
		return false;
	}
	else {
		return true;
	}
}
bool removeDataID(DID did) {
	dataInfoMap->erase(did);
	return true;
}

string getDataMemoryKey(DID did) {
	//common name + DID
	return(string(SHARED_OBJECT_PKG_SPACE).append("DID") + to_string(did));
}


OS_managed_shared_memory* openOrCreateSharedSegment(const char* name, size_t size) {
	OS_managed_shared_memory* segment;

	boost::interprocess::permissions perm;
	perm.set_unrestricted();
	if (!valid(name, sharedSystem::MSM)) {
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



void initialSharedMemory() {
	try
	{
		//If it is the first time to run the program, check if the shared memory exist
		//If it does not exist, create it
		if (processInfoSegment == nullptr) {
			//printf("not initialized\n");
			//Create shared memory space if not exist
			processInfoSegment = openOrCreateSharedSegment(SHARED_OBJECT_PKG_SPACE, DATA_LIST_SIZE);
			dataInfoMap = openOrCreateSharedMap< dataInfoAllocator, sharedDataInfoMap, DID>(processInfoSegment, DATA_INFO_MAP_NAME);

			//processInfoMap = segment.find<sharedProcessInfoMap>(PROCESS_SHARED_NAME).first;
		}
	}
	catch (const std::exception & ex) {
		errorHandle("error in initialize the shared memory\n%s", ex.what());
	}
}


/*
#####################above is internal API###########################
*/

DID findAvailableKey(DID dataID) {
	initialSharedMemory();
	while (dataInfoMap->find(dataID) != dataInfoMap->end()) {
		dataID += 1;
	}
	return dataID;
}


#ifdef WINDOWS_OS
#define CREATE_SHARED_MEM(obj,openType,name,mode,permission,size) \
OS_shared_memory_object* obj=new OS_shared_memory_object(openType, name, mode,size, permission);
#else
#define CREATE_SHARED_MEM(obj,openType,name,mode,permission,size) \
OS_shared_memory_object* obj=new OS_shared_memory_object(openType, name, mode, permission); \
obj->truncate(size);
#endif

/*
Create a shared memory
add the shared_memory object into the sharedMemoryList
add the maooed_region object into the segmentList
If fail, automatically remove the key(dataID) from the sharedMemoryList and segmentList
*/
void* reserveSpace(DID dataID, ULLong size) {
	//Get the string key for the data key
	string dataKey = getDataMemoryKey(dataID);
	try
	{
		boost::interprocess::permissions perm;
		perm.set_unrestricted();
		CREATE_SHARED_MEM(sharedData, create_only, dataKey.c_str(), read_write, perm, size);
		sharedMemoryList.insert(pair<DID, OS_shared_memory_object*>(dataID, sharedData));
		//mapped_region region(*sharedData, read_write);
		mapped_region* region = new mapped_region(*sharedData, read_write);
		//segmentList
		segmentList.insert(pair<DID, mapped_region*>(dataID, region));
		return(region->get_address());
	}
	catch (const std::exception & ex) {
		removeSharedMemory(dataKey.c_str());
		removeVectorKeyAndValueIfExist(sharedMemoryList, dataID);
		removeVectorKeyAndValueIfExist(segmentList, dataID)
			errorHandle(string("Can't open shared object,\n") + ex.what());
	}
	return nullptr;
}

void insertDataInfo(const dataInfo DI) {
	try
	{
		//Insert the data info into the record
		dataInfoMap->insert(dataInfoPair(DI.dataID, DI));
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

void createSharedObject(const void* data, const dataInfo DI) {
	initialSharedMemory();
	//Allocate memory in the shared space
	void* dataPtr = reserveSpace(DI.dataID, DI.totalSize);
	//Write to shared memory
	copyRData(dataPtr, data, DI.typeID, DI.totalSize);
	//Record the data info
	insertDataInfo(DI);
}



void* readSharedObject(DID dataID) {
	initialSharedMemory();
	string signature = getDataMemoryKey(dataID);
	try
	{
		if (segmentList.find(dataID) != segmentList.end()) {
			return segmentList[dataID]->get_address();
		}
		else {
			errorHandle("The key %llu does not exist", dataID);
		}
	}
	catch (const std::exception & ex) {
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
			//remove the data
			bool removed = removeSharedMemory(dataKey.c_str());
			if (!removed) printf("fail to remove the data\n");
			//remove the key from the data info map
			//printf("removing data key\n");
			removeDataID(dataID);
			//printf("removing memory mapped address\n");
			removeVectorKeyAndValueIfExist(segmentList, dataID);
			//printf("removing share memory object\n");
			removeVectorKeyAndValueIfExist(sharedMemoryList, dataID);
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
			errorHandle("The infomation of data %llu does not exist\n", dataID);
		}
		return dataInfoMap->at(dataID);
	}
	catch (const std::exception & ex) {
		errorHandle("Unexpected error while trying to open %llu\n", dataID);
	}
	dataInfo tmp;
	return(tmp);
}



std::vector<double> getDataIDList() {
	initialSharedMemory();
	std::vector<double> v;
	BOOST_FOREACH(dataInfoPair & dip, *dataInfoMap) {
		v.push_back(dip.first);
	}
	return v;
}



