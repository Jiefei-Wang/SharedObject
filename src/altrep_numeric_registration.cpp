#include "altrep_numeric_common_func.h"
#include "altrep_numeric_registration.h"


#define ALT_NUM_COMMOM_REG(ALT_CLASS,ALT_TYPE,C_TYPE,R_TYPE)\
	ALT_CLASS =R_make_##ALT_TYPE##_class(class_name, PGKNAME, dll);\
	/* override ALTREP methods */\
	R_set_altrep_Inspect_method(ALT_CLASS, sharedObject_Inspect);\
	R_set_altrep_Length_method(ALT_CLASS, sharedObject_length);\
	R_set_altrep_Duplicate_method(ALT_CLASS, sharedObject_dulplicate);\
	/*R_set_altrep_Coerce_method(ALT_CLASS, real_coerce);*/\
	R_set_altrep_Unserialize_method(ALT_CLASS, sharedObject_unserialize);\
	R_set_altrep_Serialized_state_method(ALT_CLASS, sharedObject_serialized_state);\
\
	/* override ALTVEC methods */\
	R_set_altvec_Dataptr_method(ALT_CLASS, sharedObject_dataptr);\
	R_set_altvec_Dataptr_or_null_method(ALT_CLASS, sharedObject_dataptr_or_null);\
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
