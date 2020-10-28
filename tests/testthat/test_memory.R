context("Shared vector memory burden test")
sharedObjectPkgOptions(minLength = 1)

library(parallel)
cl = makeCluster(2)


N <- 100
n<- 1000
for(i in seq_len(N)){
    test_that(paste0("Testing small memory alloc/free: ", i ),
              {
                  mydata <- runif(n)
                  so = share(mydata)
                  clusterExport(cl, "so", envir = environment())
                  res = clusterEvalQ(cl, {
                      so[1:10]
                  })
                  expect_equal(mydata[1:10], res[[1]][1:10])
                  res = clusterEvalQ(cl, {
                      rm(list="so")
                      gc()
                  })
                  rm(list="so")
                  gc()
              })
}
stopCluster(cl)
gc()

cl = makeCluster(2)
## use 128MB + 128MB each time
## If not shared, use 4*128MB in total each time
N <- 10
n<- round(128*1024*1024/8)

for(i in seq_len(N)){
    test_that(paste0("Testing big memory alloc/free: ", i),
              {
                  mydata <- runif(n)
                  so = share(mydata)
                  clusterExport(cl, "so", envir = environment())
                  res = clusterEvalQ(cl, {
                      so[1:10]
                  })
                  expect_equal(mydata[1:10], res[[1]][1:10])
                  res = clusterEvalQ(cl, {
                      rm(list="so")
                      gc()
                  })
                  rm(list="so")
                  gc()
              })

}
stopCluster(cl)
gc()
