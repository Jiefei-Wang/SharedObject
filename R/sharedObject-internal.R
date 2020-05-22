#############################
## Internal functions
#############################

## Fill the options with their default argument
## if not specified
# completeOptions <- function(options) {
#     for (i in seq_along(sharedOptions)) {
#         name = sharedOptions[i]
#         if (is.null(options[[name]])) {
#             options[[name]] = globalSettings[[name]]
#         }
#     }
#     options
# }

shareAtomic <- function(x, copyOnWrite,sharedSubset,sharedCopy,...) {
    if(is.shared(x)){
        props <- getSharedObjectProperty(x)
        if(props[["copyOnWrite"]]==copyOnWrite&&
           props[["sharedSubset"]]==sharedSubset&&
           props[["sharedCopy"]]==sharedCopy)
            return(x)
    }
    options <- list(copyOnWrite=copyOnWrite,
                    sharedSubset=sharedSubset,
                    sharedCopy=sharedCopy)
    #options <- completeOptions(options)
    #Construct dataInfo vector
    dataInfo = dataInfoTemplate
    dataInfo[["dataId"]] <- 1.0
    dataInfo[["length"]] <- length(x)
    dataInfo[["totalSize"]] <- calculateSharedMemorySize(x)
    dataInfo[["dataType"]] <- 0
    dataInfo[["ownData"]] <- TRUE

    for (i in sharedOptions) {
        dataInfo[i] <- options[[i]]
    }

    result <- C_createSharedMemory(x, dataInfo)
    copyAttribute(result, x)
    result
}

promptErrorANY <- function(x, autoS4Conversion,...){
    promptError(x = x, autoS4Conversion = autoS4Conversion, ...)
}
promptErrorChar <- function(x, mustWork,...){
    promptError(x = x, mustWork = mustWork, ...)
}
promptError <- function(x, ...) {
    args <- list(...)
    mustWork <- args[["mustWork"]]
    autoS4Conversion <- args[["autoS4Conversion"]]
    if(is.null(x))
        return(x)
    if(isS4(x) && autoS4Conversion){
        return(S4shareMethod(x,...))
    }
    if (!mustWork)
        return(x)
    if(isS4(x)){
        stop(
            "No share method is found for the class <",
            paste0(class(x),collapse = ", "),
            ">. To use the automatic conversion function, ",
            "you can specify `autoS4Conversion = TRUE` as an argument of the `share` function."
            )
    }
    stop(
        "The object of the class <",
        paste0(class(x),collapse = ", "),
        "> cannot be shared.\n",
        "To suppress this error and return the same object, \n",
        "provide `mustWork = FALSE` as a function argument\n",
        "or change its default value in the package settings\n"
    )
}

S4shareMethod <- function(x,...){
    slots <- slotNames(x)
    for(i in slots){
        slot(x, i, check = FALSE) <- tryShare(slot(x, i),...)
    }
    x
}


