#include <string>
struct processInfo;
struct dataInfo;

typedef unsigned long long int ULLong;
typedef int PID;
typedef unsigned int DID;
//struct processInfo;
//struct dataInfo;
void destroyAllObj();
void destroyAllObj(PID pid);
void destroyObj(DID did);
void destroyObj(int* objList, size_t size);

void* readSharedOBJ(DID did);
std::string getDataMemKey(DID did);
DID createSharedOBJ(void* data, int type, size_t total_size, PID pid);
void showDataInfo(PID pid);

size_t getProcessNum();
size_t getDataNum(PID pid);

template<typename T>
void getProcessID(T* idList);

template<typename T>
void getDataID(PID pid, T* idList);