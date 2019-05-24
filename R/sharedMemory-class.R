dataInfoName=c("DID","PID","type_id","length","total_size","copyOnWrite","sharedSub","sharedDuplicate")
sharedOption=c("copyOnWrite","sharedSub","sharedDuplicate")
dataInfoTemplate=rep(0.0,length(dataInfoName))
names(dataInfoTemplate)=dataInfoName

sharedMemory=
  setRefClass("sharedMemory",
              fields = c("ownData","type","address","dataInfo"))

sharedMemory$methods(
  initialize = function(x=NULL,opt=list()) {
    .self$dataInfo=dataInfoTemplate
    .self$ownData=FALSE
    if(!is.null(x)){
      for(i in seq_along(sharedOption)){
        name=sharedOption[i]
        .self$dataInfo[name]=ifelse(is.null(opt[[name]]),
                                    globalSettings[[name]],
                                    as.numeric(opt[[name]])
        )
      }
      .self$initializeWithData(x)
    }
  },
  finalize=function(){
    if(ownData&&!RM_data$unloaded){
      removeObject(.self$dataInfo["DID"])
    }
    if(RM_data$unloaded){
      message("Fail to release data: The package has been unloaded.")
    }
  },
  initializeWithData=function(x){
    DI=.self$dataInfo
    DI["PID"]=RM$getPID()
    DI["DID"]=generateKey()
    DI["length"]=length(x)
    DI["type_id"]=get_type_id(typeof(x))
    DI["total_size"]=getSharedMemerySize(x)
    C_createSharedMemory(x,DI)

    .self$ownData=TRUE
    .self$type=typeof(x)
    .self$dataInfo=DI
    .self$updateAddress()
  },
  initializeWithID=function(did){
    .self$dataInfo=getDataInfo_single(did)
    .self$ownData=FALSE
    .self$type=get_type_name(.self$dataInfo['type_id'])
    .self$updateAddress()
  },
  show = function(){
      cat("Shared memory object\n")
      for(i in dataInfoName){
        if(i%in%sharedOption){
          did=.self$dataInfo["DID"]
          info=switch(i,
                     copyOnWrite=C_getCopyOnWrite(did),
                     sharedSub=C_getSharedSub(did),
                     sharedDuplicate=C_getSharedDuplicate(did)
          )
        }else{
          info=.self$dataInfo[i]
        }
        cat("",i,': ', info, '\n')
      }
    cat("",'Type: ', .self$type, '\n')
    cat("",'Own data: ', .self$ownData, '\n')
      cat("",'Address: ', capture.output(.self[["address"]]), '\n')
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

