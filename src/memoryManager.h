#pragma once
#include <string>
#include <vector>
#include "SharedObject_types.h"



struct dataInfo {
#define X(id,type, name) type name;
	DATAINFO_FIELDS
#undef X
};

bool hasDataID(DID did);
DID findAvailableDataId(DID dataID);
void* allocateSharedMemory(DID dataID,ULLong size);
void insertDataInfo(dataInfo DI);
void copyRData(void* target, const void* RData, int typeID, ULLong size);
void* createSharedObject(const void* data, dataInfo DI);
void* readSharedObject(DID dataID);
void* readSharedObject(DID dataID, const char* signature);
void destroyObject(DID dataID);
std::string getDataMemoryKey(DID dataID);
dataInfo& getDataInfo(DID dataID);
std::vector<double> getDataIdList();

