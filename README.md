
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

index <- fm_index_construct(state.name, case_sensitive = FALSE)
hits <- fm_index_find("new", index)
print(hits)
#>   pattern_index library_index position
#> 1             1            29        1
#> 2             1            30        1
#> 3             1            31        1
#> 4             1            32        1
```

The result is a data frame with three columns. `pattern_index` is the
index of the query pattern, `library_index` is the index of the matching
string in the index, and `position` is the starting position of the
match in the indexed string. All indices are 1-based.

In order to extract the matching states we can use the `library_index`
to subset the vector of original state names.

``` r
print(state.name[hits$library_index])
#> [1] "New Hampshire" "New Jersey"    "New Mexico"    "New York"
```

## Performance

The speedup achieved by using fm.index over simple string scans, for
example using `grepl()` or `stringi::stri_locate()`, depends on the kind
of strings in the library.

For this example, we can generate a million random strings of length 50
and search them for all occurrences of the letters “ab”. Random strings
are hard to compress so searching the FM Index only yields a modest
\~3-fold speedup.

``` r
library(stringi)
library(microbenchmark)

set.seed(42)
lib_random <- stri_rand_strings(1000000, 50)
idx_random <- fm_index_construct(lib_random)

head(
  fm_index_find("ab", idx_random)
)
#>   pattern_index library_index position
#> 1             1        792988       36
#> 2             1        251840       23
#> 3             1        875759        1
#> 4             1        341798       47
#> 5             1         56031       50
#> 6             1        376499       40

microbenchmark(
  fm.index = fm_index_find("ab", idx_random),
  stringi = stri_locate_all_coll(lib_random, "ab"),
  times = 10
)
#> Unit: milliseconds
#>      expr       min        lq      mean    median        uq       max neval cld
#>  fm.index  877.3932  892.8557  906.2232  910.6141  919.9839  926.3932    10  a 
#>   stringi 3317.5898 3460.4412 3574.9189 3557.5405 3665.3045 3969.2219    10   b
```

Real-world text is usually repetitive and much easier to compress than
random strings. In this example, we search each line of a book for all
instances of the word “help”. The text from the book is encoded very
efficiently by the FM Index resulting in a \~50-fold speedup.

``` r
book_path <- tempfile()
download.file("http://aleph.gutenberg.org/1/2/3/7/12370/12370-8.zip", book_path)
book <- scan(unz(book_path, "12370-8.txt"), sep = "\n", what = "character")
idx_book <- fm_index_construct(book)

head(
  fm_index_find("help", idx_book)
)
#>   pattern_index library_index position
#> 1             1          5068       29
#> 2             1          1422       14
#> 3             1          5986       34
#> 4             1          8280        5
#> 5             1          8531       28
#> 6             1          8459       40

microbenchmark(
  fm.index = fm_index_find("help", idx_book),
  stringi = stri_locate_all_coll(book, "help"),
  times = 10
)
#> Unit: microseconds
#>      expr      min        lq       mean    median       uq       max neval cld
#>  fm.index   375.90   493.294   525.0246   538.031   571.46   661.422    10  a 
#>   stringi 25123.82 28314.229 29154.5828 29133.642 29491.40 33389.297    10   b
```

## Funding

This work was supported by NIH grants U54-HL127365, U24-DK116204, and
U54-HL127624.
