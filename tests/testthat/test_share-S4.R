context("Share S4 objects")



test_that("atomic type SEXP", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character"), contains = "numeric")
    x <- .myClass(runif(10), a = 1:10, b = letters)
    x1 <- share(x)
    expect_equal(x,x1)
    expect_equal(is.shared(x1), list(.Data= TRUE, a= TRUE, b = FALSE))
    removeClass("myClass")
})

test_that("list type SEXP", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character"), contains = "list")
    x <- .myClass(list(a1 = 1:10, b1 = letters), a = 1:10, b = letters)
    x1 <- share(x)
    expect_equal(x,x1)
    expect_equal(is.shared(x1,recursive = TRUE), list(.Data= list(a1= TRUE,b1=FALSE),
                                                      a= TRUE, b = FALSE))
    removeClass("myClass")
})

test_that("S4 type SEXP", {
    .myClass <- setClass("myClass", slots = c(a= "numeric",b="character"))
    x <- .myClass(a = 1:10, b = letters)
    x1 <- share(x)
    expect_identical(x,x1)
    expect_equal(is.shared(x1), list(a= TRUE, b = FALSE))
    removeClass("myClass")
})
gc()
