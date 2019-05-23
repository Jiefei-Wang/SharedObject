#pragma once
#define PGKNAME "sharedObject"

#define GET_INFO(dataInfo,var) REAL(dataInfo)[dataInfo_##var]

#define SM_ENV(x) Rf_getAttrib(x,Rf_install(".xData"))
#define SM_DATA(sm,data) Rf_findVar(Rf_install(#data), SM_ENV(sm))
#define SM_FUN(sm,fun) Rf_findFun(Rf_install(#fun), SM_ENV(sm))

#define SM_DATAINFO(sm,var) GET_INFO(SM_DATA(sm,dataInfo),var)


#define SV_ENV(so) SM_ENV(R_altrep_data1(so))
#define SV_DATA(so,data) SM_DATA(R_altrep_data1(so),data)
#define SV_FUN(so,fun) SM_FUN(R_altrep_data1(so),fun)

#define SV_DATAINFO(so,var) GET_INFO(SV_DATA(so,dataInfo),var)


#define SV_EPTR(x) SV_DATA(x,address)
#define SV_PTR(x) R_ExternalPtrAddr(SV_EPTR(x))
#define SV_UPDATE_FUN(x) SV_FUN(x,updateAddress)
#define SV_DID(x) (SV_DATAINFO(x,did))
#define SV_LENGTH(x) (SV_DATAINFO(x,length))
#define SV_SIZE(x) (SV_DATAINFO(x,total_size))
#define SV_TYPE(x) ((int)(SV_DATAINFO(x, type_id)))
#define SV_TYPE_CHAR(x) CHAR(Rf_asChar((SV_DATA(x, type))))


#include "sharedObject_types.h"
bool C_getCopyOnWrite(DID did);
bool C_getSharedSub(DID did);
bool C_getSharedDuplicate(DID did);
#define SV_COW(x) C_getCopyOnWrite(SV_DID(x))
#define SV_SHAREDSUB(x)  C_getSharedSub(SV_DID(x))
#define SV_SHARED_DUPLICATE(x)  C_getSharedDuplicate(SV_DID(x))