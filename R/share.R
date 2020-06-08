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
    }
    dataType <- C_getType(x)
    if(isSEXPAtomic(x)){
        return(shareAtomic(x,...))
    }
    if(isSEXPList(x)){
        return(shareList(x,...))
    }
    promptError(x,...)
}

## x must be an atomic object
shareAtomic <- function(x,...) {
    options <- completeOptions(...)

    ## if the object x has been shared and
    ## all the atomic options are the same,
    ## we will return the same object.
    props <- getSharedObjectProperty(x)
    if(isSharedSEXP(x)&&
       all.equal(options[sharedAtomicOptions],props[sharedAtomicOptions])){
        result <- x
    }else{
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
    }
    ## copy the attributes if necessary
    attris <- attributes(x)
    if(!is.null(attris)){
        oldCopyOnWrite <- setCopyOnWrite(result, FALSE)
        oldOwnData <- setSharedObjectProperty(result, "ownData", FALSE)
        attrs <- tryShare(attris,...)
        attributes(result) <- attrs
        setSharedObjectProperty(result, "ownData", oldOwnData)
        setCopyOnWrite(result, oldCopyOnWrite)
    }
    result
}

shareList <- function(x,...) {
    result <- lapply(x,share,...)
    if(!is.null(attributes(x))){
        if(!all(names(attributes(x)) %in% c("class","names")))
            attributes(result) <- tryShare(attributes(x),...)
        else{
            attributes(result) <- attributes(x)
        }
    }
    result
}

shareS4 <- function(x,...){
    dataType <- C_getType(x)
    ## If the object is an S4SXP,
    ## share its slots
    if(dataType=="S4"||
       dataType=="string"||
       dataType=="other"){
        x1 <- x
        slots <- slotNames(x1)
        for(i in slots){
            slot(x1, i, check = FALSE) <- tryShare(slot(x, i), ...)
        }
    }else{
        ## If the object is not an S4SXP,
        ## Calling the right share method
        C_UNSETS4(x)
        if(isS4(x))
            stop("Unexpected error, cannot convert an S4 object to a non-S4 object")
        x1 <- tryShare(x,...)
        C_SETS4(x)
        C_SETS4(x1)
    }
    x1
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









