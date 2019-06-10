dataInfoPropName=c("dataID","processID","typeID","length","totalSize")
sharedOptions=c("copyOnWrite","sharedSubset","sharedCopy")
dataInfoName=c(dataInfoPropName,sharedOptions)
dataInfoTemplate=rep(0.0,length(dataInfoName))
names(dataInfoTemplate)=dataInfoName

sharedMemory=
  setRefClass("sharedMemory",
              fields = c("dataID","ownData","typeName","address"))


sharedMemory$methods(
  initialize = function(x=NULL,options=list()) {
    .self$ownData=FALSE
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
    if(ownData&&!.globals$isPackageUnloaded()){
      .removeObject(.self$getDataID())
    }
    if(.globals$isPackageUnloaded()){
      message("Fail to release data: The package has been unloaded.")
    }
  },
  initializeWithData=function(x,options){
    dataInfo=dataInfoTemplate

    if(!is.null(options$dataID)){
      dataInfo["dataID"]=options$dataID
    }else{
      dataInfo["dataID"]=generateKey()
    }

    dataInfo["processID"]=.globals$getProcessID()
    dataInfo["typeID"]=getTypeIDByName(typeof(x))
    dataInfo["length"]=length(x)
    dataInfo["totalSize"]=calculateSharedMemerySize(x)
    for(i in sharedOptions){
      dataInfo[i]=options[[i]]
    }

    C_createSharedMemory(x,dataInfo)

    .self$dataID=dataInfo["dataID"]
    .self$ownData=TRUE
    .self$typeName=typeof(x)
    .self$updateAddress()
  },
  initializeWithPtr=function(ptr,options){

  },
  initializeWithID=function(did){
    dataInfo=getDataInfo_single(did)
    .self$dataID=dataInfo["dataID"]
    .self$ownData=FALSE
    .self$typeName=getTypeNameByID(dataInfo['typeID'])
    .self$updateAddress()
  }
)




