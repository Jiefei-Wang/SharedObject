
sharedMemory=
  setRefClass("sharedMemory",
              fields = c("NID","PID","DID",
                         "length","type","type_id","total_size",
                         "address","address_NID","address_PID"))

sharedMemory$methods(
  initialize = function(x=NULL) {
    .self$address_NID=0
    .self$address_PID=0
    if(!is.null(x)){
      .self$initializeWithData(x)
    }
  },
  finalize=function(){
    if(!is.na(.self$NID)&&
       .self$NID==RM$getNID()&&
       .self$PID==RM$getPID()){
      removeObject(.self$DID)
    }
  },
  initializeWithData=function(x){
    .self$NID=RM$getNID()
    .self$PID=RM$getPID()
    .self$length=as.double(length(x))
    .self$type=typeof(x)
    .self$type_id=get_type_id(.self$type)
    .self$total_size=.self$length*type_size(.self$type)
    .self$DID=C_createSharedMemory(x,.self$type_id,.self$total_size,.self$PID)
    .self$updateAddress()
  },
  initializeWithID=function(nid,did){
    .self$NID=nid
    .self$DID=as.double(did)
    .self$PID=C_getDataPID(.self$DID)
    dataInfo=C_recoverDataInfo(.self$DID)
    .self$total_size=as.double(dataInfo[1])
    .self$length=as.double(dataInfo[2])
    .self$type_id=as.integer(dataInfo[3])
    .self$type=get_type_name(.self$type_id)
    .self$updateAddress()
  },
  show = function(){
      flds <- getRefClass()$fields()
      cat("Shared memory object\n")
      for (fld in names(flds)){
        if(fld%in%c("address")){
          cat( fld,': ', capture.output(.self[[fld]]), '\n')
        }else{

          cat( fld,': ', .self[[fld]], '\n')
        }
      }
  }
)





