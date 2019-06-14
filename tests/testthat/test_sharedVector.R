context("Shared vector validation")
gc()
set.seed(as.numeric(Sys.time()))

library(parallel)
cl = makeCluster(1)

n = 100
data = floor(abs(runif(n) * 2))
type = c(
  logical = as.logical,
  integer = as.integer,
  double = as.double,
  raw = as.raw
  #character=as.character
)
typeName = names(type)

for (i in seq_along(typeName)) {
  test_that(paste0("Testing basic subset for type ", typeName[i]), {
    curData = type[[i]](data)
    sv = share(curData)
    expect_equal(curData, sv)
  })

  test_that(paste0("Testing duplicate for type ", typeName[i]), {
    if (i <= 3) {
      curData = type[[i]](data)
      sv = share(curData, copyOnWrite  = FALSE)
      sv2 = sv
      sv2[1] = type[[i]](1 - sv[1])
      expect_equal(sv, sv2)

      sv = share(curData, copyOnWrite = TRUE)
      sv2 = sv
      sv2[1] = type[[i]](1 - sv2[1])
      expect_false(curData[1] == sv2[1])
    }
  })

  test_that(paste0("Testing cluster export for type ", typeName[i]), {
    curData = type[[i]](data)
    sv = share(curData)
    clusterExport(cl, "sv", envir = environment())
    res = clusterEvalQ(cl, {
      sv
    })
    expect_equal(curData, res[[1]])
  })
}
test_that("Copy on write switch", {
  so1 = share(data, copyOnWrite = TRUE)
  expect_equal(getCopyOnWrite(so1), TRUE)
  setCopyOnWrite(so1, FALSE)
  so2 = so1
  expect_equal(getCopyOnWrite(so1), FALSE)
  expect_equal(getCopyOnWrite(so2), FALSE)
  so2[1] = 10
  data[1] = 10
  expect_equal(so1, data)
  expect_equal(so2, data)
  setCopyOnWrite(so1, TRUE)
  so3 = so1
  expect_equal(getCopyOnWrite(so1), TRUE)
  expect_equal(getCopyOnWrite(so2), TRUE)
  expect_equal(getCopyOnWrite(so3), TRUE)
  so3[2] = 11
  expect_equal(so1, data)
  expect_equal(so2, data)
  data[2] = 11
  expect_equal(so3, data)
})

test_that("type check", {
  data = matrix(0, 2, 2)
  so = share(data)
  expect_equal(is.altrep(so), TRUE)
  expect_equal(is.shared(so, recursive = FALSE), TRUE)
  expect_equal(is.shared(so, recursive = TRUE), TRUE)
  expect_equal(is.altrep(data), FALSE)
  expect_equal(is.shared(data, recursive = FALSE), FALSE)
  expect_equal(is.shared(data, recursive = TRUE), FALSE)

  data = as.data.frame(data)
  so = share(data)
  expect_equal(is.altrep(so), FALSE)
  expect_equal(is.shared(so, recursive = FALSE), FALSE)
  expect_equal(is.shared(so, recursive = TRUE), TRUE)
  expect_equal(is.altrep(data), FALSE)
  expect_equal(is.shared(data, recursive = FALSE), FALSE)
  expect_equal(is.shared(data, recursive = TRUE), FALSE)
})



test_that("type check", {
  data = matrix(0, 2, 2)
  so = share(data)
  expect_equal(is.altrep(so), TRUE)
  expect_equal(is.shared(so, recursive = FALSE), TRUE)
  expect_equal(is.shared(so, recursive = TRUE), TRUE)
  expect_equal(is.altrep(data), FALSE)
  expect_equal(is.shared(data, recursive = FALSE), FALSE)
  expect_equal(is.shared(data, recursive = TRUE), FALSE)

  data = as.data.frame(data)
  so = share(data)
  expect_equal(is.altrep(so), FALSE)
  expect_equal(is.shared(so, recursive = FALSE), FALSE)
  expect_equal(is.shared(so, recursive = TRUE), TRUE)
  expect_equal(is.altrep(data), FALSE)
  expect_equal(is.shared(data, recursive = FALSE), FALSE)
  expect_equal(is.shared(data, recursive = TRUE), FALSE)
})

test_that("Shared Object API", {
  ## Global option API
  getSharedObjectOptions()
  getSharedObjectOptions("copyOnWrite")
  setSharedObjectOptions(
    copyOnWrite = FALSE,
    sharedSubset = FALSE,
    sharedCopy = FALSE
  )

  #Shared object API
  x = share(data)
  getSharedProperty(x)
  expect_equal(getCopyOnWrite(x), FALSE)
  expect_equal(getSharedCopy(x), FALSE)
  expect_equal(getSharedSubset(x), FALSE)

  setCopyOnWrite(x, TRUE)
  setSharedCopy(x, TRUE)
  setSharedSubset(x, TRUE)
})

test_that("Key management", {
  #Shared object API
  x = share(data)
  sm=getSharedProperty(x)
  key=sm$getDataID()
  rm("x","sm")
  gc()
  gc()

  expect_error(share(data,dataID=key),NA)
})





stopCluster(cl)
gc()
