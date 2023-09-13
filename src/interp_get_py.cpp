#include "interp_get.hpp"
INTERP_GET_DECLARE(float, 0, 1);
INTERP_GET_DECLARE(double, 0, 1);
INTERP_GET_DECLARE(float, 0, 2);
INTERP_GET_DECLARE(double, 0, 2);
INTERP_GET_DECLARE(float, 0, 3);
INTERP_GET_DECLARE(double, 0, 3);
INTERP_GET_DECLARE(float, 1, 1);
INTERP_GET_DECLARE(double, 1, 1);
INTERP_GET_DECLARE(float, 1, 2);
INTERP_GET_DECLARE(double, 1, 2);
INTERP_GET_DECLARE(float, 1, 3);
INTERP_GET_DECLARE(double, 1, 3);
INTERP_GET_DECLARE(float, 2, 1);
INTERP_GET_DECLARE(double, 2, 1);
INTERP_GET_DECLARE(float, 2, 2);
INTERP_GET_DECLARE(double, 2, 2);
INTERP_GET_DECLARE(float, 2, 3);
INTERP_GET_DECLARE(double, 2, 3);
INTERP_GET_DECLARE(float, 3, 1);
INTERP_GET_DECLARE(double, 3, 1);
INTERP_GET_DECLARE(float, 3, 2);
INTERP_GET_DECLARE(double, 3, 2);
INTERP_GET_DECLARE(float, 3, 3);
INTERP_GET_DECLARE(double, 3, 3);
INTERP_GET_DECLARE(float, 4, 1);
INTERP_GET_DECLARE(double, 4, 1);
INTERP_GET_DECLARE(float, 4, 2);
INTERP_GET_DECLARE(double, 4, 2);
INTERP_GET_DECLARE(float, 4, 3);
INTERP_GET_DECLARE(double, 4, 3);
std::vector<py::array_t<float>> interp_get_float (const std::vector<py::array_t<float>> srcs, const std::vector<py::array_t<float>> coords, const std::vector<float> base, const std::vector<float> scale, int order) {
const size_t dim = coords.size();
switch(dim) {
case 1:
switch(order) {
case 0:
return INTERP_GET_FN_NAME(float, 0, 1)(srcs, coords, base, scale);
case 1:
return INTERP_GET_FN_NAME(float, 1, 1)(srcs, coords, base, scale);
case 2:
return INTERP_GET_FN_NAME(float, 2, 1)(srcs, coords, base, scale);
case 3:
return INTERP_GET_FN_NAME(float, 3, 1)(srcs, coords, base, scale);
case 4:
return INTERP_GET_FN_NAME(float, 4, 1)(srcs, coords, base, scale);
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
case 2:
switch(order) {
case 0:
return INTERP_GET_FN_NAME(float, 0, 2)(srcs, coords, base, scale);
case 1:
return INTERP_GET_FN_NAME(float, 1, 2)(srcs, coords, base, scale);
case 2:
return INTERP_GET_FN_NAME(float, 2, 2)(srcs, coords, base, scale);
case 3:
return INTERP_GET_FN_NAME(float, 3, 2)(srcs, coords, base, scale);
case 4:
return INTERP_GET_FN_NAME(float, 4, 2)(srcs, coords, base, scale);
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
case 3:
switch(order) {
case 0:
return INTERP_GET_FN_NAME(float, 0, 3)(srcs, coords, base, scale);
case 1:
return INTERP_GET_FN_NAME(float, 1, 3)(srcs, coords, base, scale);
case 2:
return INTERP_GET_FN_NAME(float, 2, 3)(srcs, coords, base, scale);
case 3:
return INTERP_GET_FN_NAME(float, 3, 3)(srcs, coords, base, scale);
case 4:
return INTERP_GET_FN_NAME(float, 4, 3)(srcs, coords, base, scale);
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
default:
throw std::invalid_argument("Dim " + std::to_string(dim) + " is not implemented");
}
}
std::vector<py::array_t<double>> interp_get_double (const std::vector<py::array_t<double>> srcs, const std::vector<py::array_t<double>> coords, const std::vector<double> base, const std::vector<double> scale, int order) {
const size_t dim = coords.size();
switch(dim) {
case 1:
switch(order) {
case 0:
return INTERP_GET_FN_NAME(double, 0, 1)(srcs, coords, base, scale);
case 1:
return INTERP_GET_FN_NAME(double, 1, 1)(srcs, coords, base, scale);
case 2:
return INTERP_GET_FN_NAME(double, 2, 1)(srcs, coords, base, scale);
case 3:
return INTERP_GET_FN_NAME(double, 3, 1)(srcs, coords, base, scale);
case 4:
return INTERP_GET_FN_NAME(double, 4, 1)(srcs, coords, base, scale);
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
case 2:
switch(order) {
case 0:
return INTERP_GET_FN_NAME(double, 0, 2)(srcs, coords, base, scale);
case 1:
return INTERP_GET_FN_NAME(double, 1, 2)(srcs, coords, base, scale);
case 2:
return INTERP_GET_FN_NAME(double, 2, 2)(srcs, coords, base, scale);
case 3:
return INTERP_GET_FN_NAME(double, 3, 2)(srcs, coords, base, scale);
case 4:
return INTERP_GET_FN_NAME(double, 4, 2)(srcs, coords, base, scale);
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
case 3:
switch(order) {
case 0:
return INTERP_GET_FN_NAME(double, 0, 3)(srcs, coords, base, scale);
case 1:
return INTERP_GET_FN_NAME(double, 1, 3)(srcs, coords, base, scale);
case 2:
return INTERP_GET_FN_NAME(double, 2, 3)(srcs, coords, base, scale);
case 3:
return INTERP_GET_FN_NAME(double, 3, 3)(srcs, coords, base, scale);
case 4:
return INTERP_GET_FN_NAME(double, 4, 3)(srcs, coords, base, scale);
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
default:
throw std::invalid_argument("Dim " + std::to_string(dim) + " is not implemented");
}
}
