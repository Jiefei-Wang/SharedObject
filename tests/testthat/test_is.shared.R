context("is.shared function")
sharedObjectPkgOptions(minLength = 1)

test_that("is.shared atomic", {
    x <- structure(runif(10), a = 1:10, b = letters[1:5], d=list(a1= 1,b1=letters[1:5]))
    x1 <- share(x)
    expect_equal(is.shared(x1),TRUE)
    expect_equal(is.shared(x1,depth = 1),TRUE)
    expect_equal(is.shared(x1,showAttributes = TRUE),
                 structure(TRUE, aShared = TRUE, bShared = TRUE, dShared = TRUE))
    expect_equal(is.shared(x1,depth = 1,showAttributes = TRUE),
                 is.shared(x1,showAttributes = TRUE))
})

test_that("is.shared list", {
    x <- list(a = 1:10, b = letters[1:5],d=list(a1= 1,b1=letters[1:5]))
    x1 <- share(x)
    expect_true(is.shared(x1))
    expect_equal(is.shared(x1, depth = 1),list(a=TRUE,b=TRUE,d=TRUE))
    expect_equal(is.shared(x1, showAttributes = TRUE),
                 structure(TRUE, namesShared = FALSE))
    expect_equal(is.shared(x1, depth = 1, showAttributes = TRUE),
                 structure(list(a = TRUE, b = TRUE, d = TRUE), namesShared = FALSE))
    expect_equal(is.shared(x1, depth = 2, showAttributes = TRUE),
                 structure(list(a = TRUE, b = TRUE, d = list(a1 = TRUE, b1 = TRUE)),
                           namesShared = FALSE))
})



test_that("is.shared S4", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character", d= "list"),
                         contains = "numeric")
    x <- .myClass(runif(10), a = 1:10, b = letters[1:5], d = list(a1= 1,b1=letters[1:5]))
    x1 <- share(x)
    expect_true(is.shared(x1))
    expect_equal(is.shared(x1, depth = 1), list(.Data= TRUE, a= TRUE, b = TRUE, d= TRUE))
    expect_equal(is.shared(x1, depth = 2),
                 list(.Data= TRUE, a= TRUE, b = TRUE, d= list(a1=TRUE,b1=TRUE)))
    expect_equal(is.shared(x1,showAttributes = TRUE),
                 is.shared(x1))
    expect_equal(is.shared(x1, depth = 1, showAttributes=TRUE),
                 is.shared(x1, depth = 1))
    expect_equal(is.shared(x1, depth = 2, showAttributes=TRUE),
                 is.shared(x1, depth = 2))
    removeClass("myClass")
})
gc()
