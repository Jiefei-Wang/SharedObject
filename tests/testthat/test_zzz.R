context("Post check for any potential memory leaking")

test_that("Testing memory leaking",
          {
            expect_equal(nrow(listSharedObjects()),0)
          }
          )
