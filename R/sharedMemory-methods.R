#' @include sharedMemory-class.R


sharedMemory$methods(
  getDataID = function() {
    .self$dataID
  },
  getTypeName = function() {
    .self$typeName
  },
  getOwnData = function() {
    .self$ownData
  },
  getProcessID = function() {
    C_getProcessID(.self$getDataID())
  },
  getTypeID = function() {
    C_getTypeID(.self$getDataID())
  },
  getLength = function() {
    C_getLength(.self$getDataID())
  },
  getTotalSize = function() {
    C_getTotalSize(.self$getDataID())
  },
  getCopyOnWrite = function() {
    C_getCopyOnWrite(.self$getDataID())
  },
  getSharedSubset = function() {
    C_getSharedSubset(.self$getDataID())
  },
  getSharedCopy = function() {
    C_getSharedCopy(.self$getDataID())
  },
  setCopyOnWrite = function(value) {
    C_setCopyOnWrite(.self$getDataID(), value)
  },
  setSharedSubset = function(value) {
    C_setSharedSubset(.self$getDataID(), value)
  },
  setSharedCopy = function(value) {
    C_setSharedCopy(.self$getDataID(), value)
  },
  updateAddress = function() {
    .self$address = C_readSharedMemory(.self$getDataID())
  },
  possess = function() {
    .self$ownData = TRUE
  },
  surrender = function() {
    .self$ownData = FALSE
  },
  show = function() {
    cat("Shared memory object\n")
    cat("", 'Data ID:\t', as.character(.self$getDataID()), '\n')
    cat("", 'Process ID:\t', .self$getProcessID(), '\n')
    cat("", 'Type ID:\t', .self$getTypeID(), '\n')
    cat("", 'Type name:\t', .self$getTypeName(), '\n')
    cat("", 'Length:\t', .self$getLength(), '\n')
    cat("", 'Total size:\t', .self$getTotalSize(), '\n')
    cat("", 'Copy on write:\t', .self$getCopyOnWrite(), '\n')
    cat("", 'Shared subset:\t', .self$getSharedSubset(), '\n')
    cat("", 'Shared copy: ', .self$getSharedCopy(), '\n')
    cat("", 'Own data:\t', .self$getOwnData(), '\n')
    cat("", 'Address:\t', capture.output(.self[["address"]]), '\n')
  }
)


createSharedMemoryByID <- function(dataID) {
  sm = sharedMemory()
  sm$initializeWithID(dataID)
  sm
}


