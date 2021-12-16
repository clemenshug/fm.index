
<!-- README.md is generated from README.Rmd. Please edit that file -->

# fm.index

fm.index is an R package providing a fast data structure ([FM
Index](https://en.wikipedia.org/wiki/FM-index)) for finding occurrences
of string snippets in large libraries of strings.

fm.index wraps the C++ library [SDSL
v3](https://github.com/xxsds/sdsl-lite) (licensed under the [BSD
license](https://github.com/xxsds/sdsl-lite/blob/9930944f14965c4180e40f7acd5f368fd82a3329/LICENSE)
and uses a [Compressed Suffix
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

### Random strings

For this example, we can generate a million random strings of length 50
and search them for all occurrences of the letters “ab”. Random strings
are hard to compress, so searching the FM Index only yields a modest
\~3-fold speedup.

``` r
library(stringi)
library(microbenchmark)

set.seed(42)
lib_random <- stri_rand_strings(1000000, 50)
idx_random <- fm_index_create(lib_random)

head(
  fm_index_locate("ab", idx_random)
)
#>   pattern_index library_index position
#> 1             1        792988       36
#> 2             1        251840       23
#> 3             1        875759        1
#> 4             1        341798       47
#> 5             1         56031       50
#> 6             1        376499       40

microbenchmark(
  fm.index = fm_index_locate("ab", idx_random),
  stringi = stri_locate_all_coll(lib_random, "ab"),
  times = 10
)
#> Unit: milliseconds
#>      expr       min        lq      mean    median        uq       max neval cld
#>  fm.index  863.0986  899.0695  907.8352  915.2957  924.7553  938.9154    10  a 
#>   stringi 3512.8051 3609.2250 3763.6620 3676.4697 3940.7926 4199.2934    10   b
```

### Real-world text

Real-world text is usually repetitive and much easier to compress than
random strings. In this example, we search each line of a book for all
instances of the word “help”. The text from the book is encoded very
efficiently by the FM Index resulting in a \~50-fold speedup.

``` r
book_path <- tempfile()
download.file("http://aleph.gutenberg.org/1/2/3/7/12370/12370-8.zip", book_path)
book <- scan(unz(book_path, "12370-8.txt"), sep = "\n", what = "character")
idx_book <- fm_index_create(book)

head(
  fm_index_locate("help", idx_book)
)
#>   pattern_index library_index position
#> 1             1          5068       29
#> 2             1          1422       14
#> 3             1          5986       34
#> 4             1          8280        5
#> 5             1          8531       28
#> 6             1          8459       40

microbenchmark(
  fm.index = fm_index_locate("help", idx_book),
  stringi = stri_locate_all_coll(book, "help"),
  times = 10
)
#> Unit: microseconds
#>      expr       min        lq      mean     median        uq       max neval
#>  fm.index   375.546   378.581   533.815   622.6205   634.076   646.129    10
#>   stringi 31206.814 31753.017 32497.154 32429.9385 32745.842 35008.482    10
#>  cld
#>   a 
#>    b
```

## Funding

This work was supported by NIH grants U54-HL127365, U24-DK116204, and
U54-HL127624.

## License

This package is provided under the MIT license. The bundled SDSL library is
licensed under the [BSD license](https://github.com/xxsds/sdsl-lite/blob/9930944f14965c4180e40f7acd5f368fd82a3329/LICENSE).
