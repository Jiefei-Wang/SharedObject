## we want to dispatch the share function based on the SEXP type, not the
## `class` attribute, so the S4 dispatch system does not fit
## our requirement, we create our own dispatching rule:
## 1. If the SEXP is an S4 object, dispatch to shareS4 method
## 2. If the SEXP is an atomic object,
## dispatch to shareAtomic function
## 3. If SEXP is a list, dispatch to shareList
## 4. If SEXP is an environment, dispatch to shareEnvironment
## 4. Otherwise, for any unknown object, throw an error or ignore it
shareANY <- function(x, ..., copyOnWrite, sharedSubset, sharedCopy,
                     sharedAttributes, mustWork, minLength){
    allArgs <- list(x = x, ...)
    if(!missing(copyOnWrite))
        allArgs[["copyOnWrite"]] <- copyOnWrite
    if(!missing(sharedSubset))
        allArgs[["sharedSubset"]] <- sharedSubset
    if(!missing(sharedCopy))
        allArgs[["sharedCopy"]] <- sharedCopy
    if(!missing(sharedAttributes))
        allArgs[["sharedAttributes"]] <- sharedAttributes
    if(!missing(mustWork))
        allArgs[["mustWork"]] <- mustWork
    if(!missing(minLength))
        allArgs[["minLength"]] <- minLength

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
    oldAttrs <- attributes(x)
    if(options$sharedAttributes){
        newAttrs <- doAttributes(tryShare, oldAttrs,...)
    }else{
        newAttrs <- oldAttrs
    }
    if(C_xlength(x)>=options$minLength){
        if(typeof(x)!="character"){
            result <- C_createSharedObjectFromSource(
                x=x,
                copyOnWrite=options$copyOnWrite,
                sharedSubset=options$sharedSubset,
                sharedCopy=options$sharedCopy,
                attributes=as.pairlist(newAttrs)
            )
        }else{
            result <- C_createSharedStringFromSource(
                x=x,
                copyOnWrite=options$copyOnWrite,
                attributes=as.pairlist(newAttrs)
            )
        }
        # Set the object property for the shared object
        C_setObject(result, C_getObject(x))
    }else{
        result <- x
        if(!C_isSameObject(newAttrs,oldAttrs)){
            attributes(result) <- newAttrs
        }
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
    options <- completeOptions(...)
    mustWork <- options$mustWork
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








