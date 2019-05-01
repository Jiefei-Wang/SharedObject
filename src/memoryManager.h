#include <string>
#include "sharedObject_types.h"
struct processInfo {
	ULLong object_num = 0;
	ULLong total_size = 0;
};
struct dataInfo {
	ULLong length;
	ULLong size;
	int type;
};


//struct processInfo;
//struct dataInfo;
void destroyAllObj(bool output);
void destroyAllObj(PID pid, bool output);
void destroyObj(DID did);

void* readSharedOBJ(DID did);
std::string getDataMemKey(DID did);
DID createSharedOBJ(void* data, int type, ULLong total_size, ULLong length, PID pid);

size_t getProcessNum();
size_t getDataNum(PID pid);
void getProcessIDs(double* idList);
void getDataIDs(PID pid, double* idList);
const processInfo& getProcessInfo(PID pid);
const dataInfo getDataInfo(PID pid, DID did);
PID getDataPID(DID did);

size_t getDataCount();
size_t getFreedKeyNum();
void getFreedAllKeys(double* key);


