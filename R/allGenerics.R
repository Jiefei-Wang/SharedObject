setClassUnion("characterOrNULLOrMissing", c("character", "NULL", "missing"))

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
#' This parameter has no effect on the S4 object.
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
#' For the basic R types, the function supports `raw`,`logical` ,`integer`, `double`.
#' `character` cannot be shared for it has a complicated data structure and closely
#' related to R's cache. For the list object, the elements in the list will be shared.
#' `environment` is not supported for changing an `environment` object may have an
#' unexpected consequence.
#'
#' The function `share` is an S4 generic. The default share method works for
#' most S4 objects. Therefore, there is no need to define a S4 share method
#' for each S4 class unless the S4 class has a special implementation(e.g. on-disk data).
#' The default method will share the object itself and
#' all slots it contains. No error will be given if any of these objects are not
#' sharable and they will be kept unchanged.
#'
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
    standardGeneric("share")
})
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



#' Test whether the object is a shared object
#'
#' @param x An R object
#' @param recursive Whether to recursively check the element of `x` if `x` has
#' mutiple components(e.g. `list` and S4 object), see details
#' @param showAttributes Whether to check the attributes of `x`.
#' @param ... For generalization purpose only
#' @details
#' When `x` consists of multiple elements and the elements are not a simple object
#' (e.g. a list of lists), the `is.shared` function by default will return
#' a singe logical value for each element of `x` indicating whether the element
#' contains any shared data. If `recursive = TRUE`, the function will recursively look
#' into each element and check whether the element's components are
#' shared in a list format.
#'
#' if `showAttributes = TRUE`, the attributes of the object will also be examined. The
#' result is returned in an attribute format. It can be find in the `sharedAttributes`
#' attributes. Note that `showAttributes` has no effect on an S4 object for the attributes
#' of an S4 object are used to store the slots and should not be treated as the attributes
#' of an object.
#'
#' @return TRUE/FALSE indicating whether the object is a shared object.
#' If the object is a list, the return value is a vector of TRUE/FALSE corresponding
#' to each element of the list.
#' @examples
#' x <- share(1:10)
#' is.shared(x)
#' @rdname is.shared
#' @export
setGeneric("is.shared", function(x, ..., recursive = FALSE, showAttributes  = FALSE) {
    standardGeneric("is.shared")
})




#' Get/Set the properties of the shared object.
#'
#' Get/Set the properties of the shared object.
#' The available properties are `dataId`, `length`, `totalSize`,
#' `dataType`, `ownData`, `copyOnWrite`, `sharedSubset`, `sharedCopy`.
#'
#' @param x A shared object
#' @param property A character vector, the name of the property(s),
#' if the argument is missing or the value is `NULL`, it represents all properties.
#' @param ... Not used
#' @return
#' get: The property(s) of a shared object
#' @rdname sharedObjectProperty
#' @export
setGeneric("getSharedObjectProperty", function(x, property = NULL, ...) {
    standardGeneric("getSharedObjectProperty")
})



#' @param value The new value of the property, if the length of value
#' does not match the length of the property, the argument `value` will
#' be repeated to match the length.
#' @rdname sharedObjectProperty
#' @return
#' set: No return value
#' @export
setGeneric("setSharedObjectProperty",
           function(x, property, value, ...) {
               standardGeneric("setSharedObjectProperty")
           })
