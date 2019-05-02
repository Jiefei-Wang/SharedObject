#' @include sharedMemory-class.R



sharedMemory$methods(
  updateAddress=function(){
    .self$address=C_readSharedMemory(.self$DID)
  }
)
sharedMemory$methods(
  possess=function(){
    .self$ownData=TRUE
  },
  surrender=function(){
    .self$ownData=FALSE
  }
)

createSharedMemoryByID<-function(did){
 sm=sharedMemory()
 sm$initializeWithID(did)
 sm
}




