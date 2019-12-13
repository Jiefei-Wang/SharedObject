#include "tools.h"
#include "R.h"
#include <Rinternals.h>
#include <cstdarg>
using namespace std;

const string OS_ADDRESS_SIZE = "X" + to_string(sizeof(void*) * 8);


