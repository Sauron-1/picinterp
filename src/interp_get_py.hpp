#include "interp_get.hpp"
std::vector<py::array_t<float>> interp_get_float (const std::vector<py::array_t<float>> srcs, const std::vector<py::array_t<float>> coords, const std::vector<float> base, const std::vector<float> scale, int order);
std::vector<py::array_t<double>> interp_get_double (const std::vector<py::array_t<double>> srcs, const std::vector<py::array_t<double>> coords, const std::vector<double> base, const std::vector<double> scale, int order);
#define INTERP_GET_DEFAULTS \
py::arg("srcs"), \
py::arg("coords"), \
py::arg("base"), \
py::arg("scale"), \
py::arg("order") = 2
#define INTERP_GET_DOC "\nPIC interpolate. Note that scaling and offsetting of\n`coords` must be performed before calling this function.\n\nParameters:\n-----------\nsrcs : list[ndarray]\n    List of source arrays to be interpolated.\n    Every array must have dimension `dim` (see below)\n\ncoords : list[ndarray]\n    List of arrays containing coordinates to be interpolated\n    on. Every array must have the same shape.\n    Number of elements in this list is `dim`, and must\n    match `dim` decided by `srcs`.\n    Data type of coords and srcs must be the same, either\n    float32 of float64.\n\nbase : list[number]\n    Minimum coordinate value.\n\nscale : list[number]\n    Coordinate scale. For each coordinate dim `x`,\n    `(x - base) * scale` will be used to index `srcs`\n\norder : int, 0 to 4. Default 2.\n    Order of shape function.\n\nReturns:\n--------\nresults : list[ndarray]\n    `len(srcs)` arrays, each have shape `coords[0].shape`.\n"