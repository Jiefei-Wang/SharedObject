#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception_ptr.hpp>
#include "tools.h"
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/set.hpp>
#include "memoryManager.h"
#include <map>
using namespace boost::interprocess;
using std::string;
using std::pair;
using std::to_string;
//The shared space that is shared across all spaces, it is used to shared the basic info
#define PROCESS_SHARED_NAME "R_shared_memory_process"
//The map name that stores the info of each process. it is under PROCESS_SHARED_NAME
#define PROCESS_INFO_MAP_NAME "R_shared_memory_process_info_map"
//The map name that stores the data id to process id links. it is under PROCESS_SHARED_NAME
#define DATA_PROCESS_MAP_NAME "R_shared_memory_data_process_map"
//The map name that stores the unused key. it is under PROCESS_SHARED_NAME
#define FREED_KEY_SET_NAME "R_shared_freed_key_set"
//Number of shared memory
#define DATA_COUNT_NAME "R_shared_memory_data_count"

#define PROCESS_LIST_SIZE 1024*1024*64
#define DATA_LIST_SIZE 1024*1024*8


enum sharedSystem { MSM, SMO };






typedef std::pair<const PID, processInfo> processInfoPair;
typedef allocator<processInfoPair, managed_shared_memory::segment_manager> ProcessInfoAllocator;
typedef map<PID, processInfo, std::less<PID>, ProcessInfoAllocator> sharedProcessInfoMap;

typedef std::pair<const DID, PID> dataProcessPair;
typedef allocator<dataProcessPair, managed_shared_memory::segment_manager> dataProcessMapAllocator;
typedef map<DID, PID, std::less<DID>, dataProcessMapAllocator> sharedDataProcessMap;

typedef allocator<DID, managed_shared_memory::segment_manager> freedKeySetAllocator;
typedef set< DID, std::less<DID>, freedKeySetAllocator> sharedFreedKeySet;

typedef std::pair<const DID, dataInfo> dataInfoPair;
typedef allocator<dataInfoPair, managed_shared_memory::segment_manager> dataInfoAllocator;
typedef map<DID, dataInfo, std::less<DID>, dataInfoAllocator> processDataInfoMap;





std::map<DID, mapped_region*> segment_list;



//global memory
PID current_pid = 0;
//Shared objects
ULLong* data_count;
sharedProcessInfoMap* processInfoMap = nullptr;
sharedDataProcessMap* dataProcessMap = nullptr;
sharedFreedKeySet* freedKeySet = nullptr;
//private objects for the current process
processInfo* current_processInfo = nullptr;
processDataInfoMap* current_dataMap = nullptr;


managed_shared_memory* processInfoSegment = nullptr;
managed_shared_memory* processDataInfoSegment = nullptr;


bool valid(const char* name, sharedSystem ss)
{
	try
	{
		if (ss == sharedSystem::MSM) {
			managed_shared_memory sharedData(open_only, name);
			return true;
		}
		if (ss == sharedSystem::SMO) {
			shared_memory_object sharedData(open_only, name, read_write);
			return true;
		}
		errorHandle("incorrect shared system");
	}
	catch (const std::exception & ex) {
		//errorHandle(std::string("managed_shared_memory ex: ")+boost::diagnostic_information(ex));
	}
	return false;
}

string getProcessToDataListKey(PID pid) {
	//common name + PID
	return(string(PROCESS_SHARED_NAME).append("PID") + to_string(pid));
}
string getDataMemKey(DID did) {
	//common name + DID
	return(string(PROCESS_SHARED_NAME).append("DID") + to_string(did));
}

ULLong getNewDataKey() {
	ULLong did;
	if (!freedKeySet->empty()) {
		did = *(freedKeySet->begin());
		freedKeySet->erase(did);
		printf("get id from freed key list: %llu\n", did);
	}
	else {
		did = *data_count;
		*data_count = *data_count + 1;
		printf("get id from data count: %llu\n", did);
	}
	return(did);
}

//free a data key, if the data key is the largest key, initiate garbage collection for the keys.
//TODO:Need a lock
void freeKey(DID did) {
	freedKeySet->insert(did);
	while (freedKeySet->find(*data_count - 1) != freedKeySet->end()) {
		*data_count = *data_count - 1;
		freedKeySet->erase(*data_count);
	}
}


