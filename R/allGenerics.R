#############################
## constructor for a shared object
#############################
#' Create a shared object
#'
#' This function will create a shared object for the object `x`.
#' The behavior of the shared object is exactly the same as `x`,
#' but the data of the shared object is allocated in the shared
#' memory space. Therefore, a shared object can be easily exported to the other
#' R workers without duplicating the data, which can reduce the memory consumption
#' and the overhead of data transmission.
#'
#' @param x An R object that will be shared, see details.
#' @param copyOnWrite,sharedSubset,sharedCopy The parameters
#' controlling the behavior of a shared object, see details.
#' @param mustWork Whether to throw an error if `x` is not sharable
#' (e.g. x is a function).
#' This parameter has no effect on the object's attributes and S4 object.
#' @param sharedAttributes Whether to share the attributes of the object `x`
#' (default `TRUE`). Note that attribute `class` and `names` will never be shared.
#' @param minLength The minimum length of a shared object(default `3`).
#' If `length(x)` is smaller than the minimum length, it would not be shared.
#' This parameter can be used to reduce the memory fragmentation.
#' @param ... For generalization purpose.
#'
#' @aliases share,vector-method share,matrix-method
#' share,data.frame-method share,list-method
#'
#' @return A shared object
#' @details
#' The function returns a shared object corresponding to the argument `x` if it
#' is sharable. There should be no different between `x` and the return value except
#' that the latter one is shared. The attributes of `x` will also be shared if possible.
#'
#' **Supported types**
#'
#' For the basic R type, the function supports `raw`, `logical`,`integer`,
#'  `double`, `complex`. `character` can be shared, but sharing a `character`
#' is beneficial only when there are a lot repetitions in the
#' elements of the vector. Due to the complicated structure of the character
#' vector, you are not allowed to set the value of a shared
#' character vector to a value which haven't presented in the vector before.
#' It is recommended to treat a shared character vector as read-only.
#'
#' For the container, the function supports `list`, `pairlist`
#' and `environment`. Note that sharing a container is equivalent
#' to share all elements in the container, the container itself
#' will not be shared.
#'
#' The function `share` is an S4 generic. The default share method works for
#' most S3/S4 objects. Therefore, there is no need to define a S4 share method
#' for each S3/S4 class unless the S3/S4 class has a special implementation
#' (e.g. on-disk data).
#' The default method will share all slots the object contains and the object itself
#' if possible. No error will be given if any of these objects are not
#' sharable and they will be kept unchanged.
#'
#'
#' **Behavior control**
#'
#' The behavior of a shared object can be controlled through three parameters:
#' `copyOnWrite`, `sharedSubset` and `sharedCopy`.
#'
#' `copyOnWrite` determines Whether a shared object needs to be duplicated when the
#' data of the shared object is changed. The default value is `TRUE`,
#' but can be altered by passing `copyOnWrite = FALSE` to the function.
#' This parameter can be used to let workers directly write the result back to
#' a shared object.
#'
#' Please note that the no-copy-on-write feature is not fully supported by R. When
#' `copyOnWrite` is `FALSE`, a shared object might not behaves as one expects.
#' Please refer to the example code to see the exceptions.
#'
#' `sharedSubset` determines whether the subset of a shared object is still a shared object.
#'  The default value is `FALSE`, and can be changed by passing `sharedSubset = TRUE`
#'  to the function
#'
#'  At the time of writing, The shared subset feature will
#'  cause an unnecessary memory duplication in R studio. Therefore, for
#'  the performance consideration, it is recommended to keep the feature
#'  off in R studio.
#'
#' `sharedCopy` determines whether the object is still a shared object after the
#' duplication. Note that it must be used with `copyOnWrite = TRUE`. Otherwise,
#' the shared object will never be duplicated. The default value is `FALSE`.
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
#' ## Copy on write
#' x <- runif(10)
#' so1 <- share(x, copyOnWrite = TRUE)
#' so2 <- so1
#' so2[1] <- 10
#' ## so1 is unchanged since copy-on-write feature is on.
#' so1
#' so2
#'
#' ## No copy on write
#' so1 <- share(x, copyOnWrite = FALSE)
#' so2 <- so1
#' so2[1] <- 10
#' #so1 is changed
#' so1
#' so2
#'
#' ## Flaw of no-copy-on-write
#' ## The following code changes the value of so1,
#' ## highly unexpected! Please use with caution!
#' -so1
#' so1
#' ## The reason is that the minus function tries to
#' ## duplicate so1 object, but the duplication function
#' ## will return so1 itself, so the values in so1 get changed.
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


