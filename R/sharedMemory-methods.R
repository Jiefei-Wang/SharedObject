#' @include sharedMemory-class.R


sharedMemory$methods(
  getDataId=function(){
    .self$dataID
  },
  getProcessId=function(){
    .self$processID
  },
  getTypeName=function(){
    .self$typeName
  },
  getOwnData=function(){
    .self$ownData
  },
  getTypeID=function(){
    C_getTypeID(.self$getDataId())
  },
  getLength=function(){
    C_getLength(.self$getDataId())
  },
  getTotalSize=function(){
    C_getTotalSize(.self$getDataId())
  },
  getCopyOnWrite=function(){
    C_getCopyOnWrite(.self$getDataId())
  },
  getSharedSubset=function(){
    C_getSharedSubset(.self$getDataId())
  },
  getSharedDuplicate=function(){
    C_getSharedDuplicate(.self$getDataId())
  },
  updateAddress=function(){
    .self$address=C_readSharedMemory(.self$getDataId())
  },
  possess=function(){
    .self$ownData=TRUE
  },
  surrender=function(){
    .self$ownData=FALSE
  },
  show = function(){
    cat("Shared memory object\n")
    cat("",'Data ID: ', .self$getDataID(), '\n')
    cat("",'Process ID: ', .self$getProcessID(), '\n')
    cat("",'Type ID: ', .self$getTypeID(), '\n')
    cat("",'Type name: ', .self$getTypeName(), '\n')
    cat("",'Length: ', .self$getLength(), '\n')
    cat("",'Total size: ', .self$getTotalSize(), '\n')
    cat("",'Copy on write: ', .self$getCopyOnWrite(), '\n')
    cat("",'Shared subset: ', .self$getSharedSubset(), '\n')
    cat("",'Shared duplicate: ', .self$getSharedDuplicate(), '\n')
    cat("",'Own data: ', .self$getOwnData(), '\n')
    cat("",'Address: ', capture.output(.self[["address"]]), '\n')
  }
)


createSharedMemoryByID<-function(did){
 sm=sharedMemory()
 sm$initializeWithID(did)
 sm
}

