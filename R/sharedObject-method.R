## These comments are for reminding me only
## dataInfoPropName=c("dataId","processId","typeId","length","totalSize")
## sharedOptions=c("copyOnWrite","sharedSubset","sharedCopy")



## These functions do not need to assess shared memory
.dataId <- function(x) {
    referenceInfo = getDataReferenceInfo(x)
    referenceInfo[["dataId"]]
}
.typeName <- function(x) {
    referenceInfo = getDataReferenceInfo(x)
    referenceInfo[["typeName"]]
}
.ownData <- function(x) {
    referenceInfo = getDataReferenceInfo(x)
    if (is.null(referenceInfo))
        return(NULL)
    tag = xptr_tag(referenceInfo[["dataPtr"]])
    as.logical(tag[2])
}
## These functions will get the data from the shared memory
.processId <- function(x) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_getProcessID(dataId)
}
.typeId <- function(x) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_getTypeID(dataId)
}
.length <- function(x) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_getLength(dataId)
}
.totalSize <- function(x) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_getTotalSize(dataId)
}
.copyOnWrite <- function(x) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_getCopyOnWrite(dataId)
}
.sharedSubset <- function(x) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_getSharedSubset(dataId)
}
.sharedCopy <- function(x) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_getSharedCopy(dataId)
}

.setCopyOnWrite <- function(x, value) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_setCopyOnWrite(dataId, value)
}
.setSharedSubset <- function(x, value) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_setSharedSubset(dataId, value)
}
.setSharedCopy <- function(x, value) {
    dataId = .dataId(x)
    if (is.null(dataId))
        return(NULL)
    C_setSharedCopy(dataId, value)
}
.funcList = list(
    dataId = .dataId,
    typeName = .typeName,
    ownData = .ownData,
    processId = .processId,
    typeId = .typeId,
    length = .length,
    totalSize = .totalSize,
    copyOnWrite = .copyOnWrite,
    sharedSubset = .sharedSubset,
    sharedCopy = .sharedCopy,
    setCopyOnWrite = .setCopyOnWrite,
    setSharedSubset = .setSharedSubset,
    setSharedCopy = .setSharedCopy
)

#' @param x A shared object
#' @param property Character, the name of the property
#' @details
#' .getProperty: This function returns a proporty of a shared object.
#' The available properties are `dataId`,`typeName`,`ownData`,`processId`,
#' `typeId`,`length`,`totalSize`,`copyOnWrite`,`sharedSubset`,`sharedCopy`,
#' `setCopyOnWrite`,`setSharedSubset` and `setSharedCopy`.
#' @return
#' .getProperty: The property of a shared vector or
#' a list of properties of a data.frame
#' @rdname internal
#' @export
.getProperty <- function(x, property) {
    func = .funcList[[property]]

    if (is.data.frame(x)) {
        res = lapply(seq_len(ncol(x)), function(i)
            func(x[, i]))
    } else{
        res = func(x)
    }
    res
}

#' @param value The new value of the property
#' @details
#' `.setProperty`: Set a property of a shared object.
#' @rdname internal
#' @return
#' .setProperty: No return value
#' @export
.setProperty <- function(x, property, value) {
    func = .funcList[[property]]
    if (is.data.frame(x)) {
        for (i in seq_len(ncol(x))) {
            func(x[, i], value)
        }
    } else{
        func(x, value)
    }
}


#' Get or set the properties of a shared object
#'
#' @param x A shared object
#' @param value logical, the value you want the property to be. For a `data.frame`, it can be either
#' a single value or a vector. If the length of `value` does not match the column of the `data.frame`,
#' `value` will be replicate to match the length.
#' @return
#' `get`: The property of a shared object
#'
#' `set`: No return value
#' @examples
#' x = share(1:20)
#' ## Check the default values
#' getCopyOnWrite(x)
#' getSharedSubset(x)
#' getSharedCopy(x)
#'
#' ## Set the values
#' setCopyOnWrite(x,FALSE)
#' setSharedSubset(x,FALSE)
#' setSharedCopy(x,TRUE)
#'
#' ## Check the values again
#' getCopyOnWrite(x)
#' getSharedSubset(x)
#' getSharedCopy(x)
#' @rdname sharedProperty
#' @export
getCopyOnWrite <- function(x) {
    .getProperty(x, "copyOnWrite")
}
#' @rdname sharedProperty
#' @export
getSharedSubset <- function(x) {
    .getProperty(x, "sharedSubset")
}
#' @rdname sharedProperty
#' @export
getSharedCopy <- function(x) {
    .getProperty(x, "sharedCopy")
}

#' @rdname sharedProperty
#' @export
setCopyOnWrite <- function(x, value) {
    .setProperty(x, "setCopyOnWrite", value)
}
#' @rdname sharedProperty
#' @export
setSharedSubset <- function(x, value) {
    .setProperty(x, "setSharedSubset", value)
}
#' @rdname sharedProperty
#' @export
setSharedCopy <- function(x, value) {
    .setProperty(x, "setSharedCopy", value)
}

## Get the reference data from a shared object
## It is the data in data1 slot of the ALTREP object
getDataReferenceInfo <- function(x) {
    if (is.shared(x))
        return(getAltData1(x))
    else
        return(NULL)
}

## Always return a list of shared properties(in atomic format)
.getSharedProperties <- function(x) {
    if(!is.sharedObject(x)){
        return(NULL)
    }
    dataid = .dataId(x)
    property = structure(as.list(C_getDataInfo(dataid)),
                         class = "sharedProperty",
                         names = dataInfoNames)
    property
}

#' get a summary report for a shared object
#'
#' The function reports the properties related to the shared object. the individual property
#' can be accessed via `$` operator.
#'
#' @param x A shared object
#' @param as.list Logical, Whether the result should be a list of shared properties even
#' when only one sharedProperty object is reported.
#' @return
#' If x is not a shared object, return `NULL`.
#'
#' If x is a shared vector and as.list is `FALSE`, the function returns a sharedProperty object.
#'
#' Otherwise, the function returns a list of sharedProperty object(s).
#' @examples
#' x = share(1:10)
#' getSharedProperties(x)
#' @seealso getCopyOnWrite, getSharedSubset, getSharedCopy
#' @export
getSharedProperties <- function(x, as.list = FALSE) {
    if (is.atomic(x)) {
        property = .getSharedProperties(x)
        if(as.list){
            property=list(property)
        }
        return(property)
    }
    if(is.data.frame(x)){
        property= lapply(seq_len(ncol(x)),
                         function(i) .getSharedProperties(x[,i]))
        return(property)
    }
    stop("Unknown data structure")
}

#' @export
print.sharedProperty <- function(x, ...) {
    cat("Shared property object\n")
    for (i in seq_along(x)) {
        cat(" ", dataInfoNames[i], "\t", as.character(x[i]), "\n")
    }
}
