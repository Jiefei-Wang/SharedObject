
sharedObject=
  setRefClass("sharedObject", fields = c("NID","PID","DID", "length","type","type_id","total_size","address","address_sig"))

sharedObject$methods(
  initialize = function(x=NULL) {
    if(!is.null(x)){
      .self$initializeWithData(x)
    }
},
initializeWithData=function(x){
  .self$NID=RM$getNID()
  .self$PID=RM$getPID()
  .self$length=length(x)
  .self$type=typeof(x)
  .self$type_id=get_type_id(.self$type)
  .self$total_size=length(x)*type_size(.self$type)
  .self$DID=.Call(C_createSharedMemory,x,.self$type_id,as.double(.self$total_size),.self$PID)
  .self$loadMemObj()
}
)









test<-function(x){
  devtools::load_all()
  x=runif(10)
  a=sharedObject(x)
  a$subset_oneInd(1)

  processList=.Call(C_getProcessList)
  processList
  .Call(C_getDataList,processList[1])
  .Call(C_testFunc,processList[1])
  .Call(C_clearAll)
  .Call(C_clearObj,0L:3L)
  getSignature(a)

  a=sharedObject(signature="sig:-:type:-:PID_14152_OBJ_ID_1:-:double")
  a$subset_oneInd(1:10)
}



