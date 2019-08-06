#' Internal functions
#'
#' @param dataId A vector of numeric numbers. The keys of shared objects
#' @details
#' .removeObject: This function will delete the data associated with the key
#' provided by the argument `dataId`. Any try to read the data after the
#' function call might crash R. If `dataId` is set to "all",
#' the function will delete all data. This function is for the case where
#' R is terminated abnormally without deleting the shared memory. In normal
#' case, R's garbage collector will free the shared memory and there is no
#' need to call this function.
#' @examples
#' ## Create a shared object
#' A = share(1:10)
#' ## Get the ID of the shared object
#' id = .getProperty(A, "dataId")
#' ## Delete the data associated with the ID
#' .removeObject(id)
#' ## Accessing the variable will cause an error since its
#' ## data has been deleted.
#' \dontrun{
#' A
#' }
#' @return
#' .removeObject: no return value
#' @rdname internal
#' @export
.removeObject <- function(dataId) {
  if(is.character(dataId)){
    if(identical(pmatch(dataId,"all"),1L)){
      dataId = getDataIdList()
    }
  }
  if(!is.numeric(dataId)){
    stop("Unknown dataID argument: ",dataId)
  }
  ## return NULL, cannot use vapply
  lapply(as.double(dataId), removeSingleObject)
  invisible()
}

removeSingleObject <- function(dataId) {
  C_clearObj(dataId)
}

getUsedDataId <- function() {
  C_getUsedKey()
}

getDataIdList <- function() {
  C_getDataIdList()
}

#' Get a summary report of the data in the shared memory
#'
#' This function will return a list of the summary of the data in the shared memory.
#'
#' @param data_ids The data ID. If NULL(default), all data will be returned.
#'
#' @return A data.frame
#' @examples
#' getDataInfo()
#' @export
getDataInfo <- function(data_ids = NULL) {
  if (is.null(data_ids)) {
    data_ids = getDataIdList()
  }
  if (length(data_ids) == 0) {
    res = data.frame(matrix(
      vector(),
      0,
      length(dataInfoName),
      dimnames = list(c(), dataInfoName)
    ),
    stringsAsFactors = FALSE)
    return(res)
  }

  res = vapply(data_ids, getSingleDataInfo, numeric(length(dataInfoTemplate)))
  res = as.data.frame(t(res))

  res$dataId = as.character(res$dataId)

  res
}

getSingleDataInfo <- function(data_ids) {
  res = C_getDataInfo(data_ids)
  names(res) = dataInfoName
  res
}
