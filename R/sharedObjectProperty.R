getSharedObjectPropertyAny <-  function(x, property, ...) {
    dataInfoTemplate <- getDataInfoTemplate()
    if (isSharedSEXP(x)) {
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
}


#' @rdname sharedObjectProperty
#' @export
setMethod("getSharedObjectProperty", signature(x = "ANY", property = "characterOrNULLOrMissing"),
          getSharedObjectPropertyAny)

#' @rdname sharedObjectProperty
#' @export
setMethod("getSharedObjectProperty", signature(x = "list", property = "characterOrNULLOrMissing"),
          function(x, property, ...) {
              if(is.atomic(x)){
                  getSharedObjectPropertyAny(x, property, ...)
              }else{
                  lapply(x, getSharedObjectProperty, property = property, ...)
              }
          })


setSharedObjectPropertyAny <- function(x, property, value, ...) {
    if (isSharedSEXP(x)) {
        dataInfoTemplate <- getDataInfoTemplate()
        if (missing(property) || is.null(property)) {
            property <- names(dataInfoTemplate)
        }
        if (any(!property %in% names(dataInfoTemplate))) {
            stop("The property '",
                 paste0(property[!property %in% names(dataInfoTemplate)], collapse =
                            ", "),
                 "' is not found")
        }
        value <- rep_len(value, length(property))
        info <- C_getAltData2(x)
        old_info <- info[property]
        for (i in seq_along(property)) {
            info[[property[i]]] <- as(value[i], class(info[[property[i]]]))
        }
        setAltData2(x, info)
        if("ownData" %in% names(dataInfoTemplate)){
            if("ownData" %in% property)
                C_setSharedObjectOwership(x, info[["ownData"]])
        }else{
            stop("ownData slot is not found, please contact author for this bug.")
        }
        
        if(length(property)==1){
            invisible(old_info[[1]])
        }else{
            invisible(old_info)
        }
    }
}

#' @rdname sharedObjectProperty
#' @export
setMethod("setSharedObjectProperty", signature(
    x = "ANY",
    property = "characterOrNULLOrMissing",
    value = "ANY"), setSharedObjectPropertyAny)

#' @rdname sharedObjectProperty
#' @export
setMethod("setSharedObjectProperty", signature(
    x = "list",
    property = "characterOrNULLOrMissing",
    value = "ANY"
)
, function(x, property, value, ...) {
    if(is.atomic(x)){
        setSharedObjectPropertyAny(x, property,value, ...)
    }else{
        for (i in seq_along(x)) {
            setSharedObjectProperty(x[[i]], property, value, ...)
        }
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
