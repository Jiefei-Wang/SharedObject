#############################
## Internal functions
#############################

## Fill the options with their default argument
## if not specified
completeOptions <- function(options) {
    for (i in seq_along(sharedOptions)) {
        name = sharedOptions[i]
        if (is.null(options[[name]])) {
            options[[name]] = globalSettings[[name]]
        }
    }
    options
}

shareAtomic <- function(x, ...) {
    options <- list(...)
    options <- completeOptions(options)
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

promptError <- function(x, ...) {
    options <- list(...)
    if (!is.null(options$noError)) {
        if (options$noError)
            return(x)
    } else{
        if (globalSettings$noError)
            return(x)
    }
    stop(
        "The object of type `",
        class(x),
        "' cannot be shared.\n",
        "To suppress this error and return the same object, \n",
        "provide `noError = TRUE` as a function argument\n",
        "or change its default value in the package settings"
    )
}
