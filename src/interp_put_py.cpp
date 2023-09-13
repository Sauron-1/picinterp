#include "interp_put.hpp"
INTERP_PUT_DECLARE(float, 0, 1);
INTERP_PUT_DECLARE(double, 0, 1);
INTERP_PUT_DECLARE(float, 0, 2);
INTERP_PUT_DECLARE(double, 0, 2);
INTERP_PUT_DECLARE(float, 0, 3);
INTERP_PUT_DECLARE(double, 0, 3);
INTERP_PUT_DECLARE(float, 1, 1);
INTERP_PUT_DECLARE(double, 1, 1);
INTERP_PUT_DECLARE(float, 1, 2);
INTERP_PUT_DECLARE(double, 1, 2);
INTERP_PUT_DECLARE(float, 1, 3);
INTERP_PUT_DECLARE(double, 1, 3);
INTERP_PUT_DECLARE(float, 2, 1);
INTERP_PUT_DECLARE(double, 2, 1);
INTERP_PUT_DECLARE(float, 2, 2);
INTERP_PUT_DECLARE(double, 2, 2);
INTERP_PUT_DECLARE(float, 2, 3);
INTERP_PUT_DECLARE(double, 2, 3);
INTERP_PUT_DECLARE(float, 3, 1);
INTERP_PUT_DECLARE(double, 3, 1);
INTERP_PUT_DECLARE(float, 3, 2);
INTERP_PUT_DECLARE(double, 3, 2);
INTERP_PUT_DECLARE(float, 3, 3);
INTERP_PUT_DECLARE(double, 3, 3);
INTERP_PUT_DECLARE(float, 4, 1);
INTERP_PUT_DECLARE(double, 4, 1);
INTERP_PUT_DECLARE(float, 4, 2);
INTERP_PUT_DECLARE(double, 4, 2);
INTERP_PUT_DECLARE(float, 4, 3);
INTERP_PUT_DECLARE(double, 4, 3);
void interp_put_float (std::vector<py::array_t<float>> targets, const std::vector<py::array_t<float>> coords, const std::vector<py::array_t<float>> weights, const std::vector<float> base, const std::vector<float> scale, int order) {
const size_t dim = coords.size();
switch(dim) {
case 1:
switch(order) {
case 0:
INTERP_PUT_FN_NAME(float, 0, 1)(targets, coords, weights, base, scale); break;
case 1:
INTERP_PUT_FN_NAME(float, 1, 1)(targets, coords, weights, base, scale); break;
case 2:
INTERP_PUT_FN_NAME(float, 2, 1)(targets, coords, weights, base, scale); break;
case 3:
INTERP_PUT_FN_NAME(float, 3, 1)(targets, coords, weights, base, scale); break;
case 4:
INTERP_PUT_FN_NAME(float, 4, 1)(targets, coords, weights, base, scale); break;
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
break;
case 2:
switch(order) {
case 0:
INTERP_PUT_FN_NAME(float, 0, 2)(targets, coords, weights, base, scale); break;
case 1:
INTERP_PUT_FN_NAME(float, 1, 2)(targets, coords, weights, base, scale); break;
case 2:
INTERP_PUT_FN_NAME(float, 2, 2)(targets, coords, weights, base, scale); break;
case 3:
INTERP_PUT_FN_NAME(float, 3, 2)(targets, coords, weights, base, scale); break;
case 4:
INTERP_PUT_FN_NAME(float, 4, 2)(targets, coords, weights, base, scale); break;
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
break;
case 3:
switch(order) {
case 0:
INTERP_PUT_FN_NAME(float, 0, 3)(targets, coords, weights, base, scale); break;
case 1:
INTERP_PUT_FN_NAME(float, 1, 3)(targets, coords, weights, base, scale); break;
case 2:
INTERP_PUT_FN_NAME(float, 2, 3)(targets, coords, weights, base, scale); break;
case 3:
INTERP_PUT_FN_NAME(float, 3, 3)(targets, coords, weights, base, scale); break;
case 4:
INTERP_PUT_FN_NAME(float, 4, 3)(targets, coords, weights, base, scale); break;
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
break;
default:
throw std::invalid_argument("Dim " + std::to_string(dim) + " is not implemented");
}
}
void interp_put_double (std::vector<py::array_t<double>> targets, const std::vector<py::array_t<double>> coords, const std::vector<py::array_t<double>> weights, const std::vector<double> base, const std::vector<double> scale, int order) {
const size_t dim = coords.size();
switch(dim) {
case 1:
switch(order) {
case 0:
INTERP_PUT_FN_NAME(double, 0, 1)(targets, coords, weights, base, scale); break;
case 1:
INTERP_PUT_FN_NAME(double, 1, 1)(targets, coords, weights, base, scale); break;
case 2:
INTERP_PUT_FN_NAME(double, 2, 1)(targets, coords, weights, base, scale); break;
case 3:
INTERP_PUT_FN_NAME(double, 3, 1)(targets, coords, weights, base, scale); break;
case 4:
INTERP_PUT_FN_NAME(double, 4, 1)(targets, coords, weights, base, scale); break;
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
break;
case 2:
switch(order) {
case 0:
INTERP_PUT_FN_NAME(double, 0, 2)(targets, coords, weights, base, scale); break;
case 1:
INTERP_PUT_FN_NAME(double, 1, 2)(targets, coords, weights, base, scale); break;
case 2:
INTERP_PUT_FN_NAME(double, 2, 2)(targets, coords, weights, base, scale); break;
case 3:
INTERP_PUT_FN_NAME(double, 3, 2)(targets, coords, weights, base, scale); break;
case 4:
INTERP_PUT_FN_NAME(double, 4, 2)(targets, coords, weights, base, scale); break;
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
break;
case 3:
switch(order) {
case 0:
INTERP_PUT_FN_NAME(double, 0, 3)(targets, coords, weights, base, scale); break;
case 1:
INTERP_PUT_FN_NAME(double, 1, 3)(targets, coords, weights, base, scale); break;
case 2:
INTERP_PUT_FN_NAME(double, 2, 3)(targets, coords, weights, base, scale); break;
case 3:
INTERP_PUT_FN_NAME(double, 3, 3)(targets, coords, weights, base, scale); break;
case 4:
INTERP_PUT_FN_NAME(double, 4, 3)(targets, coords, weights, base, scale); break;
default:
throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");
}
break;
default:
throw std::invalid_argument("Dim " + std::to_string(dim) + " is not implemented");
}
}
