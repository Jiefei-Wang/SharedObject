#include <string>

#define asString(x) std::string(CHAR(asChar(x)))


void errorHandle(std::string msg);
void errorHandle(const char* msg);
void warningHandle(std::string msg);
void warningHandle(const char* msg);

enum TYPE{logical=1,integer=2,real=3};
