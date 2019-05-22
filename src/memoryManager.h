#pragma once
#include <string>
#include "sharedObject_types.h"
#include <vector>





struct dataInfo {
#define X(id,type, name) type name;
	DATAINFO_FIELDS
#undef X
};


DID findAvailableKey(DID did);
void* reserveSpace(DID did,ULLong size);
void insertDataInfo(const dataInfo di);
void createSharedOBJ(void* data,const dataInfo di);
void* readSharedOBJ(DID did);
void destroyObj(DID did);
std::string getDataMemKey(DID did);
dataInfo getDataInfo(DID did);
std::vector<double> getDataID();


