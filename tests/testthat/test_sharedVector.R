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
  sv=sharedVector(curData)
  expect_equal(curData,sv)
})

test_that(paste0("Testing duplicate for type ",typeName[i]),{
  if(i<=3){
  curData=type[[i]](data)
  sv=sharedVector(curData,duplicate = FALSE)
  sv2=sv
  sv2[1]=type[[i]](1-sv[1])
  expect_equal(sv,sv2)

  sv=sharedVector(curData,duplicate = TRUE)
  sv2=sv
  sv2[1]=type[[i]](1-sv2[1])
  expect_false(curData[1]==sv2[1])
  }
})

test_that(paste0("Testing cluster export for type ",typeName[i]),{
    curData=type[[i]](data)
    sv=sharedVector(curData)
    clusterExport(cl,"sv",envir=environment())
    res=clusterEvalQ(cl,{sv})
    expect_equal(curData,res[[1]])
}
)
}


stopCluster(cl)
gc()
