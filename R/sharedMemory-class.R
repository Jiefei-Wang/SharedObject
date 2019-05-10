sharedOption=c("copyOnWrite","sharedSub")
sharedOptionType=c("logical","logical")
sharedMemory=
  setRefClass("sharedMemory",
              fields = c("PID","DID","ownData",
                         "length","type","type_id","total_size",
                         "address",
                         "copyOnWrite","sharedSub"))

sharedMemory$methods(
  initialize = function(x=NULL,opt=list()) {
    .self$ownData=FALSE
    if(!is.null(x)){
      for(i in seq_along(sharedOption)){
        name=sharedOption[i]
        if(is.null(opt[[name]])){
          .self[[name]]=globalSettings[[name]]
        }else{
          .self[[name]]=as(opt[[name]],sharedOptionType[i])
        }
      }
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
    .self$DID=generateKey()
    .self$length=length(x)
    .self$type=typeof(x)
    .self$type_id=get_type_id(.self$type)
    .self$total_size=getSharedMemerySize(x)
    .self$DID=C_createSharedMemory(x,.self$type_id,.self$total_size,.self$PID,.self$DID,
                                   .self$copyOnWrite,.self$sharedSub)
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
    .self$copyOnWrite=as.logical(dataInfo$copyOnWrite)
    .self$sharedSub=as.logical(dataInfo$sharedSub)
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



getSharedMemerySize<-function(x){
  n=length(x)
  if(typeof(x)=="character"){
    char_size=sum(sapply(x,length))+n
    return(n*8+char_size)
  }else{
    return(n*type_size(typeof(x)))
  }
}

