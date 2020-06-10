## we share the object based on the data structure, not the
## `class` attribute, so the S4 dispatch system does not work
## as we expected, we create our own dispatch rule:
## 1. If the object is an S4 object, dispatch to shareS4 method
## 2. If the internal structure of the object is an atomic object,
## dispatch to shareAtomic function
## 3. If the internal is a list, dispatch to shareList
## 4. Otherwise, for any unknow object, throw an error


shareANY <- function(x,...){
    if(isS4(x)){
        return(shareS4(x,...))
    }else if(isSharableAtomic(x)){
        return(shareAtomic(x,...))
    }else if(isSEXPList(x)){
        return(shareList(x,...))
    }
    promptError(x,...)
}

## x must be an atomic object
shareAtomic <- function(x,...) {
    options <- completeOptions(...)
    ## Construct dataInfo
    ## The dataId and dataType will be filled out at C level
    dataInfo = getDataInfoTemplate(
        dataId = 1.0,
        length = length(x),
        totalSize = calculateSharedMemorySize(x),
        dataType = 0,
        ownData = TRUE
    )
    dataInfo[sharedAtomicOptions] <- options[sharedAtomicOptions]
    if(dataInfo[["totalSize"]] == 0){
        dataInfo[["totalSize"]] <- 1
    }
    result <- C_createSharedMemory(x, dataInfo)

    ## Share the attributes
    oldAttrs <- attributes(x)
    if(!is.null(oldAttrs)){
        oldCopyOnWrite <- setCopyOnWrite(result, FALSE)
        oldOwnData <- setSharedObjectProperty(result, "ownData", FALSE)
        attrs <- tryShare(oldAttrs,...)
        ## we must set the attributes no matter if attributes can be shared or not
        ## for result does not have any attributes.
        attributes(result) <- attrs
        setSharedObjectProperty(result, "ownData", oldOwnData)
        setCopyOnWrite(result, oldCopyOnWrite)
    }
    result
}

shareList <- function(x,...) {
    doList(share,tryShare,x,...)
}


shareS4 <- function(x,...){
    doS4(tryShare,x,...)
}


promptError <- function(x, ...) {
    args <- completeOptions(...)
    mustWork <- args[["mustWork"]]
    if(is.null(x))
        return(x)
    if (!mustWork)
        return(x)
    stop(
        "The object of the class <",
        paste0(class(x),collapse = ", "),
        "> cannot be shared.\n",
        "To suppress this error and return the same object, \n",
        "provide `mustWork = FALSE` as a function argument\n",
        "or change its default value in the package settings\n"
    )
}



#' @rdname share
#' @export
setMethod("share", signature(x = "ANY"), shareANY)
#' #' @rdname share
#' #' @export
#' setMethod("share", signature(x = "character"), promptError)
#' #' @rdname share
#' #' @export
#' setMethod("share", signature(x = "vector"), shareAtomic)
#' #' @rdname share
#' #' @export
#' setMethod("share", signature(x = "matrix"), shareAtomic)
#' #' @rdname share
#' #' @export
#' setMethod("share", signature(x = "list"), shareList)
#' setMethod("share", signature(x = "environment"), promptError)





#' @rdname share
#' @export
tryShare <- function(x, ...) {
    options <- list(x = x, ...)
    options["mustWork"] <- FALSE
    do.call(share, options)
}









