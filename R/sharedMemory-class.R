dataInfoPropName=c("DataID","ProcessID","typeID","length","totalSize")
sharedOptions=c("copyOnWrite","sharedSubset","sharedDuplicate")
dataInfoName=c(dataInfoPropName,sharedOptions)
dataInfoTemplate=rep(0.0,length(dataInfoName))
names(dataInfoTemplate)=dataInfoName

sharedMemory=
  setRefClass("sharedMemory",
              fields = c("DataID","ownData","typeName","address"))


sharedMemory$methods(
  initialize = function(x=NULL,options=list()) {
    ownData<<-FALSE
    if(!is.null(x)){
      for(i in seq_along(sharedOptions)){
        name=sharedOptions[i]
        if(is.null(options[[name]])){
          options[[name]]=globalSettings[[name]]
        }
      }
      .self$initializeWithData(x,options)
    }
  },
  finalize=function(){
    if(ownData&&!RM_data$unloaded){
      removeObject(.self$dataInfo["DataID"])
    }
    if(RM_data$unloaded){
      message("Fail to release data: The package has been unloaded.")
    }
  },
  initializeWithData=function(x,options){
    dataInfo=dataInfoTemplate

    if(!is.null(options$DataID)){
      dataInfo["DataID"]=options$DataID
    }else{
      dataInfo=generateKey()
    }

    dataInfo["ProcessID"]=RM$getPID()
    dataInfo["typeID"]=get_type_id(typeof(x))
    dataInfo["length"]=length(x)
    dataInfo["totalSize"]=calculateSharedMemerySize(x)
    for(i in sharedOptions){
      dataInfo[i]=options[[i]]
    }

    C_createSharedMemory(x,dataInfo)

    .self$DataID=dataInfo["DataID"]
    .self$ownData=TRUE
    .self$typeName=typeof(x)
    .self$updateAddress()
  },
  initializeWithPtr=function(ptr,options){

  },
  initializeWithID=function(did){
    dataInfo=getDataInfo_single(did)
    .self$DataID=dataInfo["DataID"]
    .self$ownData=FALSE
    .self$type=get_type_name(dataInfo['typeID'])
    .self$updateAddress()
  }
)




