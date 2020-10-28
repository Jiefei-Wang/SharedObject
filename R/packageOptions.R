globalSettings = new.env()
globalSettings$copyOnWrite = TRUE
globalSettings$sharedSubset = FALSE
globalSettings$sharedCopy = FALSE
globalSettings$sharedAttributes = TRUE
globalSettings$minLength = 3L
globalSettings$mustWork = FALSE

#' Get or set the global options for the SharedObject package
#'
#' @param ... The name of the option(s), it can be either symbols or characters.
#' if the argument is missing, it means getting all option. See examples.
#' @param literal Whether the parameters in `...` are always treated as characters.
#'
#' @return
#' `set`: The old package options
#'
#' `get`: A list of the package options or a single value
#' @examples
#' ## Get all options
#' sharedObjectPkgOptions()
#'
#' ## Get copyOnWrite only
#' sharedObjectPkgOptions(copyOnWrite)
#' sharedObjectPkgOptions("copyOnWrite")
#' opt <- "copyOnWrite"
#' sharedObjectPkgOptions(opt, literal = FALSE)
#'
#'
#' ## Set options
#' sharedObjectPkgOptions(copyOnWrite = FALSE)
#' ## Check if we have changed the option
#' sharedObjectPkgOptions(copyOnWrite)
#'
#' ## Restore the default
#' sharedObjectPkgOptions(copyOnWrite = TRUE)
#' @rdname sharedObjectPkgOptions
#' @export
sharedObjectPkgOptions <- function(..., literal = TRUE){
    sysCall <- as.list(sys.call())[-1]
    args <- processArgs(sysCall,literal)
    if(length(args$argsGetCommand)!=0&&
       length(args$argsSetCommand)!=0){
        stop("You cannot get and set the package settings at the same time")
    }
    if(length(args$argsSetCommand)!=0){
        setSharedObjectPkgOptions(args$argsSetCommand)
    }else{
        getSharedObjectPkgOptions(args$argsGetCommand)
    }
}
setSharedObjectPkgOptions <- function(options) {
    options = checkOptionExistance(options)
    oldOptions <- as.list(globalSettings)[names(options)]
    for (i in seq_along(options)) {
        globalSettings[[names(options)[i]]] = options[[i]]
    }
    if(length(options)==1){
        oldOptions <- unlist(oldOptions)
    }
    invisible(oldOptions)
}

getSharedObjectPkgOptions <- function(options) {
    if (length(options) == 0) {
        return(as.list(globalSettings))
    } else{
        if(length(options)==1){
            return(globalSettings[[options]])
        }else{
            return(as.list(globalSettings)[options])
        }
    }
}

## Check if options exist or not
## return the options that exist
checkOptionExistance <- function(options) {
    noneExistOptions = !names(options) %in% names(globalSettings)
    if (any(noneExistOptions)) {
        vapply(paste0(names(options)[noneExistOptions]), function(x)
            warning(paste0("The option `", x, "` does not exist")), character(1))
    }
    options = options[!noneExistOptions]
    options
}

## Fill the options with their default argument
## if not specified
completeOptions <- function(...) {
    options <- list(...)
    defaultOptions <- sharedObjectPkgOptions()
    ind <- !names(defaultOptions) %in% names(options)
    c(options,defaultOptions[ind])
}



