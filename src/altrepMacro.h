#ifndef HEADER_MACRO
#define HEADER_MACRO
//List index in numeric altrep slot 2
#define INFO_DATAID 0L 
#define INFO_LENGTH 1L 
#define INFO_TOTALSIZE 2L
#define INFO_DATATYPE 3L
#define INFO_OWNDATA 4L 
#define INFO_COPYONWRITE 5L 
#define INFO_SHAREDSUBSET 6L 
#define INFO_SHAREDCOPY 7L 
#define INFO_SLOT_NUM 8L

//List index in string altrep slot 1
#define STR_DATA_STRVEC 0L
#define STR_DATA_INDEX 1L
#define STR_DATA_CHARSET 2L

//List index in string altrep slot 2
#define STR_INFO_LENGTH 0L  
#define STR_INFO_UNITSIZE 1L 
#define STR_INFO_TOTALSIZE 2L
#define STR_INFO_DATATYPE 3L
#define STR_INFO_UNIQUECHAR 4L
#define STR_INFO_COPYONWRITE 5L 
#define STR_INFO_SLOT_NUM 6L



#define GET_SLOT(dataInfo, name) VECTOR_ELT(dataInfo, name)
#define SET_SLOT(dataInfo, name, value) SET_VECTOR_ELT(dataInfo, name, value)

#define STR_ALT_DATA(x) R_altrep_data1(x)
#define ALT_EXTPTR(x) R_ExternalPtrAddr(R_altrep_data1(x))
#define ALT_DATAINFO(x) R_altrep_data2(x)

#define GET_ALT_SLOT(x, name) GET_SLOT(ALT_DATAINFO(x),name)
#define SET_ALT_SLOT(x, name, value) SET_SLOT(ALT_DATAINFO(x),name, value)

#endif