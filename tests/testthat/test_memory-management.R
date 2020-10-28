context("memory management")
sharedObjectPkgOptions(minLength = 1)
library(parallel)

test_that("Manage a shared object at current process", {
    x <- share(1:10)
    gc()
    info <- sharedObjectProperties(x)
    expect_true(hasSharedMemory(info$dataId))
    rm(x)
    gc()
    expect_false(hasSharedMemory(info$dataId))
})

test_that("release a shared object when it is in a child process", {
    cl <- makeCluster(1)
    x <- share(1:10)
    info <- sharedObjectProperties(x)
    expect_true(hasSharedMemory(info$dataId))
    clusterExport(cl, "x", envir = environment())
    gc()
    ## Check equality
    expect_equal(clusterEvalQ(cl, x)[[1]], 1:10)
    gc()
    expect_equal(x, clusterEvalQ(cl, x)[[1]])
    gc()
    expect_true(hasSharedMemory(info$dataId))
    ## release it from the main process
    rm(x)
    gc()

    if(getOS() == "windows"){
        expect_true(hasSharedMemory(info$dataId))
        ## release it from another process
        clusterEvalQ(cl,{rm(x);gc()})
        gc()
        expect_false(hasSharedMemory(info$dataId))
    }else{
        expect_false(hasSharedMemory(info$dataId))
        expect_warning(clusterEvalQ(cl,x))
        expect_error(clusterEvalQ(cl,{rm(x);gc()}),NA)
    }
    expect_error(clusterEvalQ(cl,{gc()}), NA)
    stopCluster(cl)
})


test_that("release a shared object from a child process", {
    cl <- makeCluster(1)
    x <- share(1:10)
    info <- sharedObjectProperties(x)
    expect_true(hasSharedMemory(info$dataId))
    clusterExport(cl, "x", envir = environment())
    gc()
    ## Check equality
    expect_equal(clusterEvalQ(cl, x)[[1]], 1:10)
    gc()
    expect_equal(x, clusterEvalQ(cl, x)[[1]])
    gc()
    expect_true(hasSharedMemory(info$dataId))
    ## release it from the child process
    clusterEvalQ(cl,{rm(x);gc()})
    expect_true(hasSharedMemory(info$dataId))
    ## release it from the main process
    rm(x)
    gc()
    expect_false(hasSharedMemory(info$dataId))
    stopCluster(cl)
})


test_that("create a shared object from a child process then remove from child first", {
    cl <- makeCluster(1)
    x <- clusterEvalQ(cl, {
        library(SharedObject)
        x <- share(1:10)
        x
    })[[1]]
    info <- sharedObjectProperties(x)
    expect_true(hasSharedMemory(info$dataId))
    gc()
    ## Check equality
    expect_equal(clusterEvalQ(cl, x)[[1]], 1:10)
    gc()
    expect_equal(x, 1:10)
    gc()
    expect_true(hasSharedMemory(info$dataId))
    ## release it from the child process
    clusterEvalQ(cl,{rm(x);gc()})
    if(getOS() == "windows"){
        expect_true(hasSharedMemory(info$dataId))
    }else{
        expect_false(hasSharedMemory(info$dataId))
    }
    expect_equal(x, 1:10)
    ## release it from the main process
    rm(x)
    gc()
    expect_false(hasSharedMemory(info$dataId))
    stopCluster(cl)
})


test_that("create a shared object from a child process then remove from master first", {
    cl <- makeCluster(1)
    x <- clusterEvalQ(cl, {
        library(SharedObject)
        x <- share(1:10)
        x
    })[[1]]
    info <- sharedObjectProperties(x)
    expect_true(hasSharedMemory(info$dataId))
    gc()
    ## Check equality
    expect_equal(1:10, clusterEvalQ(cl, x)[[1]])
    gc()
    expect_equal(x, 1:10)
    gc()
    expect_true(hasSharedMemory(info$dataId))
    ## release it from the master process
    rm(x)
    gc()
    expect_true(hasSharedMemory(info$dataId))
    expect_equal(clusterEvalQ(cl, x)[[1]], 1:10)
    gc()
    ## release it from the child process
    clusterEvalQ(cl,{rm(x);gc()})
    gc()
    expect_false(hasSharedMemory(info$dataId))
    stopCluster(cl)
})

test_that("create a shared object from a child process then stop cluster", {
    cl <- makeCluster(1)
    x <- clusterEvalQ(cl, {
        library(SharedObject)
        x <- share(1:10)
        x
    })[[1]]
    info <- sharedObjectProperties(x)
    expect_true(hasSharedMemory(info$dataId))
    gc()
    ## Check equality
    expect_equal(clusterEvalQ(cl, x)[[1]], 1:10)
    gc()
    expect_equal(x, 1:10)
    gc()
    expect_true(hasSharedMemory(info$dataId))
    ## Directly stop the cluster
    stopCluster(cl)
    gc()
    if(getOS() == "windows"){
        expect_true(hasSharedMemory(info$dataId))
    }else{
        expect_false(hasSharedMemory(info$dataId))
    }
    expect_equal(x, 1:10)
    ## release it from the main process
    rm(x)
    gc()
    Sys.sleep(1)
    gc()
    expect_false(hasSharedMemory(info$dataId))
})
