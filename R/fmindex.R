#' @export
print.fmindex <- function(object) {
  cat(
    "FMIndex with", object$n, "indexed strings.",
    "Size:", format(structure(object$n_bytes, class="object_size"), units="auto"), "\n"
  )
}
