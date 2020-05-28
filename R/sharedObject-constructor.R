#' @include sharedObject-internal.R
NULL

dataInfoPropNames = c("dataId", "length", "totalSize", "dataType", "ownData")
sharedOptions = c("copyOnWrite", "sharedSubset", "sharedCopy")
dataInfoNames = c(dataInfoPropNames, sharedOptions)
dataInfoTemplate = as.list(rep(0, length(dataInfoNames)))
names(dataInfoTemplate) = dataInfoNames

#############################
## constructor for a shared object
#############################
#' Create a shared object
#'
#' This function will create a shared object in the shared memory for the function
#' argument `x` and return a shared object if the object can be shared.
#' There is no duplication of the shared object when a shared object is
#' exported to the other processes.
#' `tryShare` is equivalent to `share` with the argument `mustWork = FALSE`.
#'
#' @param x An R object that you want to shared. The supported data types are
#' `raw`, `logical`, `integer` and `real`. `character` cannot be shared.
#' @param copyOnWrite,sharedSubset,sharedCopy The parameters controlling the behavior of the shared object,
#' see details.
#' @param mustWork Whether to throw an error if `x` is not a sharable object(e.g. Character).
#' @param autoS4Conversion Whether to use the automatic conversion method for
#' an S4 object when there is no `share` method defined
#' for the signiture `class(x)`, see details.
#' @param ... Additional parameters that can be passed to the shared object, see below.
#'
#' @aliases share,vector-method share,matrix-method
#' share,data.frame-method share,list-method
#'
#' @return A shared object
#' @details
#'
#' The function returns a shared object corresponding to the argument `x` if it
#' is sharable. An error will be given if the argument `x` is not sharable. specifying
#' `mustWork = FALSE` will suppress the error. This feature is useful when sharing a list
#' object that consists of both sharable and non-sharable objects. Alternatively,
#' the `tryShare` function can be used and it is equivalent to the function
#' `share` with the argument `mustWork = FALSE`.
#'
#' **Supported types**
#'
#' The function supports sharing `raw`,`logical` ,`integer`, `double` data types.
#' When the argument `x` is an atomic object(e.g vector, matrix),
#' the function will create an ALTREP object to replace it.
#' When `x` is a list, each column of `x` will be replaced by an ALTREP object.
#' The function `share` is an S4 generic, Package developers can provide their own
#' shared object by defining an S4 `share` function.
#'
#' **Behavior control**
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
#' an argument `copyOnWrite = FALSE` to the function.
#'
#' Please note that the no-copy-on-write feature is not fully supported by R. When
#' `copyOnWrite` is `FALSE`, a shared object might not behaves as user expects.
#' Please refer to the example code to see the exceptions.
#'
#' `sharedSubset` determines whether the subset of a shared object is still a shared object.
#'  The default value is `TRUE`, and can be changed by passing `sharedSubset = FALSE`
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
#' the default value is `FALSE`, user can alter it by passing `sharedCopy = FALSE`
#' to the function.
#'
#'
#' **Default S4 share function**
#' If the argument `x` is an S4 object and its class is not supported
#' by the `share` function. A default `share` function can be used by specifying
#' `autoS4Conversion = TRUE`. The default method will loop over and share
#' all slots for the object `x`. No error will be given if a slot
#' of `x` is not sharable. Please be aware that this method may have an
#' unexpected consequence(e.g. a C pointer is linked with a slot data of `x`).
#' It only serves as a backup method. Most bioconductor fundamental classes are supported in
#' `SharedObjectUltility`, if you find the class of `x` is not supported by
#' `SharedObjectUltility` and is important for Bioconductor users, please feel free to ask
#' for the new feature at
#' \href{https://github.com/Jiefei-Wang/SharedObjectUtility/issues}{GitHub}.
#'
#' @examples
#' ## For vector
#' x <- runif(10)
#' so <- share(x)
#' x
#' so
#'
#' ## For matrix
#' x <- matrix(runif(10), 2, 5)
#' so <- share(x)
#' x
#' so
#'
#' ## For data frame
#' x <- as.data.frame(matrix(runif(10), 2, 5))
#' so <- share(x)
#' x
#' so
#'
#' ## export the object
#' library(parallel)
#' cl <- makeCluster(1)
#' clusterExport(cl, "so")
#' ## check the exported object in the other process
#' clusterEvalQ(cl, so)
#'
#' ## close the connection
#' stopCluster(cl)
#'
#' ## Copy-on-write
#' ## This is the default setting
#' x <- runif(10)
#' so1 <- share(x, copyOnWrite = TRUE)
#' so2 <- so1
#' so2[1] <- 10
#' ## so1 is unchanged since copy-on-write feature is on.
#' so1
#' so2
#'
#' ## No-copy-on-write
#' so1 <- share(x, copyOnWrite = FALSE)
#' so2 <- so1
#' so2[1] <- 10
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
#' @rdname share
#' @export
setGeneric("share", signature="x", function(x,
                             ...) {
    args <- list(x=x, ...)
    ind <- which(!names(globalSettings) %in% names(args))
    if(length(ind)>0){
        defaultArgs <- lapply(names(globalSettings)[ind],
                              function(x)getSharedObjectOptions(x))
        names(defaultArgs) <- names(globalSettings)[ind]
        args <- c(args,defaultArgs)
        do.call("share",args)
    }else{
        standardGeneric("share")
    }
})
#' @rdname share
#' @export
setMethod("share", signature(x = "ANY"), promptErrorANY)
#' @rdname share
#' @export
setMethod("share", signature(x = "character"), promptErrorChar)
#' @rdname share
#' @export
setMethod("share", signature(x = "vector"), shareAtomic)
#' @rdname share
#' @export
setMethod("share", signature(x = "matrix"), shareAtomic)
#' @rdname share
#' @export
setMethod("share", signature(x = "list"), function(x,...) {
    result <- vector("list", length = length(x))
    for (i in seq_along(result)) {
        result[[i]] <- share(x[[i]], ...)
    }
    copyAttribute(result, x)
    result
})