managed_shared_memory* createSharedSegment(const char* name, size_t size) {
	managed_shared_memory* segment;

	boost::interprocess::permissions perm;
	perm.set_unrestricted();
	if (!valid(name, sharedSystem::MSM)) {
		shared_memory_object::remove(name);
		segment = new managed_shared_memory(create_only, name, size,0, perm);
	}
	else {
		segment = new managed_shared_memory(open_only, name);
	}
	return(segment);
}

template<typename alloc, typename mapType, typename mapKey>
mapType* openOrCreateSharedMap(managed_shared_memory * segment, const char* name) {
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



void initialProcessSharedMemory() {
	//If it is the first time to run the program, check if the shared memory exist
	//If it does not exist, create it
	if (processInfoSegment == nullptr) {
		//printf("not initialized\n");
		//Create shared memory space if not exist

		processInfoSegment = createSharedSegment(PROCESS_SHARED_NAME, PROCESS_LIST_SIZE);
		processInfoMap = openOrCreateSharedMap< ProcessInfoAllocator, sharedProcessInfoMap, PID>(processInfoSegment, PROCESS_INFO_MAP_NAME);
		dataProcessMap = openOrCreateSharedMap< dataProcessMapAllocator, sharedDataProcessMap, DID>(processInfoSegment, DATA_PROCESS_MAP_NAME);
		freedKeySet = openOrCreateSharedMap< freedKeySetAllocator, sharedFreedKeySet, DID>(processInfoSegment, FREED_KEY_SET_NAME);
		//processInfoMap = segment.find<sharedProcessInfoMap>(PROCESS_SHARED_NAME).first;
		//Find the data count
		data_count = processInfoSegment->find<ULLong>(DATA_COUNT_NAME).first;
		if (data_count == 0) {
			data_count = processInfoSegment->construct<ULLong>(DATA_COUNT_NAME)(0);
		}
	}
}


void initialSharedMemory(PID pid) {
	if (pid == current_pid) return;
	try
	{
		initialProcessSharedMemory();
		//printf("data count pointer:%p\n", (void*)data_count);
		//printf("data count value:%llu\n", *data_count);
		//printf("processInfoMap pointer:%p\n", processInfoMap);

		//If the process did not register in the map, register it.
		if (processInfoMap->find(pid) == processInfoMap->end()) {
			processInfo pi;
			processInfoMap->insert(processInfoPair(pid, pi));
		}

		//printf("processInfoMap size:%d\n", processInfoMap->size());
		current_processInfo = &(processInfoMap->find(pid)->second);

		//initialize the dataList map
		if (processDataInfoSegment != nullptr) {
			delete(processDataInfoSegment);
		}
		string processDataKey = getProcessToDataListKey(pid);
		processDataInfoSegment = createSharedSegment(processDataKey.c_str(), DATA_LIST_SIZE);
		current_dataMap = openOrCreateSharedMap<dataInfoAllocator, processDataInfoMap, DID>(processDataInfoSegment, processDataKey.c_str());
		current_pid = pid;
	}
	catch (const std::exception & ex) {
		errorHandle(string("error in initialize the shared memory\n") + ex.what());
	}
}

/*
typedef std::pair<const PID, processInfo> processInfoPair;
typedef allocator<processInfoPair, managed_shared_memory::segment_manager> ProcessInfoAllocator;
typedef map<PID, processInfo, std::less<PID>, ProcessInfoAllocator> sharedProcessInfoMap;

typedef std::pair<const DID, dataInfo> dataInfoPair;
typedef allocator<dataInfoPair, managed_shared_memory::segment_manager> dataInfoAllocator;
typedef map<DID, dataInfo, std::less<DID>, dataInfoAllocator> processDataInfoMap;
*/
void destroyObj(processDataInfoMap * curDataListMap, PID pid, DID did);
void destroyObj(PID pid, DID did);


void destroyAllObj(bool verbose) {
	initialProcessSharedMemory();

	for (sharedProcessInfoMap::iterator it = processInfoMap->begin(); it != processInfoMap->end(); ++it) {
		PID curPID = it->first;
		destroyAllObj(curPID, verbose);
	}
	processInfoMap->clear();
	freedKeySet->clear();
	*data_count = 0;
}

void destroyAllObj(PID pid, bool output) {
	initialProcessSharedMemory();
	try
	{
		string processDataListKey = getProcessToDataListKey(pid);
		managed_shared_memory segment(open_only, processDataListKey.c_str());
		processDataInfoMap* curDataListMap = segment.find<processDataInfoMap>(processDataListKey.c_str()).first;
		processDataInfoMap::iterator next_it = curDataListMap->begin();
		for (processDataInfoMap::iterator it = next_it; it != curDataListMap->end(); it = next_it) {
			++next_it;
			DID curDID = it->first;
			if (output) {
				messageHandle("Deleting data %u at process %d\n", curDID, pid);
			}
			destroyObj(curDataListMap, pid, curDID);
		}
		shared_memory_object::remove(processDataListKey.c_str());
	}
	catch (const std::exception & ex)
	{
		errorHandle(string("Unable to remove the shared memory at process ") + to_string(pid).append("\n") + ex.what());
	}
	if (pid == current_pid) {
		current_pid = 0;
	}
}





void destroyObj(DID did) {
	initialProcessSharedMemory();
	try
	{
		if (dataProcessMap->find(did) == dataProcessMap->end()) {
			warningHandle(string("The key ") + to_string(did) + string(" does not exist in the data-process map"));
		}
		else {
			PID pid = (*dataProcessMap)[did];
			printf("PID is %d\n", pid);
			destroyObj(pid, did);
		}
	}
	catch (const std::exception & ex)
	{
		errorHandle(string("Unable to remove the shared memory ") + to_string(did).append("\n") + ex.what());
	}
}
//Assume that the process id is correct
void destroyObj(PID pid, DID did) {
	string processDataListKey = getProcessToDataListKey(pid);
	managed_shared_memory segment(open_only, processDataListKey.c_str());
	processDataInfoMap* curDataListMap = segment.find<processDataInfoMap>(processDataListKey.c_str()).first;
	if (curDataListMap == 0) {
		warningHandle(string("The process ") + to_string(pid) + string(" does not exist in the process-data map"));
	}
	else {
		destroyObj(curDataListMap, pid, did);
	}
}
//Assume curDataListMap and pid are correct
void destroyObj(processDataInfoMap * curDataListMap, PID pid, DID did) {
	string dataKey = getDataMemKey(did);
	printf("removing data %llu\n", did);
	//remove the data
	bool removed=shared_memory_object::remove(dataKey.c_str());
	if(!removed) printf("fail to remove the data\n");
	printf("removing count\n");
	//remove the data count from the process info
	processInfo* curProcessInfo = &processInfoMap->find(pid)->second;
	curProcessInfo->object_num = curProcessInfo->object_num - 1;
	size_t size = curDataListMap->find(did)->second.size;
	curProcessInfo->total_size = curProcessInfo->total_size - size;
	printf("removing data-key\n");
	//remove the data id from the process data-key list
	curDataListMap->erase(did);
	printf("removing DID-PID pair\n");
	//remove DID to PID pair
	dataProcessMap->erase(did);
	printf("adding id to freed key list\n");
	freeKey(did);
	//add DID to the freed key list
	printf("removing memory map\n");
	if (segment_list.find(did) != segment_list.end()) {
		delete(segment_list.at(did));
		segment_list.erase(did);
	}
	else {
		warningHandle("The memory map key does not exist\n");
	}
}




DID createSharedOBJ(void* data, int type, ULLong total_size, ULLong length, PID pid) {
	initialSharedMemory(pid);
	//Write the data into shared space
	DID did = getNewDataKey();
	string dataKey = getDataMemKey(did);
	try
	{
		boost::interprocess::permissions perm;
		perm.set_unrestricted();
		shared_memory_object sharedData(create_only, dataKey.c_str(), read_write, perm);
		//Set size
		sharedData.truncate(total_size);
		//Map the whole shared memory in this process
		mapped_region region(sharedData, read_write);

		//Write to shared memory
		memcpy(region.get_address(), data, region.get_size());
	}
	catch (const std::exception & ex) {
		shared_memory_object::remove(dataKey.c_str());
		errorHandle(string("Can't open shared object,\n") + ex.what());
	}
	//Record the data info
	try
	{
		//Insert the data info into the record
		current_processInfo->object_num = current_processInfo->object_num + 1;
		current_processInfo->total_size = current_processInfo->total_size + total_size;
		dataInfo di;
		di.length=length;
		di.size = total_size;
		di.type = type;
		current_dataMap->insert(dataInfoPair(did, di));
		dataProcessMap->insert(dataProcessPair(did, pid));
	}
	catch (const std::exception & ex) {
		errorHandle("error in record a shared memory\n");
	}
	return(did);
}



void* readSharedOBJ(DID did) {
	string signature = getDataMemKey(did);
	try
	{
		shared_memory_object shm(open_only, signature.c_str(), read_write);
		//Map the whole shared memory in this process
		mapped_region* region = new mapped_region(shm, read_write);
		//segment_list
		segment_list.insert(pair<DID, mapped_region*>(did, region));
		return(region->get_address());
	}
	catch (boost::interprocess::interprocess_exception const& ex) {
		errorHandle(string("Can't read shared object,\n") + boost::diagnostic_information(ex));
	}
	return(NULL);
}

size_t getProcessNum() {
	initialProcessSharedMemory();
	return(processInfoMap->size());
}


size_t getDataNum(PID pid) {
	initialProcessSharedMemory();
	string processDataListKey = getProcessToDataListKey(pid);
	try {
		managed_shared_memory segment(open_only, processDataListKey.c_str());
		processDataInfoMap* curDataListMap = segment.find<processDataInfoMap>(processDataListKey.c_str()).first;
		if (curDataListMap == 0)
			return(0);
		return(curDataListMap->size());
	}
	catch (const std::exception & ex) {
		warningHandle("Fail to open shared memory\n%s\n",ex.what());
	}
	return(0);
}

void getProcessIDs(double* idList) {
	initialProcessSharedMemory();
	int i = 0;
	for (sharedProcessInfoMap::iterator it = processInfoMap->begin(); it != processInfoMap->end(); ++it) {
		idList[i] = it->first;
		i += 1;
	}
}

void getDataIDs(PID pid, double* idList) {
	try {
		size_t size = getDataNum(pid);
		if (size != 0) {
			string processDataListKey = getProcessToDataListKey(pid);
			managed_shared_memory segment(open_only, processDataListKey.c_str());
			processDataInfoMap* curDataListMap = segment.find<processDataInfoMap>(processDataListKey.c_str()).first;
			int i = 0;
			for (processDataInfoMap::iterator it = curDataListMap->begin(); it != curDataListMap->end(); ++it) {
				idList[i] = it->first;
				i += 1;
			}
		}
	}
	catch (const std::exception & ex) {
		errorHandle(string("The process ") + to_string(pid).append(" does not exist.\n"));
	}
}



const processInfo& getProcessInfo(PID pid) {
	initialProcessSharedMemory();
	if (processInfoMap->find(pid) == processInfoMap->end()) {
		errorHandle(string("The process ") + to_string(pid).append(" does not exist.\n"));
	}

	return processInfoMap->at(pid);
}



const dataInfo getDataInfo(PID pid, DID did) {
	initialProcessSharedMemory();
	try {
		string processDataListKey = getProcessToDataListKey(pid);
		managed_shared_memory segment(open_only, processDataListKey.c_str());
		processDataInfoMap* curDataListMap = segment.find<processDataInfoMap>(processDataListKey.c_str()).first;

		if (curDataListMap->find(did) == curDataListMap->end()) {
			errorHandle(string("The data ") + to_string(did).append(" does not exist.\n"));
		}

		return curDataListMap->at(did);
	}
	catch (const std::exception & ex) {
		errorHandle(string("The process ") + to_string(pid).append(" does not exist.\n"));
	}
}

PID getDataPID(DID did) {
  initialProcessSharedMemory();
	if (dataProcessMap->find(did) == dataProcessMap->end()) {
		errorHandle(string("The data ") + to_string(did).append(" does not exist.\n"));
	}
	return(dataProcessMap->at(did));
}


size_t getDataCount() {
	initialProcessSharedMemory();
	return *data_count;
}

size_t getFreedKeyNum() {
	initialProcessSharedMemory();
	return(freedKeySet->size());
}
void getFreedAllKeys(double* key) {
	initialProcessSharedMemory();
	int i = 0;
	for (sharedFreedKeySet::iterator itr = freedKeySet->begin(); itr != freedKeySet->end(); ++itr)
	{
		key[i] = *itr;
		++i;
	}
}





