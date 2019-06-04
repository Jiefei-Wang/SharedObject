#pragma once
#include "C_memoryManagerInterface.h"

//Shared memory basic macros
#define SM_ENV(x) Rf_getAttrib(x,Rf_install(".xData"))
#define SM_DATA(sm,data) Rf_findVar(Rf_install(#data), SM_ENV(sm))
#define SM_FUN(sm,fun) Rf_findFun(Rf_install(#fun), SM_ENV(sm))

#define SM_EPTR(x) SM_DATA(x,address)
#define SM_PTR(x) R_ExternalPtrAddr(SM_EPTR(x))
#define SM_UPDATE_FUN(x) SM_FUN(x,updateAddress)
#define SM_DATAID(x) (Rf_asReal(SM_DATA(x,dataID)))
#define SM_TYPE_NAME(x) CHAR(Rf_asChar((SM_DATA(x, type))))


#define SM_PROCESSID(x) C_getProcessID(SM_DATAID(x))
#define SM_TYPEID(x) C_getTypeID(SM_DATAID(x))
#define SM_LENGTH(x) C_getLength(SM_DATAID(x))
#define SM_TOTAL_SIZE(x) C_getTotalSize(SM_DATAID(x))
#define SM_COPY_ON_WRITE(x) C_getCopyOnWrite(SM_DATAID(x))
#define SM_SHARED_SUBSET(x)  C_getSharedSubset(SM_DATAID(x))
#define SM_SHARED_DUPLICATE(x)  C_getSharedCopy(SM_DATAID(x))

//shared vector basic macros
#define SV_ENV(x) SM_ENV(R_altrep_data1(x))
#define SV_DATA(x,data) SM_DATA(R_altrep_data1(x),data)
#define SV_FUN(x,fun) SM_FUN(R_altrep_data1(x),fun)

//Access to shared vector's slots
#define SV_EPTR(x) SM_EPTR(R_altrep_data1(x))
#define SV_PTR(x) SM_PTR(R_altrep_data1(x))
#define SV_UPDATE_FUN(x) SM_UPDATE_FUN(R_altrep_data1(x))
#define SV_DATAID(x) SM_DATAID(R_altrep_data1(x))
#define SV_TYPE_NAME(x) SM_TYPE_NAME(R_altrep_data1(x))


#define SV_PROCESSID(x) SM_PROCESSID(R_altrep_data1(x))
#define SV_TYPEID(x) SM_TYPEID(R_altrep_data1(x))
#define SV_LENGTH(x) SM_LENGTH(R_altrep_data1(x))
#define SV_TOTAL_SIZE(x) SM_TOTAL_SIZE(R_altrep_data1(x))
#define SV_COPY_ON_WRITE(x) SM_COPY_ON_WRITE(R_altrep_data1(x))
#define SV_SHARED_SUBSET(x)  SM_SHARED_SUBSET(R_altrep_data1(x))
#define SV_SHARED_DUPLICATE(x)  SM_SHARED_DUPLICATE(R_altrep_data1(x))
