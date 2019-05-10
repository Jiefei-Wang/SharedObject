#pragma once
#include <string>
#include "sharedObject_types.h"
#include <vector>

struct dataInfo {
	PID pid;
	ULLong length;
	ULLong size;
	int type;
	bool copyOnWrite;
	bool sharedSub;
};



DID createSharedOBJ(void* data, int type, ULLong total_size, ULLong length, PID pid, DID did,bool COW,bool sharedSub);
void* readSharedOBJ(DID did);
void destroyObj(DID did);
std::string getDataMemKey(DID did);
dataInfo getDataInfo(DID did);
std::vector<double> getDataID();


