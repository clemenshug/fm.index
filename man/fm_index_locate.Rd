% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{fm_index_locate}
\alias{fm_index_locate}
\title{Locate given patterns}
\usage{
fm_index_locate(patterns, index)
}
\arguments{
\item{patterns}{Vector of strings to look for in the index}

\item{index}{Index created with \code{\link[=fm_index_create]{fm_index_create()}}}
}
\value{
A data frame with three columns. \code{pattern_index} is the index
of the query pattern, \code{corpus_index} is the index of the matching
string in the corpus, and \code{position} is the starting position of the
match within the corpus string. All indices are 1-based.
}
\description{
Finds all occurrences of all given patterns in the FM Index, analogous to
\code{\link[stringi:stri_locate]{stringi::stri_locate()}} and \code{str_locate()} from \code{stringr}.
}
\examples{
data("state")
index <- fm_index_create(state.name, case_sensitive = FALSE)
# Find all states with "new" in their names
hits <- fm_index_locate("new", index)
hits
# Show matching strings in library
state.name[hits$library_index]

hits <- fm_index_locate("ar", index)
hits
state.name[hits$library_index]

}
\seealso{
Other FM Index functions: 
\code{\link{fm_index_create}()},
\code{\link{fm_index_save}()}
}
\concept{FM Index functions}
