#include "interp_get_py.hpp"
#include "interp_put_py.hpp"

PYBIND11_MODULE(picinterp, m) {
    m.def("gather", &interp_get_float, INTERP_GET_DEFAULTS, INTERP_GET_DOC);
    m.def("gather", &interp_get_double, INTERP_GET_DEFAULTS, INTERP_GET_DOC);
    m.def("scatter", &interp_put_float, INTERP_PUT_DEFAULTS, INTERP_PUT_DOC);
    m.def("scatter", &interp_put_double, INTERP_PUT_DEFAULTS, INTERP_PUT_DOC);
}
