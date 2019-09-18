typeName = c("logical", "integer", "double", "raw", "character")
availableType = data.frame(
    row.names = typeName,
    size = as.integer(c(4, 4, 8, 1, 1)),
    id = as.integer(seq_along(typeName)),
    stringsAsFactors = FALSE
)



## the size of the type
typeSize <- function(x) {
    if (!x %in% typeName)
        stop("The type has not been defined: ", x)
    availableType[x, "size"]
}
typeSizeByID <- function(x) {
    typeSize(getTypeNameByID(x))
}

getTypeIDByName <- function(x) {
    if (!x %in% typeName)
        stop("The type has not been defined: ", x)
    availableType[x, "id"]
}

getTypeNameByID <- function(id) {
    ind = which(availableType[, "id"] %in% id)
    if (length(ind) == 0)
        stop("The id does not correspond to any type: ", id)
    typeName[ind]
}


## Copy all attributes from source to target without
## duplicate source
copyAttribute <- function(source, target) {
    att = attributes(source)
    for (i in names(att)) {
        C_attachAttr(target, i, att[[i]])
    }
    target
}

generateDataId <- function() {
    key = round(runif(1, 0, 2 ^ 53))
    C_findAvailableKey(key)
}
hasDataID<-function(dataId){
    C_hasDataID(dataId)
}

#' Is an object of a desired type?
#'
#' @param x an R object
#' @return A logical value
#' @rdname typeCheck
#' @examples
#' x = share(1:10)
#' is.altrep(x)
#' is.shared(x)
#' @export
is.altrep <- function(x) {
    C_ALTREP(x)
}
is.sharedObject <- function(x) {
    if (is.atomic(x)) {
        return(is.sharedVector(x))
    }
    if (is.data.frame(x)) {
        res = vapply(x, is.sharedVector, logical(1))
        return(all(res))
    }
    return(FALSE)
}

is.sharedVector <- function(x) {
    if (is.atomic(x)) {
        altData2 = getAltData2(x)
        if (!is.na(altData2) &&
            !is.null(altData2) && length(altData2) == 1 &&
            altData2 == "shared memory") {
            return(TRUE)
        }
    }
    return(FALSE)
}
#' @rdname typeCheck
#' @param recursive Logical, whether a `data.frame` can be treated as a shared object. If `TRUE`, a `data.frame`
#' is called a shared object if and only if all of its columns are shared objects.
#' @export
is.shared <- function(x, recursive = TRUE) {
    if (recursive) {
        return(is.sharedObject(x))
    } else{
        return(is.sharedVector(x))
    }
}



createInheritedParms <- function(x) {
    sharedProperty = .getSharedProperties(x)[[1]]
    parms = as.list(sharedProperty[sharedOptions])
    parms
}



calculateSharedMemorySize <- function(x) {
    n = length(x)
    if (typeof(x) == "character") {
        char_size = sum(vapply(x, length, numeric(1))) + n
        return(n * 8 + char_size)
    } else{
        return(n * typeSize(typeof(x)))
    }
}


## The function return the value in data 1
## if and only if x is a shared object
getAltData1 <- function(x) {
    return(C_getAltData1(x))
}

getAltData2 <- function(x) {
    return(C_getAltData2(x))
}
