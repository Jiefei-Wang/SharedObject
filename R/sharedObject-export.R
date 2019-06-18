#' Create an R object in the shared memory
#'
#' This function will create an object in the shared memory for the function argument `x`
#' and return a shared object. There is no duplication of the shared object when it is
#' exported to the other processes. All the shared objects will use the data located in
#' the same shared memory space.
#'
#' @param x An R object that you want to shared. The supported data types are
#' `raw`, `logical`, `integer` and `real`. The data structure can be `vector`,
#' `matrix` and `data.frame`. List is not supported but can be created manually.
#' @param ... Additional parameters that will be passed to the shared object, see details.
#'
#' @aliases share,vector-method share,matrix-method share,data.frame-method share,list-method
#'
#' @return A shared object
#' @details
#' When the function argument `x` is an atomic object(e.g vector, matrix),
#' the function will create an ALTREP object to replace it.
#' When `x` is a data frame, each column of `x` will be replaced by an ALTREP object.
#'
#' In the R level, the behaviors of an ALTREP object is exactly the same as an atomic object
#' but the data of an ALTREP object is allocated in the shared memory space. Therefore an
#' ALTREP object can be easily exported to the other R processes without dulplicating the
#' data, which reduces the memory usage and the overhead of data transmission.
#'
#' The behavior of a shared object can be controlled through three parameters:
#' `copyOnWrite`, `sharedSubset` and `sharedCopy`.
#'
#' `copyOnWrite` determines Whether a new R object need to be allocated when the
#' shared object is changed. The default value is `TRUE`, but can be altered by passing
#' an argument `copyOnWrite=FALSE` to the function.
#'
#' Please note that the no-copy-on-write feature is not fully supported by R. When
#' `copyOnWrite` is `FALSE`, a shared object might not behaves as user expects.
#' Please refer to the example code to see the exceptions.
#'
#' `sharedSubset`` determines whether the subset of a shared object is still a shared object.
#'  The default value is `TRUE`, and can be changed by passing `sharedSubset=FALSE`
#'  to the function
#'
#'  At the time this documentation is being written, The shared subset feature will
#'  cause an unnecessary memory duplication in R studio. Therefore, for the performance
#'  consideration, it is better to turn the feature off when using R studio.
#'
#' `sharedCopy` determines whether the object is still a shared object after a
#' duplication. If `copyOnWrite` is `FALSE`, this feature is off since the duplication
#' cannot be triggered. In current version (R 3.6), an object will be duplicated four times
#' for creating a shared object and lead to a serious performance problem. Therefore,
#' the default value is `FALSE`, user can alter it by passing `sharedCopy=FALSE`
#' to the function.
#'alter
#'
#' @examples
#' ## For vector
#' x=runif(10)
#' so=share(x)
#' x
#' so
#'
#' ## For matrix
#' x=matrix(runif(10),2,5)
#' so=share(x)
#' x
#' so
#'
#' ## For data frame
#' x=as.data.frame(matrix(runif(10),2,5))
#' so=share(x)
#' x
#' so
#'
#' ## export the object
#' library(parallel)
#' cl=makeCluster(1)
#' clusterExport(cl,"so")
#' ## check the exported object in the other process
#' clusterEvalQ(cl,so)
#'
#' ## close the connection
#' stopCluster(cl)
#'
#' ## Copy-on-write
#' ## This is the default setting
#' x=runif(10)
#' so1=share(x,copyOnWrite=TRUE)
#' so2=so1
#' so2[1]=10
#' ## so1 is unchanged since copy-on-write feature is on.
#' so1
#' so2
#'
#' ## No-copy-on-write
#' so1=share(x,copyOnWrite=FALSE)
#' so2=so1
#' so2[1]=10
#' #so1 is changed
#' so1
#' so2
#'
#' ## Flaw of no-copy-on-write
#' ## The following code changes the value of so1, highly unexpected! Please use with caution!
#' -so1
#' so1
#' ## The reason is that the minus function trys to dulplicate so1 object,
#' ## but the dulplicate function will return so1 itself, so the value in so1 also get changed.
#'
#' @export
setGeneric("share",function(x,...){
  standardGeneric("share")
})

shareAtomic<-function(x,...){
  options=as.list(unlist(list(...)))
  dataReferenceInfo=initialSharedMemoryByData(x,options)
  obj=C_createAltrep(dataReferenceInfo)
  obj=copyAttribute(x,obj)
  obj
}

setMethod("share",signature(x="vector"),shareAtomic)
setMethod("share",signature(x="matrix"),shareAtomic)

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

## Used by unserialize function
sharedVectorById<-function(dataId){
  dataReferenceInfo=initialSharedMemoryByID(dataId)
  obj=C_createAltrep(dataReferenceInfo)
  obj
}





