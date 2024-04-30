#include <cstdarg>
#define UTILS_ENABLE_R
#include "utils.h"
#include "R_ext/Itermacros.h"
using namespace std;

const string OS_ADDRESS_SIZE = "X" + to_string(sizeof(void*) * 8);

#define BUFFER_SIZE 16 * 1024
static char buffer[BUFFER_SIZE];

static bool sharedMemoryPrintEnable = false;
static bool altrepPrintEnable = false;
static bool packagePrintEnable = false;


// [[Rcpp::export]]
void C_setSharedMemoryPrint(bool x)
{
    sharedMemoryPrintEnable = x;
}
// [[Rcpp::export]]
void C_setAltrepPrint(bool x)
{
    altrepPrintEnable = x;
}
// [[Rcpp::export]]
void C_setPackagePrint(bool x)
{
    packagePrintEnable = x;
}

void sharedMemoryPrint(const char *format, ...){
    if (sharedMemoryPrintEnable)
    {
        va_list args;
        va_start(args, format);
        Rvprintf(format, args);
        va_end(args);
    }
}

void altrepPrint(const char *format, ...){
    if (altrepPrintEnable)
    {
        va_list args;
        va_start(args, format);
        Rvprintf(format, args);
        va_end(args);
    }
}

void packagePrint(const char *format, ...){
    if (packagePrintEnable)
    {
        va_list args;
        va_start(args, format);
        Rvprintf(format, args);
        va_end(args);
    }
}


size_t getTypeSize(int type)
{
    size_t elt_size = 0;
    switch (type)
    {
    case INTSXP:
        elt_size = 4;
        break;
    case LGLSXP:
        elt_size = 4;
        break;
    case REALSXP:
        elt_size = 8;
        break;
    case RAWSXP:
        elt_size = 1;
        break;
    case CPLXSXP:
        elt_size = 16;
        break;
        //case STRSXP:
        // unsigned64_t
    default:
        Rf_error("Unknown type: %d\n",type);
    }
    return elt_size;
}

uint64_t getObjectSize(SEXP x)
{
    size_t elt_size = getTypeSize(TYPEOF(x));
    return elt_size * XLENGTH(x);
}

void loadLibrary()
{
    SEXP e;
    Rf_protect(e = Rf_lang2(Rf_install("library"), Rf_mkString(PACKAGE_NAME)));
    R_tryEval(e, R_GlobalEnv, NULL);
    Rf_unprotect(1);
}





/*
 Copy data from source to target without using the
 data pointer of the source.
 */
void copyData(void *target, SEXP source)
{
    int data_type = TYPEOF(source);
    if (data_type == RAWSXP)
    {
        ITERATE_BY_REGION(source, buffer, ind, nbatch, Rbyte, RAW,
        {
            size_t size = sizeof(buffer[0]);
            memcpy((Rbyte *)target + ind * nbatch, buffer, nbatch * size);
        });
        return;
    }
    if (data_type == LGLSXP)
    {
        ITERATE_BY_REGION(source, buffer, ind, nbatch, int, LOGICAL,
        {
            size_t size = sizeof(buffer[0]);
            memcpy((int *)target + ind * nbatch, buffer, nbatch * size);
        });
        return;
    }
    if (data_type == INTSXP)
    {
        ITERATE_BY_REGION(source, buffer, ind, nbatch, int, INTEGER,
        {
            size_t size = sizeof(buffer[0]);
            memcpy((int *)target + ind, buffer, nbatch * size);
        });
        return;
    }

    if (data_type == REALSXP)
    {
        ITERATE_BY_REGION(source, buffer, ind, nbatch, double, REAL,
        {
            size_t size = sizeof(buffer[0]);
            memcpy((double *)target + ind * nbatch, buffer, nbatch * size);
        });
        return;
    }
    if (data_type == CPLXSXP)
    {
        ITERATE_BY_REGION(source, buffer, ind, nbatch, Rcomplex, COMPLEX,
        {
            size_t size = sizeof(buffer[0]);
            memcpy((Rcomplex *)target + ind * nbatch, buffer, nbatch * size);
        });
        return;
    }
}

#ifdef __linux__
#pragma weak __shm_directory
extern "C" const char *__shm_directory(size_t *len);
#include <unistd.h>

// [[Rcpp::export]]
string getSharedMemoryPath()
{

    if (__shm_directory)
    {
        size_t size;
        const char *path = __shm_directory(&size);
        return path;
    }
    else
    {
        string path = "/dev/shm";
        bool pathExist = access(path.c_str(), F_OK) == 0;
        if (pathExist) {
            return path;
        }
        else{
            return "";
        }
    }
}
#else
string getSharedMemoryPath()
{
    return "";
}
#endif






PROTECT_GUARD::PROTECT_GUARD():protect_num(0) {}
PROTECT_GUARD::~PROTECT_GUARD() {
    if (protect_num != 0)
        UNPROTECT(protect_num);
}
SEXP PROTECT_GUARD::protect(SEXP x){
    protect_num++;
    return PROTECT(x);
}

void throwError(const char *format, ...){
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    throw std::runtime_error(buffer);
}
