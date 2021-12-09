test_that("finding queries works", {
  index <- construct_fm_index(c("asdf", "dbd"))
  hits <- fm_index_find(c("a", "b", "d"), index)
  hits <- hits[
    order(hits$pattern_index, hits$library_index, hits$position),
  ]
  rownames(hits) <- NULL
  expect_equal(
    hits,
    data.frame(
      pattern_index = c(1, 2, 3, 3, 3),
      library_index = c(1, 2, 1, 2, 2),
      position = c(1, 2, 3, 1, 3)
    )
  )
})
