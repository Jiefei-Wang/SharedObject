#ifndef HEADER_UTILS
#define HEADER_UTILS

#include <string>
#define PACKAGE_NAME "SharedObject"
#define PACKAGE_ENV R_FindNamespace(Rf_mkString(PACKAGE_NAME))
#define PACKAGE_FUNC(x) Rf_findFun(Rf_install(x), R_FindNamespace(Rf_mkString(PACKAGE_NAME)))

extern const std::string OS_ADDRESS_SIZE;

//Debug print function
void sharedMemoryPrint(const char *format, ...);
void altrepPrint(const char *format, ...);
void packagePrint(const char *format, ...);
std::string getSharedMemoryPath();

size_t getTypeSize(int type);
void loadLibrary();


//Catch all errors as a runtime error
//Otherwise, the error will be an R error
class ERROR_CATCHER
{
public:
  static size_t counter;
  ERROR_CATCHER();
  ~ERROR_CATCHER();
};
void throwError(const char *format, ...);



#ifdef Rcpp_hpp
#define UTILS_ENABLE_R
#endif

#ifdef UTILS_ENABLE_R
#include <Rcpp.h>
uint64_t getObjectSize(SEXP x);
void copyData(void *target, SEXP source);
class PROTECT_GUARD
{
private:
  int protect_num;

public:
  PROTECT_GUARD();
  ~PROTECT_GUARD();
  SEXP protect(SEXP x);
};
#endif
#endif
