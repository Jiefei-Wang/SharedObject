context("Shared vector memory burden test")
gc()

library(parallel)
cl = makeCluster(2)

N <- 100
n<- 1000


test_that("Testing small memory alloc/free", {
    mydata <- runif(n)
    for(i in seq_len(N)){
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
    }
})
stopCluster(cl)
gc()

cl = makeCluster(2)
## use 128MB + 128MB each time
## If not shared, use 4*128MB in total each time
N <- 10
n<- round(128*1024*1024/8)
test_that("Testing big memory alloc/free", {
    mydata <- runif(n)
    for(i in seq_len(N)){
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
    }
})

stopCluster(cl)
gc()
