#include <string>

#define DEBUG(x) x;
#define asString(x) std::string(CHAR(asChar(x)))
#define BOOL_TYPE 1L
#define INT_TYPE 2L
#define REAL_TYPE 3L
#define REF_SLOT(x,name) ((Environment)x.attr(".xData"))[name]

void errorHandle(std::string msg);
void errorHandle(const char* fmt, ...);
void warningHandle(std::string msg);
void warningHandle(const char* fmt, ...);

void messageHandle(std::string msg);
void messageHandle(const char* fmt, ...);
int getTypeSize(unsigned int type);

