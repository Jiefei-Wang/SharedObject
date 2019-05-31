#' Create an R object in the shared memory
#'
#' This function will create an object in the shared memory for the function argument `x`,
#' It can be exported to the other R processes. If exported, the object in all processes will use
#' the same data in the same memory location. Therefore, there is no cost to share the return value of this function.
#' @param x An R object that you want to shared. The data type can be `vector`,
#' `matrix` and `data.frame`. List is not supported.
#' @param ... Additional parameters that will be passed to the shared object, see details.
#'
#'
#' @return A shared object
#' @details
#' When the function argument `x` is an automic object(e.g vector, matrix),
#' the function will create an ALTREP object to replace it.
#' When `x` is a data frame, each column of `x` will be replaced by an ALTREP object.
#'
#' On the R level, the behaviors of an ALTREP object is exactly the same as an automic object(AKA vector),
#' but the data of an ALTREP object is allocated in the shared memory space. Therefore an ALTREP object
#' can be easily exported to the other R processes without dulplicate the memory, which reduces the memory
#' usage and the overhead of data transmission.
#'
#' The behavior of a shared object can be controlled through three parameters:
#' `copyOnWrite`, `sharedSubset` and `sharedDuplicate`.
#'
#' `copyOnWrite` determines Whether a new R object need to be allocated when the
#' shared object is changed. The default value is `TRUE`, but can be altered by passing
#' an argument `copyOnWrite=FALSE` to the function.
#'
#' The no-copy-on-write feature is not fully supported by R. In other words, when
#' `copyOnWrite` is `FALSE`, a shared object might not behaves as user expects.
#' Please refer to the example code to see the exceptions.
#'
#' `sharedSubset`` determines whether the subset of a shared object is still a shared object.
#'  The default value is `TRUE`, and can be changed by passing `sharedSubset=FALSE`
#'  to the function
#'
#' `sharedDuplicate` determines whether the object is still a shared object after a
#' duplication. In current version (R 3.6), an object will be duplicated four times
#' for creating a shared object and lead to a serious performance problem. Therefore,
#' the default value is `FALSE`, user can change it by passing `sharedDuplicate=FALSE`
#' to the function
#'
#'
#' @examples
#' #For vector
#' x=runif(10)
#' so=share(x)
#' x
#' so
#'
#' #For matrix
#' x=matrix(runif(10),2,5)
#' so=share(x)
#' x
#' so
#'
#' #For data frame
#' x=as.data.frame(matrix(runif(10),2,5))
#' so=share(x)
#' x
#' so
#'
#' #export the object
#' library(parallel)
#' cl=makeCluster(1)
#' clusterExport(cl,"so")
#' #check the exported object in the other cluster
#' clusterEvalQ(cl,so)
#'
#' #Copy-on-write
#' x=runif(10)
#' so1=share(x,copyOnWrite=TRUE)
#' so2=so1
#' so2[1]=10
#' #so1 is unchanged since copy-on-write feature is on.
#' so1
#' so2
#'
#' #No-copy-on-write
#' so1=share(x,copyOnWrite=FALSE)
#' so2=so1
#' so2[1]=10
#' #so1 is changed
#' so1
#' so2
#'
#' #Flaw of no-copy-on-write
#' #The following code changes the value of so1, highly unexpected! Please use with caution!
#' -so1
#' so1
#' #The reason is that the minus function trys to dulplicate so1 object,
#' #but the dulplicate function will return so1 itself, so the value in so1 also get changed.
#'
#' @export
setGeneric("share",function(x,...){
  standardGeneric("share")
})


setMethod("share",signature(x="vector"),function(x,...){
  options=as.list(unlist(list(...)))
  sm=sharedMemory(x,options)
  obj=C_createAltrep(sm)
  obj=copyAttribute(x,obj)
  obj
} )

setMethod("share",signature(x="data.frame"),function(x,...){
  options=as.list(unlist(list(...)))
  obj=vector("list",length=length(x))
  for(i in seq_along(obj)){
    sm=share(x[[i]],options=options)
    obj[[i]]=sm
  }
  obj=copyAttribute(x,obj)
  obj

} )
setMethod("share",signature(x="list"),function(x,...){
  stop("Shared list cannot be automatically created. Please create it manually")
} )



#' A fundamental tool to share an R object.
#'
#' retrieve an R object by the data ID, this function is for internal usage only
#' @param did The data id
#' @export
sharedVectorById<-function(did){
  sm=createSharedMemoryByID(did)
  obj=C_createAltrep(sm)
  obj
}

#' @export
getDataID <- function(x) {
  sm=getSharedProperty(x)
  sm$getDataID()
}
#' @export
getTypeName <- function(x) {
  sm=getSharedProperty(x)
  sm$getTypeName()
}
#' @export
getOwnData <- function(x) {
  sm=getSharedProperty(x)
  sm$getOwnData()
}
#' @export
getProcessID <- function(x) {
  sm=getSharedProperty(x)
  sm$getProcessID()
}
#' @export
getTypeID <- function(x) {
  sm=getSharedProperty(x)
  sm$getTypeID()
}
#' @export
getLength <- function(x) {
  sm=getSharedProperty(x)
  sm$getLength()
}
#' @export
getTotalSize <- function(x) {
  sm=getSharedProperty(x)
  sm$getTotalSize()
}
#' @export
getCopyOnWrite <- function(x) {
  sm=getSharedProperty(x)
  sm$getCopyOnWrite()
}
#' @export
getSharedSubset <- function(x) {
  sm=getSharedProperty(x)
  sm$getSharedSubset()
}
#' @export
getSharedDuplicate <- function(x) {
  sm=getSharedProperty(x)
  sm$getSharedDuplicate()
}




