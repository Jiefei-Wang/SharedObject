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
            gc()
            so[1:10]
        })
        expect_equal(mydata[1:10], res[[1]][1:10])
        gc()
    }
})

## use 1GB + 1GB each time
## If not shared, use 4GB in total each time
N <- 10
n<- round(10^9/8)
test_that("Testing big memory alloc/free", {
    mydata <- runif(n)
    for(i in seq_len(N)){
        so = share(mydata)
        clusterExport(cl, "so", envir = environment())
        res = clusterEvalQ(cl, {
            gc()
            so[1:10]
        })
        expect_equal(mydata[1:10], res[[1]][1:10])
        rm(list="so")
        gc()
    }
})


stopCluster(cl)
gc()
