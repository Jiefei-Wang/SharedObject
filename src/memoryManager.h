#pragma once
#include <string>
#include "SharedObject_types.h"
#include <vector>



struct dataInfo {
#define X(id,type, name) type name;
	DATAINFO_FIELDS
#undef X
};


DID findAvailableKey(DID dataID);
void* reserveSpace(DID dataID,ULLong size);
void insertDataInfo(const dataInfo DI);
void copyRData(void* target, const void* RData, int typeID, ULLong size);
void createSharedObject(const void* data,const dataInfo DI);
void* readSharedObject(DID dataID);
void* readSharedObject(DID dataID, const char* signature);
void destroyObject(DID dataID);
std::string getDataMemoryKey(DID dataID);
dataInfo& getDataInfo(DID dataID);
std::vector<double> getDataIDList();


