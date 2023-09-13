import os

src_path = "impls"
if not os.path.exists(src_path):
    os.makedirs(src_path)

orders = range(5)

cases = {}
fn_headers = []

class InterpGetItem:

    def __init__(self, typename, order, dim):
        self.typename = typename
        self.order = order
        self.dim = dim

        if not typename in cases:
            cases[typename] = {}
        if not dim in cases[typename]:
            cases[typename][dim] = {}
        if not order in cases[typename][dim]:
            cases[typename][dim][order] = {}

        code = []
        code.append('return ' + self.get_fn() + '(srcs, coords, base, scale);')

        cases[typename][dim][order] = '\n'.join(code)

    def get_impl(self):
        return f"INTERP_GET_IMPL({self.typename}, {self.order}, {self.dim})"

    def get_fn(self):
        return f"INTERP_GET_FN_NAME({self.typename}, {self.order}, {self.dim})"

    def get_declare(self):
        return f"INTERP_GET_DECLARE({self.typename}, {self.order}, {self.dim})"


def emit_fn(typename):
    code = []

    line = []
    line.append(f'std::vector<py::array_t<{typename}>> ')
    line.append(f'interp_get_{typename} (')
    line.append(f'const std::vector<py::array_t<{typename}>> srcs, ')
    line.append(f'const std::vector<py::array_t<{typename}>> coords, ')
    line.append(f'const std::vector<{typename}> base, ')
    line.append(f'const std::vector<{typename}> scale, ')
    line.append(f'int order)')
    decl = ''.join(line)
    fn_headers.append(decl + ';')
    code.append(decl + ' {')

    code.append('const size_t dim = coords.size();')
    code.append('switch(dim) {')
    for dim in cases[typename]:
        code.append(f'case {dim}:')
        code.append('switch(order) {')
        for order in cases[typename][dim]:
            code.append(f'case {order}:')
            code.append(cases[typename][dim][order])
        code.append('default:')
        code.append('throw std::invalid_argument("Order " + std::to_string(order) + " is not implemented");')
        code.append('}')
    code.append('default:')
    code.append('throw std::invalid_argument("Dim " + std::to_string(dim) + " is not implemented");')
    code.append('}')
    code.append('}')

    return '\n'.join(code)

def emit_py_def(doc_str = None):
    code = []
    code.append('#define INTERP_GET_DEFAULTS \\')
    code.append('py::arg("srcs"), \\')
    code.append('py::arg("coords"), \\')
    code.append('py::arg("base"), \\')
    code.append('py::arg("scale"), \\')
    code.append('py::arg("order") = 2')
    code.append(f'#define INTERP_GET_DOC "{doc_str}"')
    return '\n'.join(code)

declares = []

for order in orders:
    cpp_fn = f'interp_get_{order}.cpp'
    with open(os.path.join(src_path, cpp_fn), 'w') as f:
        f.write("#include \"../interp_get_impl.hpp\"\n")
        f.write("#include \"../interp_get.hpp\"\n")
        for dim in range(1, 4):
            for typename in ['float', 'double']:
                item = InterpGetItem(typename, order, dim)
                declares.append(item.get_declare() + ';')
                f.write(item.get_impl() + '\n')

doc_str = """
PIC interpolate. Note that scaling and offsetting of
`coords` must be performed before calling this function.

Parameters:
-----------
srcs : list[ndarray]
    List of source arrays to be interpolated.
    Every array must have dimension `dim` (see below)

coords : list[ndarray]
    List of arrays containing coordinates to be interpolated
    on. Every array must have the same shape.
    Number of elements in this list is `dim`, and must
    match `dim` decided by `srcs`.
    Data type of coords and srcs must be the same, either
    float32 of float64.

base : list[number]
    Minimum coordinate value.

scale : list[number]
    Coordinate scale. For each coordinate dim `x`,
    `(x - base) * scale` will be used to index `srcs`

order : int, 0 to 4. Default 2.
    Order of shape function.

Returns:
--------
results : list[ndarray]
    `len(srcs)` arrays, each have shape `coords[0].shape`.
"""
doc_str = doc_str.replace('\n', '\\n')

with open('./interp_get_py.cpp', 'w') as f:
    f.write("#include \"interp_get.hpp\"\n")
    f.write('\n'.join(declares) + '\n')
    for tn in cases:
        f.write(emit_fn(tn) + '\n')
with open('./interp_get_py.hpp', 'w') as f:
    f.write('#include "interp_get.hpp"\n')
    f.write('\n'.join(fn_headers) + '\n')
    f.write(emit_py_def(doc_str))
