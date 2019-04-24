
#include <R_ext/RS.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#include "C_Interface.h"
#include "altrep_real_class.h"



static const R_CallMethodDef callMethods[] = {
  { "testFunc", (DL_FUNC)& testFunc, 2 },
  { "createSharedMemory", (DL_FUNC)& createSharedMemory, 4 },
  { "readSharedMemory", (DL_FUNC)& readSharedMemory, 1 },
  { "getValue_32", (DL_FUNC)& getValue_32, 3 },
  { "clearAll", (DL_FUNC)& clearAll, 1 },
  { "clearObj", (DL_FUNC)& clearObj, 1 },
  { "createAltrep", (DL_FUNC)& createAltrep, 4 },
  { "getDataCount", (DL_FUNC)& R_getDataCount, 0 },
  { "getFreedKeys", (DL_FUNC)& R_getFreedKeys, 0 },
  { "getProcessIDs", (DL_FUNC)& R_getProcessIDs, 0 },
  { "getDataIDs", (DL_FUNC)& R_getDataIDs, 1 },
  { "getProcessInfo", (DL_FUNC)& R_getProcessInfo, 0 },
  { "getDataInfo", (DL_FUNC)& R_getDataInfo, 1 },
  { "getDataPID", (DL_FUNC)& R_getDataPID, 1 },
  { "recoverDataInfo", (DL_FUNC)& R_recoverDataInfo, 1 },
  { "attachAttr", (DL_FUNC)& attachAttr, 3 },
  { NULL, NULL, 0 }
};


extern "C"
void R_init_sharedObject(DllInfo * info)
{
  //Normal functions registration
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
  R_useDynamicSymbols(info, FALSE);
  R_forceSymbols(info, TRUE);

  //altrep class registration
  InitRealClass(info);
}



