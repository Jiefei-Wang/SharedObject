#include <string>
#define PACKAGE_NAME "SharedObject"
#define PACKAGE_ENV R_FindNamespace(Rf_mkString(PACKAGE_NAME))
#define PACKAGE_FUNC(x) Rf_findFun(Rf_install(x),R_FindNamespace(Rf_mkString(PACKAGE_NAME)))

#define DEBUG(x) x;
#define asString(x) std::string(CHAR(asChar(x)))


extern const std::string OS_ADDRESS_SIZE;




