#define INFO_DATAID 0L 
#define INFO_LENGTH 1L 
#define INFO_TOTALSIZE 2L
#define INFO_DATATYPE 3L
#define INFO_OWNDATA 4L 
#define INFO_COPYONWRITE 5L 
#define INFO_SHAREDSUBSET 6L 
#define INFO_SHAREDCOPY 7L 




#define GET_SLOT(dataInfo, name) VECTOR_ELT(dataInfo, name)
#define SET_SLOT(dataInfo, name, value) SET_VECTOR_ELT(dataInfo, name, value)

#define ALT_EXTPTR(x) R_ExternalPtrAddr(R_altrep_data1(x))
#define ALT_DATAINFO(x) R_altrep_data2(x)

#define GET_ALT_SLOT(x, name) GET_SLOT(ALT_DATAINFO(x),name)
#define SET_ALT_SLOT(x, name, value) SET_SLOT(ALT_DATAINFO(x),name, value)