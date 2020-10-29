context("the ShareObject function")
sharedObjectPkgOptions(minLength = 10)

types <- c("raw", "logical", "integer", "numeric", "complex")

for(type in types){
    test_that("0-length vector", {
        n <- 0
        x <- SharedObject(mode = type, length = n)
        expect_equal(length(x), n)
        expect_equal(type, class(x))
        expect_true(is.shared(x))
        expect_equal(x, vector(mode = type, length = n))
    })


    test_that("create empty shared vector but length smaller than minLenght",{
        n <- 5
        x <- SharedObject(mode = type, length = n)
        expect_equal(length(x), n)
        expect_equal(type, class(x))
        expect_true(is.shared(x))
        expect_equal(x, vector(mode = type, length = n))
    })

    test_that("create empty shared vector",{
        n <- 20
        x <- SharedObject(mode = type, length = n)
        expect_equal(length(x), n)
        expect_equal(type, class(x))
        expect_true(is.shared(x))
        expect_equal(x, vector(mode = type, length = n))
    })
}

