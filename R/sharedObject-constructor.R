dataInfoPropName = c("dataId", "processId", "typeId", "length", "totalSize")
sharedOptions = c("copyOnWrite", "sharedSubset", "sharedCopy")
dataInfoName = c(dataInfoPropName, sharedOptions)
dataInfoTemplate = rep(0.0, length(dataInfoName))
names(dataInfoTemplate) = dataInfoName


completeOptions <- function(options) {
  for (i in seq_along(sharedOptions)) {
    name = sharedOptions[i]
    if (is.null(options[[name]])) {
      options[[name]] = globalSettings[[name]]
    }
  }
  options
}

initialSharedMemoryByData <- function(x, options = list()) {
  options = completeOptions(options)
  #Construct dataInfo vector
  dataInfo = dataInfoTemplate
  if (!is.null(options$dataId)) {
    key = options$dataId
  } else{
    key = generateKey()
  }
  key=findUniqueKey(key)
  dataInfo["dataId"] =key
  dataInfo["processId"] = .globals$getProcessID()
  dataInfo["typeId"] = getTypeIDByName(typeof(x))
  dataInfo["length"] = length(x)
  dataInfo["totalSize"] = calculateSharedMemerySize(x)
  for (i in sharedOptions) {
    dataInfo[i] = options[[i]]
  }

  dataPtr = C_createSharedMemory(x, dataInfo)
  list(dataId = key,
       dataPtr = dataPtr,
       typeName = typeof(x))
}

initialSharedMemoryByID = function(dataId) {
  dataPtr = C_readSharedMemory(dataId, ownData = FALSE)
  typeName = getTypeNameByID(C_getTypeID(dataId))
  list(dataId = dataId,
       dataPtr = dataPtr,
       typeName = typeName)
}
