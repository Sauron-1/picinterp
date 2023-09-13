#include <stdexcept>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#define INTERP_PUT_FN_NAME(type, order, dim) \
    interp_put_py_##type##_##order##_##dim

#define INTERP_PUT_DECLARE(T, O, D) \
    void INTERP_PUT_FN_NAME(T, O, D) ( \
            std::vector<py::array_t<T>>& targets, \
            const std::vector<py::array_t<T>>& coords, \
            const std::vector<py::array_t<T>>& weights, \
            const std::vector<T>& base, const std::vector<T>& scale)

#define INTERP_PUT_IMPL(T, O, D) \
    INTERP_PUT_DECLARE(T, O, D) { interp_put_py_impl<O, D>(targets, coords, weights, base, scale); }
