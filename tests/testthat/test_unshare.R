context("Unshare function")
sharedObjectPkgOptions(minLength = 1)


test_that("unshare Atomic", {
    x <- structure(runif(10), a= 1:10,b=list(a1= 1:2,b1= letters[1:3]))
    x1 <- share(x)
    x2 <- unshare(x1)
    expect_equal(x,x2)
    expect_equal(is.shared(x, depth = Inf, showAttributes = TRUE),
                 is.shared(x2, depth = Inf,showAttributes = TRUE))
})

test_that("unshare list", {
    x <- structure(list(x=runif(10)), a= 1:10,b=list(a1= 1:2,b1= letters[1:3]))
    x1 <- share(x)
    x2 <- unshare(x1)
    expect_equal(x,x2)
    expect_equal(is.shared(x, depth = Inf, showAttributes = TRUE),
                 is.shared(x2, depth = Inf,showAttributes = TRUE))
})

test_that("unshare class", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character", d= "list"),
                         contains = "numeric")
    x <- .myClass(runif(10), a = 1:10, b = letters[1:5], d = list(a1= 1,b1=letters[1:5]))
    x1 <- share(x)
    x2 <- unshare(x1)
    expect_equal(x,x2)
    expect_equal(is.shared(x, depth = Inf, showAttributes = TRUE),
                 is.shared(x2, depth = Inf,showAttributes = TRUE))
    removeClass("myClass")
})

gc()
