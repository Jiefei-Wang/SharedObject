context("developer APIs")
sharedObjectPkgOptions(minLength = 1)

test_that("Testing memory tools", {
    x <- share(1:10)
    info <- sharedObjectProperties(x)
    check1 <- hasSharedMemory(info$dataId)
    expect_true(check1)
    check2 <- getSharedMemorySize(info$dataId)
    expect_true(check2>=info$totalSize)
})

test_that("Create memory by ID", {
    size <- 10
    id <- allocateSharedMemory(size)
    expect_true(is.character(id))
    result <- hasSharedMemory(id)
    expect_true(result)
    ptr <- mapSharedMemory(id)
    expect_true(is(ptr,"externalptr"))
    result <- getSharedMemorySize(id)
    expect_true(result>=size)
    unmapSharedMemory(id)
    result <- hasSharedMemory(id)
    if(getOS()=="windows"){
        expect_false(result)
    }else{
        expect_true(result)
    }
    freeSharedMemory(id)
    result <- hasSharedMemory(id)
    expect_false(result)
})

test_that("Create memory by name", {
    name <- "SharedObjectPackageTest"
    size <- 10
    noMemory <- TRUE
    if(hasSharedMemory(name)){
        freeSharedMemory(name)
    }
    expect_true(!hasSharedMemory(name))
    if(!hasSharedMemory(name)){
        expect_equal(allocateSharedMemory(size, name),
                     name)
        ## Repeat allocating the same memory
        expect_error(allocateSharedMemory(size, name), NA)
        expect_error(allocateSharedMemory(1000000, name))
        ## Check memory existence
        result <- hasSharedMemory(name)
        expect_true(result)
        ptr <- mapSharedMemory(name)
        expect_true(is(ptr,"externalptr"))
        result <- getSharedMemorySize(name)
        expect_true(result>=size)
        unmapSharedMemory(name)
        result <- hasSharedMemory(name)
        if(getOS()=="windows"){
            expect_false(result)
        }else{
            expect_true(result)
        }
        freeSharedMemory(name)
        result <- hasSharedMemory(name)
        expect_false(result)
    }
})




test_that("Create memory by ID without unmap", {
    size <- 10
    id <- allocateSharedMemory(size)
    expect_true(is.character(id))
    ptr <- mapSharedMemory(id)
    expect_true(is(ptr,"externalptr"))
    freeSharedMemory(id)
    result <- hasSharedMemory(id)
    expect_false(result)
})

test_that("Create memory by name without unmap", {
    name <- "SharedObjectPackageTest"
    size <- 10
    if(hasSharedMemory(name)){
        freeSharedMemory(name)
    }
    expect_true(!hasSharedMemory(name))
    if(!hasSharedMemory(name)){
        allocateSharedMemory(size, name)
        ptr <- mapSharedMemory(name)
        expect_true(is(ptr,"externalptr"))
        freeSharedMemory(name)
        result <- hasSharedMemory(name)
        expect_false(result)
    }
})

if(getOS() == "linux"){
    test_that("listSharedObjects", {
        gc()
        expect_equal(nrow(listSharedObjects()), 0)
        x <- lm(mpg~cyl, mtcars)
        x2 <- share(x)
        expect_true(nrow(listSharedObjects())>0)
        rm(list = "x2")
        gc()
        expect_equal(nrow(listSharedObjects()), 0)
    })
    test_that("list named shared object", {
        name <- "SharedObjectPackageTest2"
        size <- 10
        if(hasSharedMemory(name)){
            freeSharedMemory(name)
        }
        expect_true(!hasSharedMemory(name))
        if(!hasSharedMemory(name)){
            allocateSharedMemory(size,name)
            expect_equal(nrow(listSharedObjects()),1)
            freeSharedMemory(name)
            expect_equal(nrow(listSharedObjects()),0)
        }
    })
}
