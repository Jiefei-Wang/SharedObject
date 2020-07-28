## we share the object based on the data structure, not the
## `class` attribute, so the S4 dispatch system does not work
## as we expected, we create our own dispatch rule:
## 1. If the object is an S4 object, dispatch to shareS4 method
## 2. If the internal structure of the object is an atomic object,
## dispatch to shareAtomic function
## 3. If the internal is a list, dispatch to shareList
## 4. If the internal is an environment, dispatch to shareEnvironment
## 4. Otherwise, for any unknow object, throw an error


shareANY <- function(x, ..., copyOnWrite, sharedSubset, sharedCopy, mustWork){
    allArgs <- list(x = x, ...)
    if(!missing(copyOnWrite))
        allArgs[["copyOnWrite"]] <- copyOnWrite
    if(!missing(sharedSubset))
        allArgs[["sharedSubset"]] <- sharedSubset
    if(!missing(sharedCopy))
        allArgs[["sharedCopy"]] <- sharedCopy
    if(!missing(mustWork))
        allArgs[["mustWork"]] <- mustWork

    if(isS4(x)){
        return(do.call(shareS4, allArgs, quote = TRUE))
    }else if(isSharableAtomic(x)){
        return(do.call(shareAtomic, allArgs, quote = TRUE))
    }else if(is.list(x)){
        return(do.call(shareList, allArgs, quote = TRUE))
    }else if(is.environment(x)){
        return(do.call(shareEnvironment, allArgs, quote = TRUE))
    }
    do.call(promptError, allArgs, quote = TRUE)
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
        oldObject <- C_getObject(x)
        attrs <- tryShare(as.pairlist(oldAttrs),...)
        ## we must set the attributes no matter if attributes can be shared or not
        ## for result does not have any attributes.
        ## We do not use `attributes(result) <- attrs` for it
        ## sometimes would duplicate the object using the pointer
        ## and results in an unshared object
        C_setAttributes(result,attrs)
        ## We need to manually set the object attribute when the class 
        ## attribute is not NULL for we use the internal C_setAttributes 
        ## function to set the clas attribute.
        if(oldObject)
            C_setObject(result, TRUE)
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
shareEnvironment <- function(x,...){
   doEnvironment(share,tryShare,x,...)
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


## We use tryShare internally
tryShare <- function(x, ...) {
    options <- list(x = x, ...)
    options["mustWork"] <- FALSE
    do.call(share, options, quote = TRUE)
}









