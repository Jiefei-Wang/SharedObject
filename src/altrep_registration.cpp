#include "altrep_numeric_common_func.h"
#include "altrep_registration.h"


#define ALT_NUM_COMMOM_REG(ALT_CLASS,ALT_TYPE,C_TYPE,R_TYPE)\
	ALT_CLASS =R_make_##ALT_TYPE##_class(class_name, PGKNAME, dll);\
	/* override ALTREP methods */\
	R_set_altrep_Inspect_method(ALT_CLASS, sharedVector_Inspect);\
	R_set_altrep_Length_method(ALT_CLASS, sharedVector_length);\
	R_set_altrep_Duplicate_method(ALT_CLASS, sharedVector_duplicate);\
	/*R_set_altrep_Coerce_method(ALT_CLASS, real_coerce);*/\
	R_set_altrep_Unserialize_method(ALT_CLASS, sharedVector_unserialize);\
	R_set_altrep_Serialized_state_method(ALT_CLASS, sharedVector_serialized_state);\
\
	/* override ALTVEC methods */\
	R_set_altvec_Dataptr_method(ALT_CLASS, sharedVector_dataptr);\
	R_set_altvec_Dataptr_or_null_method(ALT_CLASS, sharedVector_dataptr_or_null);\
	R_set_altvec_Extract_subset_method(ALT_CLASS, numeric_subset<R_TYPE, C_TYPE>);\
/* override ALTREAL methods */\
R_set_##ALT_TYPE##_Elt_method(ALT_CLASS, numeric_Elt<C_TYPE>);\
R_set_##ALT_TYPE##_Get_region_method(ALT_CLASS, numeric_region<C_TYPE>);



R_altrep_class_t shared_real_class;
#define C_TYPE double
#define R_TYPE REALSXP
void init_real_class(DllInfo* dll)
{
	char class_name[] = "shared_real";
	ALT_NUM_COMMOM_REG(shared_real_class, altreal, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE


R_altrep_class_t shared_integer_class;
#define C_TYPE int
#define R_TYPE INTSXP
void init_integer_class(DllInfo* dll) {
	char class_name[] = "shared_int";
	ALT_NUM_COMMOM_REG(shared_integer_class,altinteger, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE



R_altrep_class_t shared_logical_class;
#define C_TYPE int
#define R_TYPE LGLSXP
void init_logical_class(DllInfo* dll) {
	char class_name[] = "shared_logical";
	ALT_NUM_COMMOM_REG(shared_logical_class, altlogical, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE


R_altrep_class_t shared_raw_class;
#define C_TYPE Rbyte
#define R_TYPE RAWSXP
void init_raw_class(DllInfo* dll) {
	char class_name[] = "shared_raw";
	ALT_NUM_COMMOM_REG(shared_raw_class, altraw, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE

/*
typedef SEXP (*R_altstring_Elt_method_t)(SEXP, R_xlen_t);
typedef void (*R_altstring_Set_elt_method_t)(SEXP, R_xlen_t, SEXP);
typedef int (*R_altstring_Is_sorted_method_t)(SEXP);
typedef int (*R_altstring_No_NA_method_t)(SEXP);
*/
SEXP altstring_elt(SEXP x, R_xlen_t i);
void* altstring_dataptr(SEXP x, Rboolean writable);
const void* altstring_dataptr_or_null(SEXP x);
R_altrep_class_t shared_str_class;
void init_str_class(DllInfo* dll) {
	shared_str_class = R_make_altstring_class("shared_str", PGKNAME, dll); 
	/* override ALTREP methods */
	R_set_altrep_Inspect_method(shared_str_class, sharedVector_Inspect);
	R_set_altrep_Length_method(shared_str_class, sharedVector_length);
	R_set_altrep_Duplicate_method(shared_str_class, sharedVector_duplicate);
	/*R_set_altrep_Coerce_method(ALT_CLASS, real_coerce);*/
	R_set_altrep_Unserialize_method(shared_str_class, sharedVector_unserialize);
	R_set_altrep_Serialized_state_method(shared_str_class, sharedVector_serialized_state);

	/* override ALTVEC methods */
	R_set_altvec_Dataptr_method(shared_str_class, altstring_dataptr);
	R_set_altvec_Dataptr_or_null_method(shared_str_class, altstring_dataptr_or_null);
	//R_set_altvec_Extract_subset_method(shared_str_class, numeric_subset<R_TYPE, C_TYPE>);
	R_set_altstring_Elt_method(shared_str_class, altstring_elt);
}


SEXP altstring_elt(SEXP x, R_xlen_t i) {
	DEBUG(messageHandle("string subset:%llu\n", i));
	char* source = (char*)SV_PTR(x);
	ULLong* offset = (ULLong*)source;
	ULLong curOffset = *(offset + i);
	char* data = source + curOffset;
	return(Rf_mkChar(data));
}

void* altstring_dataptr(SEXP x, Rboolean writable) {
	DEBUG(messageHandle("accessing string pointer\n"));

	ULLong n=SV_LENGTH(x);
	SEXP* res = (SEXP*)malloc(n * sizeof(SEXP*));
	SEXP char_res = Rf_mkChar("NA");
	for (ULLong i = 0; i < n; i++) {
		res[i] = char_res;
	}
	return(res);
}

const void* altstring_dataptr_or_null(SEXP x) {
	DEBUG(messageHandle("accessing string pointer or null\n"));
	return(NULL);
}