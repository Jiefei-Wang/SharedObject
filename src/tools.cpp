#include "tools.h"
#include "R.h"
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
void warningHandle(const char* msg){
  warning(msg);

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
}
