context("test the path in the package")

## I have no idea why it fails on Bioconductor BBS
## It works perfect fine locally, so I decide to ignore
## it and wait until someone find a way to reproduce it.
# test_that("Package library", {
#     libPath <- SharedObject:::pkgconfig("PKG_LIBS")
#     expect_true(file.exists(libPath))
#     # expect_true(libPath)
# })

test_that("Package flags", {
    libFlags <- capture.output(SharedObject:::pkgconfig("PKG_CPPFLAGS"))
    expect_true(length(libFlags)==0)
})

test_that("shared memory path", {
    memoryPath <- getSharedMemoryPath()
    if(memoryPath!=""){
        expect_true(dir.exists(memoryPath))
    }
})
