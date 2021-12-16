## R CMD check results

No errors, warnings, or notes on MacOS 11.6

## Builds

I successfully built the package on MacOS, Debian 11, and Windows x64.

The x86 build on Windows failed because the underlying SDSL library seemingly
only supports 64-bit systems. E.g.

    sdsl/include/sdsl/uint128_t.hpp:20:56: error: unable to emulate 'TI'
     typedef unsigned int uint128_t __attribute__((mode(TI)));
                                                            ^
    sdsl/include/sdsl/uint128_t.hpp: In function 'std::ostream& sdsl::operator<<(std::ostream&, const uint128_t&)':
    sdsl/include/sdsl/uint128_t.hpp:222:39: warning: right shift count >= width of type [-Wshift-count-overflow]
         uint64_t X[2] = { (uint64_t)(x >> 64), (uint64_t)x };
