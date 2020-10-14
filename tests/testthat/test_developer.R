context("developers: shared memory management")
gc()
get_os <- function(){
    sysinf <- Sys.info()
    if (!is.null(sysinf)){
        os <- sysinf['sysname']
        if (os == 'Darwin')
            os <- "osx"
    } else { ## mystery machine
        os <- .Platform$OS.type
        if (grepl("^darwin", R.version$os))
            os <- "osx"
        if (grepl("linux-gnu", R.version$os))
            os <- "linux"
    }
    tolower(os)
}

test_that("Testing memory tools", {
    x <- share(1:10)
    info <- getSharedObjectProperty(x)
    check1 <- hasSharedMemory(info$dataId)
    expect_true(check1)
    check2 <- getSharedMemorySize(info$dataId)
    expect_true(check2>=info$totalSize)
})

test_that("Create memory by ID", {
    size <- 10
    id <- allocateSharedMemory(size)
    expect_true(is.numeric(id))
    result <- hasSharedMemory(id)
    expect_true(result)
    ptr <- mapSharedMemory(id)
    expect_true(is(ptr,"externalptr"))
    result <- getSharedMemorySize(id)
    expect_true(result>=size)
    result <- unmapSharedMemory(id)
    expect_true(result)
    result <- hasSharedMemory(id)
    if(get_os()=="windows"){
        expect_false(result)
    }else{
        expect_true(result)
    }
    result <- freeSharedMemory(id)
    expect_true(result)
    result <- hasSharedMemory(id)
    expect_false(result)
})

test_that("Create memory by name", {
    name <- "SharedObjectPackageTest"
    size <- 10
    noMemory <- TRUE
    if(hasSharedMemory(name)){
        noMemory <- freeSharedMemory(name)
    }
    expect_true(noMemory)
    if(noMemory){
        allocateNamedSharedMemory(name,size)
        result <- hasSharedMemory(name)
        expect_true(result)
        ptr <- mapSharedMemory(name)
        expect_true(is(ptr,"externalptr"))
        result <- getSharedMemorySize(name)
        expect_true(result>=size)
        result <- unmapSharedMemory(name)
        expect_true(result)
        result <- hasSharedMemory(name)
        if(get_os()=="windows"){
            expect_false(result)
        }else{
            expect_true(result)
        }
        result <- freeSharedMemory(name)
        expect_true(result)
        result <- hasSharedMemory(name)
        expect_false(result)
    }
})




test_that("Create memory by ID without unmap", {
    size <- 10
    id <- allocateSharedMemory(size)
    expect_true(is.numeric(id))
    ptr <- mapSharedMemory(id)
    expect_true(is(ptr,"externalptr"))
    result <- freeSharedMemory(id)
    expect_true(result)
    result <- hasSharedMemory(id)
    expect_false(result)
})

test_that("Create memory by name without unmap", {
    name <- "SharedObjectPackageTest"
    size <- 10
    noMemory <- TRUE
    if(hasSharedMemory(name)){
        noMemory <- freeSharedMemory(name)
    }
    expect_true(noMemory)
    if(noMemory){
        allocateNamedSharedMemory(name,size)
        ptr <- mapSharedMemory(name)
        expect_true(is(ptr,"externalptr"))
        result <- freeSharedMemory(name)
        expect_true(result)
        result <- hasSharedMemory(name)
        expect_false(result)
    }
})
gc()

if(get_os() == "linux"){
    test_that("listSharedObject", {
        expect_equal(nrow(listSharedObject()), 0)
        x <- lm(mpg~cyl, mtcars)
        x2 <- share(x)
        expect_true(nrow(listSharedObject())>0)
        rm(list = "x2")
        gc()
        expect_equal(nrow(listSharedObject()), 0)
    })
    test_that("list named shared object", {
        name <- "SharedObjectPackageTest2"
        size <- 10
        noMemory <- TRUE
        if(hasSharedMemory(name)){
            noMemory <- freeSharedMemory(name)
        }
        expect_true(noMemory)
        if(noMemory){
            allocateNamedSharedMemory(name,size)
            expect_equal(nrow(listSharedObject(includeCharId = TRUE)),1)
            result <- freeSharedMemory(name)
            expect_true(result)
            expect_equal(nrow(listSharedObject(includeCharId = TRUE)),0)
        }
    })
}
