getAtomicSharedObjectProperties <-  function(x, property) {
    if (!isSharedSEXP(x)) {
        return(NULL)
    }
    if(!is.character(x)){
        dataInfoTemplate <- C_getDataInfoTemplate()
    }else{
        dataInfoTemplate <- C_getStringDataInfoTemplate()
    }
    if (missing(property) || is.null(property)) {
        property <- names(dataInfoTemplate)
    }else{
        property <-
            property[property %in% names(dataInfoTemplate)]
    }
    info <- C_getAltData2(x)
    if("ownData"%in% names(dataInfoTemplate) && "ownData"%in% property){
        info[["ownData"]] <- C_getSharedObjectOwership(x)
    }
    if (length(property) == 1) {
        return(info[[property]])
    } else{
        return(info[property])
    }
}

setAtomicSharedObjectProperties <- function(x, property, value) {
    if (!isSharedSEXP(x)) {
        return(invisible())
    }
    dataInfo <- C_getAltData2(x)
    if (any(!property %in% names(dataInfo))) {
        stop("The property '",
             paste0(property[!property %in% names(dataInfo)], collapse =
                        ", "),
             "' is not found")
    }
    value <- rep_len(value, length(property))
    oldInfo <- dataInfo[property]
    for (i in seq_along(property)) {
        dataInfo[[property[i]]] <- as(value[i], class(dataInfo[[property[i]]]))
    }
    setAltData2(x, dataInfo)
    if("ownData" %in% names(dataInfo)){
        if("ownData" %in% property)
            C_setSharedObjectOwership(x, dataInfo[["ownData"]])
    }

    if(length(property)==1){
        invisible(oldInfo[[1]])
    }else{
        invisible(oldInfo)
    }
}

getOrSetAtomicSharedObjectProperties <- function(x, args){
    if(length(args$argsGetCommand)!=0&&
       length(args$argsSetCommand)!=0){
        stop("You cannot get and set the properties of a shared object at the same time")
    }
    if(length(args$argsSetCommand)!=0){
        setAtomicSharedObjectProperties(x, names(args$argsSetCommand),
                                   unlist(args$argsSetCommand))
    }else{
        getAtomicSharedObjectProperties(x, args$argsGetCommand)
    }
}

#' @rdname sharedObjectProperties
#' @export
setMethod("sharedObjectProperties", signature(x = "ANY"),
          function(x,...,literal){
              sysCall <- as.list(sys.call())[c(-1,-2)]
              args <- processArgs(sysCall,literal)
              getOrSetAtomicSharedObjectProperties(x, args)
          })

#' @rdname sharedObjectProperties
#' @export
setMethod("sharedObjectProperties", signature(x = "list"),
          function(x,...,literal) {
              sysCall <- as.list(sys.call())[c(-1,-2)]
              args <- processArgs(sysCall,literal)
              if(is.atomic(x)){
                  res <- getOrSetAtomicSharedObjectProperties(x, args)
              }else{
                  res <- vector("list", length(x))
                  names(res) <- names(x)
                  for(i in seq_along(x)){
                      res[[i]] <- sharedObjectProperties(x[[i]], args=args)
                  }
              }
              res
          })





#' @param value The value of the property
#' @rdname sharedObjectProperties
#' @export
getCopyOnWrite <- function(x) {
    sharedObjectProperties(x, "copyOnWrite")
}
#' @rdname sharedObjectProperties
#' @export
getSharedSubset <- function(x) {
    sharedObjectProperties(x, "sharedSubset")
}
#' @rdname sharedObjectProperties
#' @export
getSharedCopy <- function(x) {
    sharedObjectProperties(x, "sharedCopy")
}

#' @rdname sharedObjectProperties
#' @export
setCopyOnWrite <- function(x, value) {
    sharedObjectProperties(x, copyOnWrite = value)
}
#' @rdname sharedObjectProperties
#' @export
setSharedSubset <- function(x, value) {
    sharedObjectProperties(x, sharedSubset = value)
}
#' @rdname sharedObjectProperties
#' @export
setSharedCopy <- function(x, value) {
    sharedObjectProperties(x, sharedCopy = value)
}
