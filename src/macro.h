
#define SLOT_DATAID(dataInfo) VECTOR_ELT(dataInfo, 0L)
#define SLOT_LENGTH(dataInfo) VECTOR_ELT(dataInfo, 1L)
#define SLOT_TOTALSIZE(dataInfo) VECTOR_ELT(dataInfo, 2L)
#define SLOT_OWNDATA(dataInfo) VECTOR_ELT(dataInfo, 3L)
#define SLOT_COPYONWRITE(dataInfo) VECTOR_ELT(dataInfo, 4L)
#define SLOT_SHAREDSUBSET(dataInfo) VECTOR_ELT(dataInfo, 5L)
#define SLOT_SHAREDCOPY(dataInfo) VECTOR_ELT(dataInfo, 6L)


#define SET_SLOT_DATAID(dataInfo,x) SET_VECTOR_ELT(dataInfo, 0L, x)
#define SET_SLOT_LENGTH(dataInfo,x) SET_VECTOR_ELT(dataInfo, 1L, x)
#define SET_SLOT_TOTALSIZE(dataInfo,x) SET_VECTOR_ELT(dataInfo, 2L, x)
#define SET_SLOT_OWNDATA(dataInfo,x) SET_VECTOR_ELT(dataInfo, 3L, x)
#define SET_SLOT_COPYONWRITE(dataInfo,x) SET_VECTOR_ELT(dataInfo, 4L, x)
#define SET_SLOT_SHAREDSUBSET(dataInfo,x) SET_VECTOR_ELT(dataInfo, 5L, x)
#define SET_SLOT_SHAREDCOPY(dataInfo,x) SET_VECTOR_ELT(dataInfo, 6L, x)