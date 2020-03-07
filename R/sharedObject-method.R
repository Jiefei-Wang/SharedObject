#' @include utils.R
NULL

#' Test whether the object is a shared object
#'
#' @param x An R object
#' @return TRUE/FALSE indicating whether the object is a shared object.
#' If the object is a list, the return value is a vector of TRUE/FALSE corresponding
#' to each element of the list.
#' @rdname is.shared
#' @examples
#' x <- share(1:10)
#' is.shared(x)
#' @export
is.shared <- function(x) {
    UseMethod("is.shared", x)
}
#' @rdname is.shared
#' @export
is.shared.default <- function(x) {
    if (is.altrep(x)) {
        info <- C_getAltData2(x)
        if (is.list(info) &&
            length(info) == length(dataInfoTemplate) &&
            identical(names(dataInfoTemplate), names(dataInfoTemplate))) {
            return(TRUE)
        }
    }
    return(FALSE)
}
#' @rdname is.shared
#' @export
is.shared.list <- function(x) {
    lapply(x, is.shared)
}
#' @rdname is.shared
#' @export
is.shared.data.frame <- function(x) {
    lapply(x, is.shared)
}



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
