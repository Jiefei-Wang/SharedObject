context("Shared vector validation")
gc()

library(parallel)
cl=makeCluster(1)

n=100
data=abs(rnorm(n,0,10))
data[data>100]=0
type=c(
  logical=as.logical,
  integer=as.integer,
  double=as.double,
  raw=as.raw,
  character=as.character
)
typeName=names(type)

for(i in seq_along(typeName)){
test_that(paste0("Testing basic subset for type ",typeName[i]),{
  curData=type[[i]](data)
  sv=sharedObject(curData)
  expect_equal(curData,sv)
})

test_that(paste0("Testing duplicate for type ",typeName[i]),{
  if(i<=3){
  curData=type[[i]](data)
  sv=sharedObject(curData,copyOnWrite  = FALSE)
  sv2=sv
  sv2[1]=type[[i]](1-sv[1])
  expect_equal(sv,sv2)

  sv=sharedObject(curData,copyOnWrite = TRUE)
  sv2=sv
  sv2[1]=type[[i]](1-sv2[1])
  expect_false(curData[1]==sv2[1])
  }
})

test_that(paste0("Testing cluster export for type ",typeName[i]),{
    curData=type[[i]](data)
    sv=sharedObject(curData)
    clusterExport(cl,"sv",envir=environment())
    res=clusterEvalQ(cl,{sv})
    expect_equal(curData,res[[1]])
}
)
}
test_that("Copy on write switch",{
  so1=sharedObject(data,copyOnWrite = TRUE)
  expect_equal(copyOnwriteProp(so1),TRUE)
  unsetCopyOnwrite(so1)
  so2=so1
  expect_equal(copyOnwriteProp(so1),FALSE)
  expect_equal(copyOnwriteProp(so2),FALSE)
  so2[1]=10
  data[1]=10
  expect_equal(so1,data)
  expect_equal(so2,data)
  setCopyOnwrite(so1)
  so3=so1
  expect_equal(copyOnwriteProp(so1),TRUE)
  expect_equal(copyOnwriteProp(so2),TRUE)
  expect_equal(copyOnwriteProp(so3),TRUE)
  so3[2]=11
  expect_equal(so,data)
  expect_equal(so2,data)
  data[2]=11
  expect_equal(so2,data)
}
)

test_that("type check",{
  data=matrix(0,2,2)
  so=sharedObject(data)
  expect_equal(is.altrep(so),TRUE)
  expect_equal(is.sharedMemory(so),TRUE)
  expect_equal(is.sharedObject(so),TRUE)
  expect_equal(is.altrep(data),FALSE)
  expect_equal(is.sharedMemory(data),FALSE)
  expect_equal(is.sharedObject(data),FALSE)

  data=as.data.frame(data)
  so=sharedObject(data)
  expect_equal(is.altrep(so),FALSE)
  expect_equal(is.sharedMemory(so),FALSE)
  expect_equal(is.sharedObject(so),TRUE)
  expect_equal(is.altrep(data),FALSE)
  expect_equal(is.sharedMemory(data),FALSE)
  expect_equal(is.sharedObject(data),FALSE)
}
)


stopCluster(cl)
gc()
