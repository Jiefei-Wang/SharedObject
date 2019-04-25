
sharedMemory=
  setRefClass("sharedMemory",
              fields = c("NID","PID","DID",
                         "length","type","type_id","total_size",
                         "address","needPrint"))

sharedMemory$methods(
  initialize = function(x=NULL) {
    .self$needPrint=TRUE
    if(!is.null(x)){
      .self$initializeWithData(x)
    }
  },
  finalize=function(){
    if(!"sharedObject"%in%loadedNamespaces())
      message("package has been unloaded")
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
    .self$DID=.Call(C_createSharedMemory,x,.self$type_id,.self$total_size,.self$PID)
    .self$loadMemObj()
  },
  initializeWithID=function(id){
    .self$NID=NA
    .self$DID=as.double(id)
    .self$PID=as.double(.Call(C_getDataPID,.self$DID))
    dataInfo=.Call(C_recoverDataInfo,.self$DID)
    .self$total_size=as.double(dataInfo[1])
    .self$type_id=as.integer(dataInfo[2])
    .self$type=get_type_name(.self$type_id)
    .self$length=.self$total_size/type_size(.self$type)
    .self$loadMemObj()
  },
  show = function(){
    if(needPrint){
      flds <- getRefClass()$fields()
      cat("Shared memory object\n")
      for (fld in names(flds)){
        if(fld%in%c("address")){
          cat( fld,': ', capture.output(.self[[fld]]), '\n')
        }else{

          cat( fld,': ', .self[[fld]], '\n')
        }
      }
      #print.default(.self)
    }else{
      cat("Shared memory object\n")
    }
  }
)





