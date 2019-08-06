##dataInfoPropName=c("dataId","processId","typeId","length","totalSize")
##sharedOptions=c("copyOnWrite","sharedSubset","sharedCopy")



#These functions do not need to assess shared memory
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
#These functions will get the data from the shared memory
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
#' @rdname internal
.getProperty <- function(x, property) {
    func = .funcList[[property]]

    if (is.data.frame(x)) {
        res = lapply(seq_len(ncol(x)), function(i)
            func(x[, i]))
    } else{
        res = func(x)
        #res=list(res)
    }
    res
}

#' @param value The new value of the property
#' @rdname internal
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
#' x=share(1:20)
#' ##Check the default values
#' getCopyOnWrite(x)
#' getSharedSubset(x)
#' getSharedCopy(x)
#'
#' ##Set the values
#' setCopyOnWrite(x,FALSE)
#' setSharedSubset(x,FALSE)
#' setSharedCopy(x,TRUE)
#'
#' ##Check the values again
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



#' get a summary report for a shared object
#'
#' @param x A shared object
#' @param as.list Logical, Whether the result should be in a list format even
#' if only one shared property is reported
#' @return
#' If x is a shared vector and as.list is FALSE, the function returns a sharedMemory object.
#' Otherwise, the function returns a list of sharedMemory object(s).
#' @examples
#' x=share(1:10)
#' getSharedProperty(x)
#' @seealso getCopyOnWrite, getSharedSubset, getSharedCopy
#' @export
getSharedProperty <- function(x, as.list = FALSE) {
    if (is.data.frame(x)) {
        res = lapply(seq_len(ncol(x)), function(i)
            getSharedProperty(x[, i]))
    } else{
        dataid = .dataId(x)
        res = structure(C_getDataInfo(dataid),
                        class = "sharedProperty",
                        names = dataInfoName)
        if (as.list) {
            res = list(res)
        }
    }
    res
}



print.sharedProperty <- function(x, ...) {
    cat("Shared memory object\n")
    for (i in seq_along(x)) {
        cat(" ", dataInfoName[i], "\t", as.character(x[i]), "\n")
    }
}
