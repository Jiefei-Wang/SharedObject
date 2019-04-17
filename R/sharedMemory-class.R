
sharedMemory=
  setRefClass("sharedMemory", fields = c("NID","PID","DID", "length","type","type_id","total_size","address","address_sig"))

sharedMemory$methods(
  initialize = function(x=NULL) {
    if(!is.null(x)){
      .self$initializeWithData(x)
    }
},
initializeWithData=function(x){
  .self$NID=RM$getNID()
  .self$PID=RM$getPID()
  .self$length=as.double(length(x))
  .self$type=typeof(x)
  .self$type_id=get_type_id(.self$type)
  .self$total_size=.self$length*type_size(.self$type)
  .self$DID=.Call(C_createSharedMemory,x,.self$type_id,.self$total_size,.self$PID)
  .self$loadMemObj()
}
)









test<-function(x){
  devtools::load_all()
  x=runif(10)
  a=sharedMemory(x)
  a$subset_oneInd(1)

  processList=showProcessIDs()
  processList
  showDataIDs(processList[1])
  removeObject(0L:3L)
  removeAllObject()

  devtools::load_all()
  x=runif(10)
  SM_obj=sharedMemory(x)
  obj=.Call(C_createAltrep,SM_obj$address,SM_obj$type_id,SM_obj$length,SM_obj$total_size)
}



