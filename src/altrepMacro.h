#pragma once
#define PGKNAME "sharedObject"
#include "C_memoryManagerInterface.h"

//Shared memory basic macros
#define SM_ENV(x) Rf_getAttrib(x,Rf_install(".xData"))
#define SM_DATA(sm,data) Rf_findVar(Rf_install(#data), SM_ENV(sm))
#define SM_FUN(sm,fun) Rf_findFun(Rf_install(#fun), SM_ENV(sm))

//shared vector basic macros
#define SV_ENV(x) SM_ENV(R_altrep_data1(x))
#define SV_DATA(x,data) SM_DATA(R_altrep_data1(x),data)
#define SV_FUN(x,fun) SM_FUN(R_altrep_data1(x),fun)

//Access to shared vector's slots
#define SV_EPTR(x) SV_DATA(x,address)
#define SV_PTR(x) R_ExternalPtrAddr(SV_EPTR(x))
#define SV_UPDATE_FUN(x) SV_FUN(x,updateAddress)
#define SV_DATAID(x) (Rf_asInteger(SV_DATA(x,did)))
#define SV_TYPE_NAME(x) CHAR(Rf_asChar((SV_DATA(x, type))))


#define SV_PROCESSID(x) C_getProcessID(SV_DATAID(x))
#define SV_TYPEID(x) C_getTypeID(SV_DATAID(x))
#define SV_LENGTH(x) C_getLength(SV_DATAID(x))
#define SV_TOTAL_SIZE(x) C_getTotalSize(SV_DATAID(x))
#define SV_COPY_ON_WRITE(x) C_getCopyOnWrite(SV_DATAID(x))
#define SV_SHARED_SUBSET(x)  C_getSharedSubset(SV_DATAID(x))
#define SV_SHARED_DUPLICATE(x)  C_getSharedDuplicate(SV_DATAID(x))
