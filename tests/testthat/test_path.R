context("test the path in the package")

test_that("Package library", {
    libPath <- SharedObject:::pkgconfig("PKG_LIBS")
    expect_true(libPath)
    expect_true(file.exists(libPath))
})

test_that("Package flags", {
    libFlags <- capture.output(SharedObject:::pkgconfig("PKG_CPPFLAGS"))
    expect_true(length(libFlags)==0)
})

test_that("shared memory path", {
    memoryPath <- getSharedMemoryPath()
    if(getOS()=="linux"){
        expect_true(dir.exists(memoryPath))
    }else{
        expect_true(memoryPath=="")
    }
})
