
<!-- README.md is generated from README.Rmd. Please edit that file -->

# fm.index

fm.index is an R package providing a fast data structure ([FM
Index](https://en.wikipedia.org/wiki/FM-index)) for finding occurrences
of string snippets in large libraries of strings.

fm.index wraps the C++ library [SDSL
v3](https://github.com/xxsds/sdsl-lite) and uses a [Compressed Suffix
Array](https://en.wikipedia.org/wiki/Compressed_suffix_array) based on a
[Wavelet Tree](https://en.wikipedia.org/wiki/Wavelet_Tree) of the
[Burrow-Wheeler
Transform](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform)
of the given string library.

## Installation

You can install the development version from
[GitHub](https://github.com/) with:

``` r
# install.packages("remotes")
remotes::install_github("clemenshug/fm.index")
```

## Example

In this example we search a library of state names for all states that
contain the substring “new”.

``` r
library(fm.index)

data("state")
print(state.name)
#>  [1] "Alabama"        "Alaska"         "Arizona"        "Arkansas"      
#>  [5] "California"     "Colorado"       "Connecticut"    "Delaware"      
#>  [9] "Florida"        "Georgia"        "Hawaii"         "Idaho"         
#> [13] "Illinois"       "Indiana"        "Iowa"           "Kansas"        
#> [17] "Kentucky"       "Louisiana"      "Maine"          "Maryland"      
#> [21] "Massachusetts"  "Michigan"       "Minnesota"      "Mississippi"   
#> [25] "Missouri"       "Montana"        "Nebraska"       "Nevada"        
#> [29] "New Hampshire"  "New Jersey"     "New Mexico"     "New York"      
#> [33] "North Carolina" "North Dakota"   "Ohio"           "Oklahoma"      
#> [37] "Oregon"         "Pennsylvania"   "Rhode Island"   "South Carolina"
#> [41] "South Dakota"   "Tennessee"      "Texas"          "Utah"          
#> [45] "Vermont"        "Virginia"       "Washington"     "West Virginia" 
#> [49] "Wisconsin"      "Wyoming"

index <- fm_index_create(state.name, case_sensitive = FALSE)
hits <- fm_index_locate("new", index)
print(hits)
#>   pattern_index corpus_index position
#> 1             1           29        1
#> 2             1           30        1
#> 3             1           31        1
#> 4             1           32        1
```

The result is a data frame with three columns. `pattern_index` is the
index of the query pattern, `corpus_index` is the index of the matching
string in the string corpus, and `position` is the starting position of
the match in the indexed string. All indices are 1-based.

In order to extract the matching states we can use the `corpus_index` to
subset the vector of original state names.

``` r
print(state.name[hits$library_index])
#> character(0)
```

## Performance

The speedup achieved by using fm.index over simple string scans, for
example using `grepl()` or `stringi::stri_locate()`, depends on the kind
of strings in the library.

### Random strings

For this example, we can generate a million random strings of length 50
and search them for all occurrences of the letters “ab”.

``` r
library(stringi)
library(microbenchmark)

set.seed(42)
lib_random <- stri_rand_strings(1000000, 50)
idx_random <- fm_index_create(lib_random)

head(fm_index_locate("ab", idx_random))
#>   pattern_index corpus_index position
#> 1             1       792988       36
#> 2             1       251840       23
#> 3             1       875759        1
#> 4             1       341798       47
#> 5             1        56031       50
#> 6             1       376499       40

random_benchmark <- microbenchmark(
  fm.index = fm_index_locate("ab", idx_random),
  stringi = stri_locate_all_coll(lib_random, "ab"),
  times = 3
)
print(random_benchmark)
#> Unit: milliseconds
#>      expr       min        lq      mean    median       uq       max neval cld
#>  fm.index  927.7187  937.2335  947.8389  946.7483  957.899  969.0497     3  a 
#>   stringi 3732.3779 3772.4135 3833.5300 3812.4491 3884.106 3955.7630     3   b
```

Random strings are hard to compress, so searching using an FM Index only
yields a modest \~4-fold speedup.

### Real-world text

Real-world text is usually repetitive and much easier to compress than
random strings. In this example, we search each line of a book for all
instances of the word “help”.

``` r
book_path <- tempfile()
download.file("http://aleph.gutenberg.org/1/2/3/7/12370/12370.zip", book_path)
book <- scan(unz(book_path, "12370.txt"), sep = "\n", what = "character")
```

Five random lines from the book.

``` r
book[sample(length(book), 5)]
#> [1] "bring her a true and particular account of that strange circumstance,"  
#> [2] "it. For a long while this was my way, that whatever living beings"      
#> [3] "reason his appearance is such; he is burning with the fire of love; how"
#> [4] "prosperous. What strange fancy has possessed the royal mind! If to this"
#> [5] "water of immortality, and that in consequence of having drunk thereof,"
```

``` r
idx_book <- fm_index_create(book)
help_locations <- fm_index_locate("water", idx_book)

nrow(help_locations)
#> [1] 73
head(help_locations)
#>   pattern_index corpus_index position
#> 1             1         5262       43
#> 2             1         5533       16
#> 3             1         4097       23
#> 4             1         6969       18
#> 5             1         4130       13
#> 6             1         4141       46
```

The number of rows in the resulting data frame tells us that the word
“water” occurs 73 times.

``` r
book_benchmark <- microbenchmark(
  fm.index = fm_index_locate("water", idx_book),
  stringi = stri_locate_all_coll(book, "water"),
  times = 10
)
print(book_benchmark)
#> Unit: microseconds
#>      expr       min       lq       mean     median        uq       max neval
#>  fm.index   482.183   522.51   585.4968   528.3555   725.481   772.178    10
#>   stringi 26649.996 27683.24 29156.1119 28875.5665 30187.416 32227.820    10
#>  cld
#>   a 
#>    b
```

The text from this book is encoded very efficiently by the FM Index,
resulting in a \~55-fold speedup.

## Funding

This work was supported by NIH grants U54-HL127365, U24-DK116204, and
U54-HL127624.

## License

This package is provided under the MIT license. The bundled SDSL library
is licensed under the [BSD
license](https://github.com/xxsds/sdsl-lite/blob/9930944f14965c4180e40f7acd5f368fd82a3329/LICENSE).
