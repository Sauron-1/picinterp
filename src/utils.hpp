#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <xsimd/xsimd.hpp>

#if defined(_OPENMP)
#include <omp.h>
#else
inline int omp_get_max_threads() { return 1; }
inline int omp_get_thread_num() { return 0; }
#endif

namespace py = pybind11;

template<typename T> struct int_type_of { using type = int; };
template<> struct int_type_of<double> { using type = int64_t; };
template<> struct int_type_of<float> { using type = int32_t; };
template<typename T> using int_type_of_t = typename int_type_of<T>::type;

template<typename T> static inline constexpr size_t simd_size_of = xsimd::simd_type<T>::size;

template<typename F>
auto get_same_shape(const std::vector<py::array_t<F>>& a) {
    // Check all have same dim
    size_t N = a.size();
    size_t ndim = a[0].ndim();
    for (size_t i = 0; i < N; ++i)
        if (a[i].ndim() != ndim)
            throw std::runtime_error("Dimension mismatch!");
    std::vector<size_t> shape(ndim);
    for (size_t i = 0; i < ndim; ++i)
        shape[i] = a[0].shape(i);
    
    // Check all have same shape
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < ndim; ++j)
            if (shape[j] != a[i].shape(j))
                throw std::runtime_error("Shape mismatch!");

    return shape;
}

template<typename T, size_t N>
class IndexGeneratorP {
    public:
        IndexGeneratorP(const std::vector<T>& shape) {
            for (auto i = 0; i < N; ++i)
                m_shape[i] = shape[i];
            m_size = 1;
            for (auto s : m_shape) m_size *= s;
        }

        std::array<T, N> operator[](size_t idx) {
            std::array<T, N> result;
            for (size_t _i = 0; _i < N; ++_i) {
                size_t i = N - _i - 1;
                result[i] = idx % m_shape[i];
                idx /= m_shape[i];
            }
            return result;
        }

        size_t size() const { return m_size; }

    private:
        std::array<T, N> m_shape;
        size_t m_size;
};

class IndexGenerator {
    public:
        template<typename T>
        IndexGenerator(const std::vector<T>& shape) {
            m_dim = shape.size();
            m_shape.resize(m_dim);
            for (auto i = 0; i < m_dim; ++i)
                m_shape[i] = shape[i];
            m_size = 1;
            for (auto s : m_shape) m_size *= s;
        }

        void get(size_t idx, std::vector<size_t>& result) {
            for (size_t _i = 0; _i < m_dim; ++_i) {
                size_t i = m_dim - _i - 1;
                result[i] = idx % m_shape[i];
                idx /= m_shape[i];
            }
        }

        size_t size() const { return m_size; }
        size_t ndim() const { return m_dim; }

    private:
        std::vector<size_t> m_shape;
        size_t m_size, m_dim;
};

template<typename T>
class NdArrayWrapper {
    public:
        NdArrayWrapper(const py::array_t<T>& arr) {
            ptr = arr.data();
            strides.resize(arr.ndim());
            for (auto i = 0u; i < strides.size(); ++i) {
                strides[i] = arr.strides(i) / sizeof(T);
            }
        }

        const T& operator[](const std::vector<size_t>& idx) {
            size_t offset = 0;
            for (auto i = 0u; i < idx.size(); ++i) {
                offset += strides[i] * idx[i];
            }
            return ptr[offset];
        }

    private:
        const T* ptr;
        std::vector<size_t> strides;
};

template<typename T>
class NdArrayWrapperMut {
    public:
        NdArrayWrapperMut(py::array_t<T>& arr) {
            ptr = arr.mutable_data();
            strides.resize(arr.ndim());
            for (auto i = 0u; i < strides.size(); ++i) {
                strides[i] = arr.strides(i) / sizeof(T);
            }
        }

        T& operator[](const std::vector<size_t>& idx) {
            size_t offset = 0;
            for (auto i = 0u; i < idx.size(); ++i) {
                offset += strides[i] * idx[i];
            }
            return ptr[offset];
        }

    private:
        T* ptr;
        std::vector<size_t> strides;
};
