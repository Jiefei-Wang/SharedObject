#include "tools.h"
#include "R.h"
#include <Rinternals.h>
#include <cstdarg>
using namespace std;
#define HANDLE_CHAR(FUNC) {\
	va_list args;\
	char buf[1000];\
	va_start(args, fmt);\
	vsnprintf(buf, sizeof(buf), fmt, args);\
	va_end(args);\
	FUNC(buf);\
	}



const string OS_ADDRESS_SIZE = "X" + to_string(sizeof(void*) * 8);

void errorHandle(string msg) {
	errorHandle(msg.c_str());

}
void errorHandle(const char* fmt, ...) {
	HANDLE_CHAR(error);
}
void warningHandle(string msg) {
	warningHandle(msg.c_str());
}
void warningHandle(const char* fmt, ...) {
	HANDLE_CHAR(warning)
}

void messageHandle(std::string msg) {
	messageHandle(msg.c_str());
}
void messageHandle(const char* fmt, ...) {
	HANDLE_CHAR(Rprintf);
}


int getTypeSize(unsigned int type) {
	switch (type) {
	case INT_TYPE:
		return 4;
	case LOGICAL_TYPE:
		return 4;
	case REAL_TYPE:
		return 8;
	case RAW_TYPE:
		return 1;
	}
	errorHandle("Unexpected data type");
	return 0;
}

/*
data structure:
offset to the ith string,i=1,...,n.(64bit offset)
actual data
*/

void strCpy(const void* target, const void* R_str) {
	SEXP str = (SEXP)R_str;
	//printf("get string length\n");
	ULLong n = XLENGTH(str);
	ULLong* ptr = (ULLong*)target;
	char* data = (char*)(ptr + n);
	ULLong curOff = n * sizeof(ULLong);
	for (ULLong i = 0; i < n; i++) {
		//get an element from the string vector
		SEXP ele = STRING_ELT(str, i);
		R_xlen_t ele_size = XLENGTH(ele);
		const char* ele_char = CHAR(ele);
		//set the offset to the data
		*ptr = curOff;
		ptr = ptr + 1;
		curOff = curOff + ele_size + 1;
		//perform the memory copy
		memcpy(data, ele_char, ele_size + 1);
		data = data + ele_size + 1;
	}
}
