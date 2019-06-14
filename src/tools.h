#include <string>
#include "SharedObject_types.h"
#define PACKAGE_NAME "SharedObject"
#define PACKAGE_ENV R_FindNamespace(Rf_mkString(PACKAGE_NAME))
#define PACKAGE_FUNC(x) Rf_findFun(Rf_install(x),R_FindNamespace(Rf_mkString(PACKAGE_NAME)))

#define DEBUG(x) ;
#define asString(x) std::string(CHAR(asChar(x)))


#define LOGICAL_TYPE 1L
#define INT_TYPE 2L
#define REAL_TYPE 3L
#define RAW_TYPE 4L
#define STR_TYPE 5L
#define REF_SLOT(x,name) ((Environment)x.attr(".xData"))[name]
#define ADDRESS_BYTE 8L

#define DATAINFO_FIELDS_NUMBER 8
#define DATAINFO_FIELDS \
    X(0,DID, dataID) \
    X(1,PID, processID) \
    X(2,int, typeID) \
    X(3,ULLong, length) \
    X(4,ULLong, totalSize) \
    X(5,bool, copyOnWrite) \
    X(6,bool, sharedSubset)\
	X(7,bool, sharedCopy)

#define X(id,type, name) extern const ULLong dataInfo_##name;
DATAINFO_FIELDS;
#undef X

extern std::string OS_ADDRESS_SIZE;

void errorHandle(std::string msg);
void errorHandle(const char* fmt, ...);
void warningHandle(std::string msg);
void warningHandle(const char* fmt, ...);

void messageHandle(std::string msg);
void messageHandle(const char* fmt, ...);
int getTypeSize(unsigned int type);

void strCpy(const void* target, const void* R_str);



