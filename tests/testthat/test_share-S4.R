context("Share S4 objects")
sharedObjectPkgOptions(minLength = 1)

test_that("atomic type SEXP", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character"), contains = "numeric")
    x <- .myClass(runif(10), a = 1:10, b = letters)
    x1 <- share(x)
    expect_equal(x,x1)
    expect_true(is.shared(x1))
    expect_equal(is.shared(x1, depth = 1), list(.Data= TRUE, a= TRUE, b = TRUE))
    removeClass("myClass")
})

test_that("list type SEXP", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character"), contains = "list")
    x <- .myClass(list(a1 = 1:10, b1 = letters), a = 1:10, b = letters)
    x1 <- share(x)
    expect_equal(x,x1)
    expect_true(is.shared(x1))
    expect_equal(is.shared(x1, depth = 1), list(.Data= TRUE,
                                                a= TRUE, b = TRUE))
    expect_equal(is.shared(x1, depth = 2), list(.Data= list(a1= TRUE,b1=TRUE),
                                                      a= TRUE, b = TRUE))
    removeClass("myClass")
})

test_that("S4 type SEXP", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character"))
    x <- .myClass(a = 1:10, b = letters)
    x1 <- share(x)
    expect_identical(x,x1)
    expect_true(is.shared(x1))
    expect_equal(is.shared(x1, depth = 1), list(a= TRUE, b = TRUE))
    removeClass("myClass")
})
gc()
