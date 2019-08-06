#' Internal functions
#'
#' @details
#' removeAllObject: This function will force the package to delete all data in the shared memory.
#' Any try to read the data after the function call will crash R.
#'
#' @rdname internal
#' @return
#' removeAllObject: no return value
#' @examples
#' removeAllObject()
#' @export
removeAllObject <- function() {
  dids = getDataIdList()
  .removeObject(dids)
  invisible()
}

#' @details
#' removeObject: This function will delete the data associated with the key provided by the function argument.
#' Any try to read the data after the function call will crash R.
#'
#' @param dataId The data ID you want to delete
#' @rdname internal
#' @return
#' removeObject: no return value
#' @export
removeObject <- function(dataId) {
  #return NULL, cannot use vapply
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
