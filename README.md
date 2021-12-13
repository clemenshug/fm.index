
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

## Funding

This work was supported by NIH grants U54-HL127365, U24-DK116204, and
U54-HL127624.
