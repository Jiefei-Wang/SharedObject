#ifndef _WIN32
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
#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception_ptr.hpp>
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


typedef std::pair<const DID, dataInfo> dataInfoPair;
typedef allocator<dataInfoPair, OS_managed_shared_memory::segment_manager> dataInfoAllocator;
typedef map<DID, dataInfo, std::less<DID>, dataInfoAllocator> sharedDataInfoMap;



std::map<DID, OS_shared_memory_object*> sharedMem_list;
std::map<DID, mapped_region*> segment_list;
OS_managed_shared_memory* processInfoSegment=nullptr;
sharedDataInfoMap* dataInfoMap = nullptr;


#define removeDataIfExist(vec,key)\
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
bool hasDID(DID did) {
	if (dataInfoMap->find(did) == dataInfoMap->end()) {
		return false;
	}
	else {
		return true;
	}
}
bool removeDID(DID did) {
	dataInfoMap->erase(did);
	return true;
}

string getDataMemKey(DID did) {
	//common name + DID
	return(string(SHARED_OBJECT_PKG_SPACE).append("DID") + to_string(did));
}


OS_managed_shared_memory* openOrCreateSharedSegment(const char* name, size_t size) {
	OS_managed_shared_memory* segment;

	boost::interprocess::permissions perm;
	perm.set_unrestricted();
	if (!valid(name, sharedSystem::MSM)) {
		removeSharedMemory(name);
		segment = new OS_managed_shared_memory(create_only, name, size,0, perm);
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
	if (processInfoSegment==nullptr) {
		//printf("not initialized\n");
		//Create shared memory space if not exist
		processInfoSegment = openOrCreateSharedSegment(SHARED_OBJECT_PKG_SPACE, DATA_LIST_SIZE);
		dataInfoMap = openOrCreateSharedMap< dataInfoAllocator, sharedDataInfoMap, DID>(processInfoSegment, DATA_INFO_MAP_NAME);

		//processInfoMap = segment.find<sharedProcessInfoMap>(PROCESS_SHARED_NAME).first;
	}
	}
	catch (const std::exception& ex) {
		errorHandle("error in initialize the shared memory\n%s", ex.what());
	}
}







#ifdef WINDOWS_OS
#define CREATE_SHARED_MEM(obj,openType,name,mode,permission,size) \
OS_shared_memory_object* obj=new OS_shared_memory_object(openType, name, mode,size, permission);
#else
#define CREATE_SHARED_MEM(obj,openType,name,mode,permission,size) \
OS_shared_memory_object* obj=new OS_shared_memory_object(openType, name, mode, permission); \
obj->truncate(size);
#endif



DID createSharedOBJ(void* data, int type, ULLong total_size, ULLong length, PID pid,DID did) {
	initialSharedMemory();
	while (dataInfoMap->find(did) != dataInfoMap->end()) {
		did += 1;
	}
	//Write the data into shared space
	string dataKey = getDataMemKey(did);
	try
	{
		boost::interprocess::permissions perm;
		perm.set_unrestricted();
		CREATE_SHARED_MEM(sharedData, create_only, dataKey.c_str(), read_write, perm, total_size);
		sharedMem_list.insert(pair<DID, OS_shared_memory_object*>(did, sharedData));
		//Map the whole shared memory in this process
		mapped_region region(*sharedData, read_write);

		//Write to shared memory
		memcpy(region.get_address(), data, total_size);

	}
	catch (const std::exception & ex) {
		removeSharedMemory(dataKey.c_str());
		removeDataIfExist(sharedMem_list, did);
		errorHandle(string("Can't open shared object,\n") + ex.what());
	}
	//Record the data info
	try
	{
		//Insert the data info into the record
		dataInfo di;
		di.pid=pid;
		di.length=length;
		di.size = total_size;
		di.type = type;
		dataInfoMap->insert(dataInfoPair(did, di));
	}
	catch (const std::exception & ex) {
		errorHandle("error in record a shared memory\n");
	}
	return(did);

}



void* readSharedOBJ(DID did) {
	initialSharedMemory();
	string signature = getDataMemKey(did);
	try
	{
		if (segment_list.find(did) != segment_list.end()) {
			return segment_list[did]->get_address();
		}
		OS_shared_memory_object shm(open_only, signature.c_str(), read_write);
		//Map the whole shared memory in this process
		mapped_region* region = new mapped_region(shm, read_write);
		//segment_list
		segment_list.insert(pair<DID, mapped_region*>(did, region));
		return(region->get_address());
	}
	catch (const std::exception& ex){
		errorHandle("Can't read shared object:%s\n" , ex.what());
	}
	return(NULL);
}


void destroyObj(DID did) {
	initialSharedMemory();
	try
	{
		if (!hasDID(did)) {
			warningHandle("The key %llu does not exist in the data info map\n", did);
		}
		else {
			string dataKey = getDataMemKey(did);
			printf("removing data %llu\n", did);
			//remove the data
			bool removed = removeSharedMemory(dataKey.c_str());
			if (!removed) printf("fail to remove the data\n");
			//remove the key from the data info map
			printf("removing data key\n");
			removeDID(did);
			printf("removing memory mapped address\n");
			removeDataIfExist(segment_list, did);
			printf("removing share memory object");
			removeDataIfExist(sharedMem_list, did);
		}
	}
	catch (const std::exception & ex)
	{
		errorHandle("Unable to remove the shared memory %llu:\n%s", did, ex.what());
	}
}

dataInfo getDataInfo(DID did) {
	initialSharedMemory();
	try {
		if (!hasDID(did)) {
			errorHandle("The data %llu does not exist\n", did);
		}
		return dataInfoMap->at(did);
	}
	catch (const std::exception & ex) {
		errorHandle("Unexpected error while trying to open %llu\n",did);
	}
	dataInfo di;
	return(di);
}



std::vector<double> getDataID() {
	initialSharedMemory();
	std::vector<double> v;
	BOOST_FOREACH(dataInfoPair &dip, *dataInfoMap) {
		v.push_back(dip.first);
	}
	return v;
}



