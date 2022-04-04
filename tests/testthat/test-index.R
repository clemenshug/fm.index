test_that("finding queries works", {
  index <- fm_index_create(c("asDf", "dBd"))
  hits <- fm_index_locate(c("a", "b", "d"), index)
  hits <- hits[
    order(hits$pattern_index, hits$corpus_index, hits$position),
  ]
  rownames(hits) <- NULL
  expect_equal(
    hits,
    data.frame(
      pattern_index = c(1, 2, 3, 3, 3),
      corpus_index = c(1, 2, 1, 2, 2),
      position = c(1, 2, 3, 1, 3)
    )
  )
})

test_that("case sensitivity is respected", {
  index <- fm_index_create(c("asDf", "dBd"), case_sensitive = TRUE)
  hits <- fm_index_locate(c("a", "b", "D"), index)
  hits <- hits[
    order(hits$pattern_index, hits$corpus_index, hits$position),
  ]
  rownames(hits) <- NULL
  expect_equal(
    hits,
    data.frame(
      pattern_index = c(1, 3),
      corpus_index = c(1, 1),
      position = c(1, 3)
    )
  )
})

test_that("indices can be saved and loaded from disk", {
  index1 <- fm_index_create(c("asDf", "dBd"))
  hits1 <- fm_index_locate(c("a", "b", "d"), index1)
  hits1 <- hits1[
    order(hits1$pattern_index, hits1$corpus_index, hits1$position),
  ]
  rownames(hits1) <- NULL
  temp <- tempfile()
  fm_index_save(index1, temp)
  index2 <- fm_index_load(temp)
  hits2 <- fm_index_locate(c("a", "b", "d"), index2)
  hits2 <- hits2[
    order(hits2$pattern_index, hits2$corpus_index, hits2$position),
  ]
  rownames(hits2) <- NULL
  expect_equal(hits1, hits2)
})
