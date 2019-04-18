#include "tools.h"
#include "R.h"
#include <cstdarg>
using namespace std;
void errorHandle(string msg){
  errorHandle(msg.c_str());

}
void errorHandle(const char* msg){
  error(msg);
}
void warningHandle(string msg){
  warningHandle(msg.c_str());
}
void warningHandle(const char* msg) {
	warning(msg);
}

void messageHandle(const char* fmt,...){
	va_list args;
	char buf[1000];
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	Rprintf(buf);
}

int getTypeSize(unsigned int type){
  switch(type){
  case INT_TYPE:
    return 4;
  case BOOL_TYPE:
    return 4;
  case REAL_TYPE:
    return 8;
  }
  errorHandle("Unexpected data type");
  return 0;
}
