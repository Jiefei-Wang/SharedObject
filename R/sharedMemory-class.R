
sharedMemory=
  setRefClass("sharedMemory",
              fields = c("PID","DID","ownData",
                         "length","type","type_id","total_size",
                         "address"))

sharedMemory$methods(
  initialize = function(x=NULL) {
    .self$ownData=FALSE
    if(!is.null(x)){
      .self$initializeWithData(x)
    }
  },
  finalize=function(){
    if(ownData&&!RM_data$unloaded){
      removeObject(.self$DID)
    }
    if(RM_data$unloaded){
      message("Fail to release data: The package has been unloaded.")
    }
  },
  initializeWithData=function(x){
    .self$ownData=TRUE
    .self$PID=RM$getPID()
    .self$DID=rdunif(1,min=0,max=2^53)
    .self$length=length(x)
    .self$type=typeof(x)
    .self$type_id=get_type_id(.self$type)
    .self$total_size=.self$length*type_size(.self$type)
    .self$DID=C_createSharedMemory(x,.self$type_id,.self$total_size,.self$PID,.self$DID)
    .self$updateAddress()
  },
  initializeWithID=function(did){
    .self$ownData=FALSE
    .self$DID=as.double(did)
    dataInfo=getDataInfo(.self$DID)
    .self$PID=dataInfo$PID
    .self$total_size=dataInfo$size
    .self$length=dataInfo$length
    .self$type_id=dataInfo$type
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





