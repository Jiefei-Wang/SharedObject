context("Share character vector")
sharedObjectPkgOptions(minLength = 0)

test_that("zero-length vector",{
    x1 <- character(0)
    x2 <- share(x1)
    expect_true(is.shared(x2))
    expect_equal(x1, x2)
    props <- sharedObjectProperties(x2)
    expect_equal(props$length, length(x1))
    expect_equal(props$unitSize, 1)
    expect_equal(props$totalSize, length(x1)*props$unitSize)
    expect_equal(props$uniqueChar, length(unique(x1)))
})


test_that("length 1 vector",{
    x1 <- character(1)
    x2 <- share(x1)
    expect_true(is.shared(x2))
    expect_equal(x1, x2)
    props <- sharedObjectProperties(x2)
    expect_equal(props$length, length(x1))
    expect_equal(props$unitSize, 1)
    expect_equal(props$totalSize, length(x1)*props$unitSize)
    expect_equal(props$uniqueChar, length(unique(x1)))
})


test_that("long vector with one unique element",{
    x1 <- character(1000)
    x2 <- share(x1)
    expect_true(is.shared(x2))
    expect_equal(x1, x2)
    props <- sharedObjectProperties(x2)
    expect_equal(props$length, length(x1))
    expect_equal(props$unitSize, 1)
    expect_equal(props$totalSize, length(x1)*props$unitSize)
    expect_equal(props$uniqueChar, length(unique(x1)))
})


test_that("long vector with all unique element",{
    x1 <- as.character(1:256)
    x2 <- share(x1)
    expect_true(is.shared(x2))
    expect_equal(x1, x2)
    props <- sharedObjectProperties(x2)
    expect_equal(props$length, length(x1))
    expect_equal(props$unitSize, 1)
    expect_equal(props$totalSize, length(x1)*props$unitSize)
    expect_equal(props$uniqueChar, length(unique(x1)))


    x1 <- as.character(1:257)
    x2 <- share(x1)
    expect_true(is.shared(x2))
    expect_equal(x1, x2)
    props <- sharedObjectProperties(x2)
    expect_equal(props$length, length(x1))
    expect_equal(props$unitSize, 2)
    expect_equal(props$totalSize, length(x1)*props$unitSize)
    expect_equal(props$uniqueChar, length(unique(x1)))
})