#' @rdname share
#' @export
tryShare <- function(x, ...) {
    options <- list(x = x, ...)
    options["mustWork"] <- FALSE
    do.call(share, options)
}

#' Unshare a shared object
#'
#' Unshare a shared object. There will be no effect if the
#' object is not shared.
#'
#' @param x a shared object, or an object that contains a shared object.
#' @return An unshared object
#' @aliases unshare,ANY-method unshare,vector-method unshare,list-method
#' @examples
#' x1 <- share(1:10)
#' x2 <- unshare(x1)
#' is.shared(x1)
#' is.shared(x2)
#' @export
setGeneric("unshare", signature="x", function(x){
    standardGeneric("unshare")
})
unshareAttributes<-function(x){
    attrs <- attributes(x)
    if(!is.null(attrs)&&any(unlist(is.shared(attrs)))){
        unSharedAttrs <- unshare(attrs)
        if(!C_isSameObject(attrs,unSharedAttrs)){
            attributes(x) <- unSharedAttrs
        }
    }
    x
}
#' @export
setMethod("unshare", signature(x = "ANY"), function(x){
    x <- unshareAttributes(x)
    if(isS4(x)){
        slots <- slotNames(x)
        for(i in slots){
            curSlot <- slot(x,i)
            unSharedSlot <- unshare(curSlot)
            if(!C_isSameObject(curSlot,unSharedSlot)){
                slot(x,i,check =FALSE) <- unSharedSlot
            }
        }
        return(x)
    }

    if(is.environment(x)){
        for(i in names(x)){
            x[[i]] <- unshare(x)
        }
    }
    return(x)
})
#' @export
setMethod("unshare", signature(x = "vector"), function(x){
    if(!is.shared(x)){
        return(x)
    }
    if(!typeof(x)%in%c("logical", "integer", "double", "character", "raw")){
        return(x)
    }
    y <- vector(mode = typeof(x),length = length(x))
    attributes(y) <- unshare(attributes(x))
    ## This function directly operates on the memory
    C_memcpy(x, y,length(x) * getTypeSize(typeof(x)))
    y
})
#' @export
setMethod("unshare", signature(x = "list"), function(x){
    x <- unshareAttributes(x)

    for(i in seq_along(x)){
        curElt <- x[[i]]
        unSharedElt <- unshare(curElt)
        if(!C_isSameObject(curElt,unSharedElt)){
            x[[i]] <- unSharedElt
        }
    }
    x
})








