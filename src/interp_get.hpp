#include <stdexcept>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#define INTERP_GET_FN_NAME(type, order, dim) \
    interp_get_py_##type##_##order##_##dim

#define INTERP_GET_DECLARE(T, O, D) \
    std::vector<py::array_t<T>> INTERP_GET_FN_NAME(T, O, D) ( \
            const std::vector<py::array_t<T>>& srcs, \
            const std::vector<py::array_t<T>>& coords, \
            const std::vector<T>& base, const std::vector<T>& scale)

#define INTERP_GET_IMPL(T, O, D) \
    INTERP_GET_DECLARE(T, O, D) { return interp_get_py_impl<O, D>(srcs, coords, base, scale); }
