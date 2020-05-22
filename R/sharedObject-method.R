#' @include utils.R
NULL

#' Test whether the object is a shared object
#'
#' @param x An R object
#' @param recursive Whether to recursively check the element of `x` if `x` has
#' mutiple components(e.g. `list` and S4 object), see details
#' @param ... For generalization purpose only
#' @details
#' When `x` consists of multiple elements and the elements are not a simple object
#' (e.g. a list of lists), the `is.shared` function by default will recursively look
#' into each element and return whether the element's components are
#' shared in a list format. If `recursive = FALSE`, a singe logical value is returned
#' for each element of `x` indicating whether the element contains any shared data.
#'
#' @return TRUE/FALSE indicating whether the object is a shared object.
#' If the object is a list, the return value is a vector of TRUE/FALSE corresponding
#' to each element of the list.
#' @examples
#' x <- share(1:10)
#' is.shared(x)
#' @rdname is.shared
#' @export
setGeneric("is.shared", function(x, recursive = TRUE, ...) {
    standardGeneric("is.shared")
})

#' @rdname is.shared
#' @export
setMethod("is.shared", "ANY", function(x,recursive,...){
    if(isS4(x)){
        return(isS4Shared(x,recursive=recursive,...))
    }
    if (is.altrep(x)) {
        info <- C_getAltData2(x)
        if (is.list(info) &&
            length(info) == length(dataInfoTemplate) &&
            identical(names(dataInfoTemplate), names(dataInfoTemplate))) {
            return(TRUE)
        }
    }
    return(FALSE)
})
isS4Shared <- function(x,recursive,...){
    slots <- slotNames(x)
    res <- vector("list",length(slots))
    for(i in seq_along(slots)){
        res[[i]] <- is.shared(slot(x, slots[i]),recursive=recursive,...)
    }
    names(res) <- slots
    res <- res[unlist(lapply(res, function(x) length(x) != 0))]
    if(!recursive){
        res <- lapply(res, function(x)any(unlist(x)))
    }
    res
}


#' @rdname is.shared
#' @export
setMethod("is.shared", "list", function(x,recursive, ...){
    res <- lapply(x, function(x,...)is.shared(x,...),
                  recursive=recursive,...)
    if(!recursive){
        res <- lapply(res, function(x)any(unlist(x)))
    }
    res
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
setGeneric("getSharedObjectProperty", function(x, property, ...) {
    standardGeneric("getSharedObjectProperty")
})

#' @rdname sharedObjectProperty
#' @export
setMethod("getSharedObjectProperty", signature(x = "ANY", property = "characterOrNULLOrMissing"),
          function(x, property, ...) {
              if (is.shared(x)) {
                  if (missing(property) || is.null(property)) {
                      property <- names(dataInfoTemplate)
                  }
                  info <- C_getAltData2(x)
                  property <-
                      property[property %in% names(dataInfoTemplate)]
                  if (length(property) == 1) {
                      return(info[[property]])
                  } else{
                      return(info[property])
                  }
              }
              NULL
          })

#' @rdname sharedObjectProperty
#' @export
setMethod("getSharedObjectProperty", signature(x = "list", property = "characterOrNULLOrMissing"),
          function(x, property, ...) {
              lapply(x, getSharedObjectProperty, property = property, ...)
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

#' @rdname sharedObjectProperty
#' @export
setMethod("setSharedObjectProperty", signature(
    x = "ANY",
    property = "characterOrNULLOrMissing",
    value = "ANY"
)
, function(x, property, value, ...) {
    if (is.shared(x)) {
        if (missing(property) || is.null(property)) {
            property <- names(dataInfoTemplate)
        }
        if (any(!property %in% names(dataInfoTemplate))) {
            stop("The property '",
                 paste0(property[!property %in% names(dataInfoTemplate)], collapse =
                            ", "),
                 "' is not supported")
        }
        value <- rep_len(value, length(property))
        info <- C_getAltData2(x)
        old_info <- info[property]
        for (i in seq_along(property)) {
            info[[property[i]]] <- as(value[i], class(info[[property[i]]]))
        }
        setAltData2(x, info)
    }
    if(length(property)==1){
        invisible(old_info[[1]])
    }else{
        invisible(old_info)
    }
})
#' @rdname sharedObjectProperty
#' @export
setMethod("setSharedObjectProperty", signature(
    x = "list",
    property = "characterOrNULLOrMissing",
    value = "ANY"
)
, function(x, property, value, ...) {
    for (i in seq_along(x)) {
        setSharedObjectProperty(x[[i]], property, value)
    }
})




#' @examples
#' x = share(1:20)
#'
#' ## Check the default values
#' getSharedObjectProperty(x, NULL)
#' getCopyOnWrite(x)
#' getSharedSubset(x)
#' getSharedCopy(x)
#'
#' ## Set the values
#' setCopyOnWrite(x, FALSE)
#' setSharedSubset(x, FALSE)
#' setSharedCopy(x, TRUE)
#'
#' ## Check the values again
#' getSharedObjectProperty(x, NULL)
#' getCopyOnWrite(x)
#' getSharedSubset(x)
#' getSharedCopy(x)
#' @rdname sharedObjectProperty
#' @export
getCopyOnWrite <- function(x) {
    getSharedObjectProperty(x, "copyOnWrite")
}
#' @rdname sharedObjectProperty
#' @export
getSharedSubset <- function(x) {
    getSharedObjectProperty(x, "sharedSubset")
}
#' @rdname sharedObjectProperty
#' @export
getSharedCopy <- function(x) {
    getSharedObjectProperty(x, "sharedCopy")
}

#' @rdname sharedObjectProperty
#' @export
setCopyOnWrite <- function(x, value) {
    setSharedObjectProperty(x, "copyOnWrite", value)
}
#' @rdname sharedObjectProperty
#' @export
setSharedSubset <- function(x, value) {
    setSharedObjectProperty(x, "sharedSubset", value)
}
#' @rdname sharedObjectProperty
#' @export
setSharedCopy <- function(x, value) {
    setSharedObjectProperty(x, "sharedCopy", value)
}
