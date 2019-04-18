#include <string>
struct processInfo;
struct dataInfo;

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


void getProcessInfo(double* pid, double* data_num, double* total_size);
void getDataInfo(PID pid, double* did,double* size, double* type);

size_t getDataCount();
size_t getFreedKeyNum();
void getFreedAllKeys(double* key);


