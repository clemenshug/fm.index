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
      pattern_index = c(0, 1, 2, 2, 2),
      library_index = c(0, 1, 0, 1, 1),
      position = c(0, 1, 2, 0, 2)
    )
  )
})
