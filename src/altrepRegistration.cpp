#include "altrep.h"
#include "altrepNumericCommonFunc.h"
#include "tools.h"

R_altrep_class_t shared_real_class;
R_altrep_class_t shared_integer_class;
R_altrep_class_t shared_logical_class;
R_altrep_class_t shared_raw_class;
R_altrep_class_t shared_complex_class;

R_altrep_class_t getAltClass(int type) {
	switch (type) {
	case REALSXP:
		return shared_real_class;
	case INTSXP:
		return shared_integer_class;
	case LGLSXP:
		return shared_logical_class;
	case RAWSXP:
		return shared_raw_class;
	case CPLXSXP:
		return shared_complex_class;
	case STRSXP:
		//return shared_str_class;
	default: Rf_error("Type of %d is not supported yet", type);
	}
	// Just for suppressing the annoying warning, it should never be excuted
	return shared_real_class;
}


/*
Register ALTREP class
*/

#define ALT_COMMOM_REGISTRATION(ALT_CLASS,ALT_TYPE,C_TYPE,R_TYPE)\
	ALT_CLASS =R_make_##ALT_TYPE##_class(class_name, PACKAGE_NAME, dll);\
	/* common ALTREP methods */\
	R_set_altrep_Inspect_method(ALT_CLASS, sharedVector_Inspect);\
	R_set_altrep_Length_method(ALT_CLASS, sharedVector_length);\
	R_set_altrep_Duplicate_method(ALT_CLASS, sharedVector_duplicate);\
	/*R_set_altrep_Coerce_method(ALT_CLASS, real_coerce);*/\
	R_set_altrep_Unserialize_method(ALT_CLASS, sharedVector_unserialize);\
	R_set_altrep_Serialized_state_method(ALT_CLASS, sharedVector_serialized_state);\
	/* ALTVEC methods */\
	R_set_altvec_Dataptr_method(ALT_CLASS, sharedVector_dataptr);\
	R_set_altvec_Dataptr_or_null_method(ALT_CLASS, sharedVector_dataptr_or_null);\
	R_set_altvec_Extract_subset_method(ALT_CLASS, numeric_subset<R_TYPE, C_TYPE>);\
	R_set_##ALT_TYPE##_Elt_method(ALT_CLASS, numeric_Elt<C_TYPE>);\
	R_set_##ALT_TYPE##_Get_region_method(ALT_CLASS, numeric_region<C_TYPE>);




#define C_TYPE double
#define R_TYPE REALSXP
//[[Rcpp::init]]
void init_real_class(DllInfo* dll)
{
	char class_name[] = "shared_real";
	ALT_COMMOM_REGISTRATION(shared_real_class, altreal, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE


#define C_TYPE int
#define R_TYPE INTSXP
//[[Rcpp::init]]
void init_integer_class(DllInfo* dll) {
	char class_name[] = "shared_int";
	ALT_COMMOM_REGISTRATION(shared_integer_class, altinteger, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE



#define C_TYPE int
#define R_TYPE LGLSXP
//[[Rcpp::init]]
void init_logical_class(DllInfo* dll) {
	char class_name[] = "shared_logical";
	ALT_COMMOM_REGISTRATION(shared_logical_class, altlogical, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE


#define C_TYPE Rbyte
#define R_TYPE RAWSXP
//[[Rcpp::init]]
void init_raw_class(DllInfo* dll) {
	char class_name[] = "shared_raw";
	ALT_COMMOM_REGISTRATION(shared_raw_class, altraw, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE

#define C_TYPE Rcomplex
#define R_TYPE CPLXSXP
//[[Rcpp::init]]
void init_complex_class(DllInfo* dll)
{
	char class_name[] = "shared_complex";
	ALT_COMMOM_REGISTRATION(shared_complex_class, altcomplex, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE