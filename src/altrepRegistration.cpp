#include "altrep.h"
#include "utils.h"
#include "numericAltrepOperations.h"
#include "stringAltrepOperations.h"
/*
Register ALTREP class
*/

#define ALT_COMMOM_REGISTRATION(ALT_CLASS, ALT_TYPE)\
	ALT_CLASS =R_make_##ALT_TYPE##_class(class_name, PACKAGE_NAME, dll);\
	/* common ALTREP methods */\
	R_set_altrep_Inspect_method(ALT_CLASS, sharedVector_Inspect);\
	R_set_altrep_Length_method(ALT_CLASS, sharedVector_length);\
	R_set_altrep_Duplicate_method(ALT_CLASS, sharedVector_duplicate);\
	R_set_altrep_Unserialize_method(ALT_CLASS, sharedVector_unserialize);\
	R_set_altrep_Serialized_state_method(ALT_CLASS, sharedVector_serialized_state);\
	/* ALTVEC methods */\
	R_set_altvec_Dataptr_method(ALT_CLASS, sharedVector_dataptr);\
	R_set_altvec_Dataptr_or_null_method(ALT_CLASS, sharedVector_dataptr_or_null);\
	R_set_altvec_Extract_subset_method(ALT_CLASS, sharedVector_subset);


//[[Rcpp::init]]
void init_real_class(DllInfo* dll)
{
	char class_name[] = "shared_real";
	R_altrep_class_t& altrep_class = getAltClass(REALSXP);
	ALT_COMMOM_REGISTRATION(altrep_class, altreal);
}


//[[Rcpp::init]]
void init_integer_class(DllInfo* dll) {
	char class_name[] = "shared_int";
	R_altrep_class_t& altrep_class = getAltClass(INTSXP);
	ALT_COMMOM_REGISTRATION(altrep_class, altinteger);
}



//[[Rcpp::init]]
void init_logical_class(DllInfo* dll) {
	char class_name[] = "shared_logical";
	R_altrep_class_t& altrep_class = getAltClass(LGLSXP);
	ALT_COMMOM_REGISTRATION(altrep_class, altlogical);
}

//[[Rcpp::init]]
void init_raw_class(DllInfo* dll) {
	char class_name[] = "shared_raw";
	R_altrep_class_t& altrep_class = getAltClass(RAWSXP);
	ALT_COMMOM_REGISTRATION(altrep_class, altraw);
}

//[[Rcpp::init]]
void init_complex_class(DllInfo* dll)
{
	char class_name[] = "shared_complex";
	R_altrep_class_t& altrep_class = getAltClass(CPLXSXP);
	ALT_COMMOM_REGISTRATION(altrep_class, altcomplex);
}

//[[Rcpp::init]]
void init_string_class(DllInfo* dll)
{
	char class_name[] = "shared_string";
	R_altrep_class_t& altrep_class = getAltClass(STRSXP);
	altrep_class =R_make_altstring_class(class_name, PACKAGE_NAME, dll);
	R_set_altrep_Inspect_method(altrep_class, sharedString_Inspect);
	R_set_altrep_Length_method(altrep_class, sharedString_length);
	R_set_altrep_Duplicate_method(altrep_class, sharedString_duplicate);
	R_set_altrep_Unserialize_method(altrep_class, sharedString_unserialize);
	R_set_altrep_Serialized_state_method(altrep_class, sharedString_serialized_state);
	R_set_altvec_Dataptr_method(altrep_class, sharedString_dataptr);
	R_set_altvec_Dataptr_or_null_method(altrep_class, sharedString_dataptr_or_null);
	R_set_altstring_Elt_method(altrep_class, sharedString_elt);
	R_set_altstring_Set_elt_method(altrep_class, sharedString_set_elt);
}