#' Test whether an object is shared
#'
#' @param x An R object
#' @param depth Whether to recursively check the element of `x`. This parameter
#' only works for container objects(e.g. `list` and `environment`), see details.
#' @param showAttributes Whether to check the attributes of `x`, default `FALSE`.
#' @param ... For generalization purpose only
#' @details
#' When `depth=0`, the `is.shared` function return a single logical value indicating
#' whether `x` is shared or contains any shared objects. When `depth>0` and `x` is a
#' container(e.g. `list`), the function will recursively check each element of `x` and
#' return a list with each elements corresponding to the elements in `x`.
#' The `depth` parameter determines the depth of the checking procedure.
#'
#' if `showAttributes = TRUE`, the attributes of the object will also be checked. The
#' check result is returned as attributes of the return value by appending `Shared` to the
#' end of the original attribute names. Note that `showAttributes` has
#' no effect on an S4 object for the attributes of an S4 object are used to store the
#' slots and should not be treated as the attributes of an object.
#'
#' @return a single logical value or a list.
#' @examples
#' x1 <- share(1:10)
#' is.shared(x1)
#'
#' x2 <- share(list(a=1:10, b = list(d = letters, e = runif(10))))
#' is.shared(x2, depth=0)
#' is.shared(x2, depth=0, showAttributes = TRUE)
#' is.shared(x2, depth=1)
#' is.shared(x2, depth=2)
#' @rdname is.shared
#' @export
setGeneric("is.shared", signature="x",
           function(x, ..., depth = 0, showAttributes  = FALSE) {
               standardGeneric("is.shared")
           })




#' Get/Set the properties of a shared object.
#'
#' @param x A shared object
#' @param literal Whether the parameters in `...` are always treated as characters.
#' @param ... The name of the property(s), it can be either symbols or characters.
#' if the argument is missing, it means getting all properties. See examples.
#' @details
#' For numeric objects, the properties are `dataId`, `length`,
#' `totalSize`, `dataType`, `ownData`, `copyOnWrite`, `sharedSubset`,
#' `sharedCopy`.
#'
#' For character objects, the properties are
#' `length`, `unitSize`,`totalSize`,`dataType`,`uniqueChar`,`copyOnWrite`.
#'
#' Note that only `copyOnWrite`, `sharedSubset` and `sharedCopy` are mutable.
#' The other attributes are read-only.
#' @return
#' get: The property(s) of a shared object
#'
#' set: The old property(s)
#' @examples
#' ## For numeric objects
#' x1 <- share(1:10)
#'
#' ## Get attributes
#' sharedObjectProperties(x1)
#' 
#' ## Get a specific property
#' ## Both symbol and character give the same result
#' sharedObjectProperties(x1, copyOnWrite)
#' sharedObjectProperties(x1, "copyOnWrite")
#' 
#' ## Shortcut for obtaining copy-on-write property
#' getCopyOnWrite(x1)
#' 
#' ## Use a character vector
#' props <- c("copyOnWrite","sharedSubset")
#' sharedObjectProperties(x1, props, literal = FALSE)
#'
#' ## Set attributes
#' sharedObjectProperties(x1, copyOnWrite = FALSE)
#' setCopyOnWrite(x1, FALSE)
#'
#' ## For character objects
#' x2 <- share(letters)
#' sharedObjectProperties(x2)
#' @rdname sharedObjectProperties
#' @export
setGeneric("sharedObjectProperties", function(x, ..., literal = TRUE) {
    standardGeneric("sharedObjectProperties")
})

