#' @export
print.fmindex <- function(x, ...) {
  cat(
    "FMIndex with", x$n, "indexed strings.",
    "Size:", format(structure(x$n_bytes, class="object_size"), units="auto"), "\n"
  )
}
