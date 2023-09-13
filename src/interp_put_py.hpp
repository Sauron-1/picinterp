#include "interp_put.hpp"
void interp_put_float (std::vector<py::array_t<float>> targets, const std::vector<py::array_t<float>> coords, const std::vector<py::array_t<float>> weights, const std::vector<float> base, const std::vector<float> scale, int order);
void interp_put_double (std::vector<py::array_t<double>> targets, const std::vector<py::array_t<double>> coords, const std::vector<py::array_t<double>> weights, const std::vector<double> base, const std::vector<double> scale, int order);
#define INTERP_PUT_DEFAULTS \
py::arg("targets"), \
py::arg("coords"), \
py::arg("weights"), \
py::arg("base"), \
py::arg("scale"), \
py::arg("order") = 2
#define INTERP_PUT_DOC "\nPIC interpolate. Note that scaling and offsetting of\n`coords` must be performed before calling this function.\n\nParameters:\n-----------\ntargets : list[ndarray]\n    List of source arrays to be interpolated.\n    Every array must have dimension `dim` (see below)\n\ncoords : list[ndarray]\n    List of arrays containing coordinates to be interpolated\n    on. Every array must have the same shape.\n    Number of elements in this list is `dim`, and must\n    match `dim` decided by `targets`.\n    Data type of coords and targets must be the same, either\n    float32 of float64.\n\nweights : list[ndarray]\n    List of weights to be put. Must have same number of\n    arrays as `targets` and the arrays must have the same\n    shape as `coords`.\n\nbase : list[number]\n    Minimum coordinate value.\n\nscale : list[number]\n    Coordinate scale. For each coordinate dim `x`,\n    `(x - base) * scale` will be used to index `targets`\n\norder : int, 0 to 4. Default 2.\n    Order of shape function.\n"