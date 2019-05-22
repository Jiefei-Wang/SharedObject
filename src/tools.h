#include <string>
#include "sharedObject_types.h"

#define DEBUG(x);
#define asString(x) std::string(CHAR(asChar(x)))
#define LOGICAL_TYPE 1L
#define INT_TYPE 2L
#define REAL_TYPE 3L
#define RAW_TYPE 4L
#define STR_TYPE 5L
#define REF_SLOT(x,name) ((Environment)x.attr(".xData"))[name]
#define ADDRESS_BYTE 8L

#define DATAINFO_FIELDS_NUMBER 7
#define DATAINFO_FIELDS \
    X(0,DID, did) \
    X(1,PID, pid) \
    X(2,int, type_id) \
    X(3,ULLong, length) \
    X(4,ULLong, total_size) \
    X(5,bool, copyOnWrite) \
    X(6,bool, sharedSub) 

#define X(id,type, name) extern const ULLong dataInfo_##name;
DATAINFO_FIELDS
#undef X


void errorHandle(std::string msg);
void errorHandle(const char* fmt, ...);
void warningHandle(std::string msg);
void warningHandle(const char* fmt, ...);

void messageHandle(std::string msg);
void messageHandle(const char* fmt, ...);
int getTypeSize(unsigned int type);

void strCpy(void* target, void* R_str);



