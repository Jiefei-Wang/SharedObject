
#include <R_ext/RS.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#include "C_Interface.h"


static const R_CallMethodDef callMethods[] = {
  { "testFunc", (DL_FUNC)& testFunc, 1 },
  { "createSharedMemory", (DL_FUNC)& createSharedMemory, 4 },
  { "readSharedMemory", (DL_FUNC)& readSharedMemory, 1 },
  { "getValue_32", (DL_FUNC)& getValue_32, 3 },
  { "clearAll", (DL_FUNC)& clearAll, 0 },
  { "clearObj", (DL_FUNC)& clearObj, 1 },
  { "getProcessList", (DL_FUNC)& getProcessList, 0 },
  { "getDataList", (DL_FUNC)& getDataList, 1 },
  { NULL, NULL, 0 }
};


extern "C"
void R_init_sharedObject(DllInfo * info)
{
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
  R_useDynamicSymbols(info, FALSE);
  R_forceSymbols(info, TRUE);
}
