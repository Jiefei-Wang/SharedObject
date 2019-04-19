#include <string>
struct processInfo {
	size_t object_num = 0;
	size_t total_size = 0;
};
struct dataInfo {
	size_t size;
	int type;
};

typedef unsigned long long int ULLong;
typedef int PID;
typedef unsigned int DID;
//struct processInfo;
//struct dataInfo;
void destroyAllObj(bool output);
void destroyAllObj(PID pid, bool output);
void destroyObj(DID did);

void* readSharedOBJ(DID did);
std::string getDataMemKey(DID did);
DID createSharedOBJ(void* data, int type, size_t total_size, PID pid);
void showDataInfo(PID pid);

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


