# picinterp

A C++ library implementing shape functions interpolate for particle-in-cell simulations with SIMD support. The different order of implemented shape functions are obtained by the rectangle function convolving multiple times with itself. The maximum order can be up to 20, and interpolate dimension is arbitrary. A python interface is also included, with order 0-4 and dimension 1-3.

Header `picinterp_cb.hpp` has an additional boundary check comparing to `picinterp.hpp`, where no boundary check is applied and user is responsible for memory safety.

For C++ usage please refer to comments in `include/picinterp.hpp`. Bound-checked version has the same interface. For Python usage please refer to the doc-string.
