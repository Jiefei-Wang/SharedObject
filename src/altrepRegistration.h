#include "R_ext/Altrep.h"

extern R_altrep_class_t shared_real_class;
void init_real_class(DllInfo *dll);


extern R_altrep_class_t shared_integer_class;
void init_integer_class(DllInfo* dll);


extern R_altrep_class_t shared_logical_class;
void init_logical_class(DllInfo* dll);

extern R_altrep_class_t shared_raw_class;
void init_raw_class(DllInfo* dll);

extern R_altrep_class_t shared_str_class;
void init_str_class(DllInfo* dll);