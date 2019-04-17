shareObj<-function(x){
  SM_obj=sharedMemory(x)
  obj=.Call(C_createAltrep,SM_obj$address,SM_obj$type_id,SM_obj$length,SM_obj$total_size)
}
