#include <string>

#define asString(x) std::string(CHAR(asChar(x)))
#define BOOL_TYPE 1L
#define INT_TYPE 2L
#define REAL_TYPE 3L

void errorHandle(std::string msg);
void errorHandle(const char* msg);
void warningHandle(std::string msg);
void warningHandle(const char* msg);

int getTypeSize(unsigned int type);
