#ifndef DAKE__MATH__MATRIX_HPP
#define DAKE__MATH__MATRIX_HPP

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>

#include <initializer_list>
#include <stdexcept>
#include <type_traits>


namespace dake
{
namespace math
{

#define _double_default(T) typename std::conditional<std::is_floating_point<T>::value, T, double>::type


template<int R, int C, typename T> class mat;


template<int R, typename T> using vec = mat<R, 1, T>;
template<typename T> using scl = mat<1, 1, T>;


typedef vec<2, float> vec2;
typedef vec<3, float> vec3;
typedef vec<4, float> vec4;

typedef vec<2, int> vec2i;
typedef vec<3, int> vec3i;
typedef vec<4, int> vec4i;

typedef vec<2, double> vec2d;
typedef vec<3, double> vec3d;
typedef vec<4, double> vec4d;

typedef mat<2, 2, float> mat2;
typedef mat<2, 3, float> mat23;
typedef mat<2, 4, float> mat24;
typedef mat<3, 2, float> mat32;
typedef mat<3, 3, float> mat3;
typedef mat<3, 4, float> mat34;
typedef mat<4, 2, float> mat42;
typedef mat<4, 3, float> mat43;
typedef mat<4, 4, float> mat4;

typedef mat<2, 2, int> mat2i;
typedef mat<2, 3, int> mat23i;
typedef mat<2, 4, int> mat24i;
typedef mat<3, 2, int> mat32i;
typedef mat<3, 3, int> mat3i;
typedef mat<3, 4, int> mat34i;
typedef mat<4, 2, int> mat42i;
typedef mat<4, 3, int> mat43i;
typedef mat<4, 4, int> mat4i;


static inline void _skip_space(const char **ptr)
{
    while (isspace(**ptr)) {
        (*ptr)++;
    }
}


template<int R, int C, typename T> class mat {
    public:
        T d[R * C];

        enum {
            rows = R,
            columns = C
        };

        typedef T scalar_type;


        template<class...Tv, typename std::enable_if<sizeof...(Tv) == C && C != 1, int>::type = 0>
        mat(Tv... cols)
        {
            int i = 0;
            for (const mat<R, 1, T> &col: {cols...}) {
                memcpy(&d[i], col, sizeof(col));
                i += R;
            }
        }

        template<class...Tv, typename std::enable_if<sizeof...(Tv) == R && C == 1, int>::type = 0>
        mat(Tv... vals)
        {
            int i = 0;
            for (const T &val: {vals...}) {
                d[i++] = val;
            }
        }

        template<int Ro, int Co, typename To>
        mat(const mat<Ro, Co, To> &mo)
        {
            for (int i = 0; i < C; i++) {
                for (int j = 0; j < R; j++) {
                    d[i * R + j] = ((i < Co) && (j < Ro)) ? mo.d[i * Ro + j] : T(0);
                }
            }
        }

        mat(const char *ruby_desc)
        {
            _skip_space(&ruby_desc);
            if (strncmp(ruby_desc, "Matrix[", sizeof("Matrix[") - 1)) {
                throw std::runtime_error("Matrix description not prefixed by \"Matrix[\"");
            }
            ruby_desc += sizeof("Matrix[") - 1;

            for (int i = 0; i < R; i++) {
                _skip_space(&ruby_desc);
                if (*(ruby_desc++) != '[') {
                    throw std::runtime_error("Matrix parsing error: Expected [ for start of row");
                }

                for (int j = 0; j < C; j++) {
                    _skip_space(&ruby_desc);
                    d[j * R + i] = strtod(ruby_desc, const_cast<char **>(&ruby_desc));
                    _skip_space(&ruby_desc);
                    if (j == C - 1) {
                        if (*(ruby_desc++) != ']') {
                            throw std::runtime_error("Matrix parsing error: Expected ] for end of row");
                        }
                    } else {
                        if (*(ruby_desc++) != ',') {
                            throw std::runtime_error("Matrix parsing error: Expected , between row elements");
                        }
                    }
                }

                _skip_space(&ruby_desc);
                if (i == R - 1) {
                    if (*(ruby_desc++) != ']') {
                        throw std::runtime_error("Matrix parsing error: Expected ] for end of matrix");
                    }
                } else {
                    if (*(ruby_desc++) != ',') {
                        throw std::runtime_error("Matrix parsing error: Expected , between rows");
                    }
                }
            }
        }

        mat(void)
        {}


        static mat<R, C, T> zero(void)
        {
            mat<R, C, T> z;
            memset(z.d, 0, sizeof(z.d));
            return z;
        }

        static mat<R, C, T> identity(void)
        {
            static_assert(R == C, "identity() is defined for square matrices only");
            mat<R, C, T> i;
            i.make_identity();
            return i;
        }

        static mat<R, C, T> from_data(const T *f)
        { mat<R, C, T> m; memcpy(m.d, f, sizeof(m.d)); return m; }

        template<class...Tv, typename std::enable_if<R == C && sizeof...(Tv) == C, int>::type = 0>
        static mat<R, C, T> diagonal(Tv... vals)
        {
            mat<R, C, T> ret = mat<R, C, T>::zero();
            int i = 0;
            for (const T &v: {vals...}) {
                ret.d[i * R + i] = v;
                i++;
            }
            return ret;
        }

        static mat<R, C, T> projection(float fovy, float aspect, float zn, float zf)
        {
            static_assert(R == 4 && C == 4 && std::is_floating_point<T>::value, "projection() is defined for 4x4 floating point matrices only");
            float f = 1.f / tanf(fovy / 2.f);
            return mat4(vec4(f / aspect, 0.f, 0.f, 0.f), vec4(0.f, f, 0.f, 0.f), vec4(0.f, 0.f, (zn + zf) / (zn - zf), -1.f), vec4(0.f, 0.f, 2.f * (zn * zf) / (zn - zf), 0.f));
        }


        static mat<R, C, T> orthographic(float left, float right, float top, float bottom, float near, float far)
        {
            static_assert(R == 4 && C == 4 && std::is_floating_point<T>::value, "orthographic() is defined for 4x4 floating point matrices only");
            return mat4(vec4(2.f / (right - left), 0.f, 0.f, 0.f),
                        vec4(0.f, 2.f / (top - bottom), 0.f, 0.f),
                        vec4(0.f, 0.f, 2.f / (near - far), 0.f),
                        vec4((right + left) / (left - right), (top + bottom) / (bottom - top), (far + near) / (near - far), 1.f));
        }


        static mat<R, C, T> direction(const mat<3, 1, T> &v)
        {
            static_assert(R == 4 && C == 1, "direction() is defined for vec4 only");
            return mat<4, 1, T>(v.x(), v.y(), v.z(), T(0));
        }

        static mat<R, C, T> position(const mat<3, 1, T> &v)
        {
            static_assert(R == 4 && C == 1, "position() is defined for vec4 only");
            return mat<4, 1, T>(v.x(), v.y(), v.z(), T(1));
        }


        T &x(void)
        { static_assert(R >= 1 && C == 1, "x() is defined for vectors only"); return d[0]; }

        T &y(void)
        { static_assert(R >= 2 && C == 1, "y() is defined for at least 2-element vectors only"); return d[1]; }

        T &z(void)
        { static_assert(R >= 3 && C == 1, "z() is defined for at least 3-element vectors only"); return d[2]; }

        T &w(void)
        { static_assert(R >= 4 && C == 1, "w() is defined for at least 4-element vectors only"); return d[3]; }

        const T &x(void) const
        { static_assert(R >= 1 && C == 1, "x() is defined for vectors only"); return d[0]; }

        const T &y(void) const
        { static_assert(R >= 2 && C == 1, "y() is defined for at least 2-element vectors only"); return d[1]; }

        const T &z(void) const
        { static_assert(R >= 3 && C == 1, "z() is defined for at least 3-element vectors only"); return d[2]; }

        const T &w(void) const
        { static_assert(R >= 4 && C == 1, "w() is defined for at least 4-element vectors only"); return d[3]; }


        T &r(void) { return x(); }
        T &g(void) { return y(); }
        T &b(void) { return z(); }
        T &a(void) { return w(); }
        const T &r(void) const { return x(); }
        const T &g(void) const { return y(); }
        const T &b(void) const { return z(); }
        const T &a(void) const { return w(); }

        T &s(void) { return x(); }
        T &t(void) { return y(); }
        T &p(void) { return z(); }
        T &q(void) { return w(); }
        const T &s(void) const { return x(); }
        const T &t(void) const { return y(); }
        const T &p(void) const { return z(); }
        const T &q(void) const { return w(); }


        const typename std::conditional<C == 1, T, vec<R, T>>::type &operator[](int i) const
        { return *reinterpret_cast<const typename std::conditional<C == 1, T, vec<R, T>>::type *>(&d[(C == 1) ? i : (i * R)]); }

        typename std::conditional<C == 1, T, vec<R, T>>::type &operator[](int i)
        { return *reinterpret_cast<typename std::conditional<C == 1, T, vec<R, T>>::type *>(&d[(C == 1) ? i : (i * R)]); }


        mat<R, C, T> &operator=(const mat<R, C, T> &om)
        { memcpy(d, om.d, sizeof(d)); return *this; }


        template<typename U>
        auto operator+(const mat<R, C, U> &om) const -> mat<R, C, decltype(d[0] + om.d[0])>
        { mat<R, C, decltype(d[0] + om.d[0])> ret; for (int i = 0; i < R * C; i++) ret.d[i] = d[i] + om.d[i]; return ret; }

        template<typename U>
        mat<R, C, T> &operator+=(const mat<R, C, U> &om)
        { for (int i = 0; i < R * C; i++) d[i] += om.d[i]; return *this; }


        template<typename U>
        auto operator-(const mat<R, C, U> &om) const -> mat<R, C, decltype(d[0] - om.d[0])>
        { mat<R, C, decltype(d[0] - om.d[0])> ret; for (int i = 0; i < R * C; i++) ret.d[i] = d[i] - om.d[i]; return ret; }

        template<typename U>
        mat<R, C, T> &operator-=(const mat<R, C, U> &om)
        { for (int i = 0; i < R * C; i++) d[i] -= om.d[i]; return *this; }


        mat<R, C, T> operator-(void) const
        { mat<R, C, T> ret; for (int i = 0; i < R * C; i++) ret.d[i] = -d[i]; return ret; }


        template<int Co, typename To>
        auto operator*(const mat<C, Co, To> &om) const -> mat<R, Co, decltype(d[0] * om.d[0])>
        {
            mat<R, Co, decltype(d[0] * om.d[0])> ret;

            for (int i = 0; i < Co; i++) {
                for (int j = 0; j < R; j++) {
                    auto val(d[j] * om.d[i * C]);

                    for (int k = 1; k < C; k++) {
                        val += d[k * R + j] * om.d[i * C + k];
                    }

                    ret.d[i * R + j] = val;
                }
            }

            return ret;
        }

        template<typename U>
        auto operator*(const U &scale_v) const -> mat<R, C, decltype(d[0] * scale_v)>
        {
            mat<R, C, decltype(d[0] * scale_v)> ret;

            for (int i = 0; i < R * C; i++) {
                ret.d[i] = d[i] * scale_v;
            }

            return ret;
        }

        mat<R, C, T> &operator*=(const mat<R, C, T> &om)
        {
            static_assert(R == C, "operator*=() is defined for square matrices only");

            return *this = *this * om;
        }

        template<typename U>
        mat<R, C, T> &operator*=(const U &scale_v)
        { for (int i = 0; i < R * C; i++) d[i] *= scale_v; return *this; }


        template<typename U>
        auto operator/(const U &scale_v) const -> mat<R, C, decltype(d[0] / scale_v)>
        {
            mat<R, C, decltype(d[0] / scale_v)> ret;

            for (int i = 0; i < R * C; i++) {
                ret.d[i] = d[i] / scale_v;
            }

            return ret;
        }

        template<typename U>
        mat<R, C, T> &operator/=(const U &scale_v)
        { for (int i = 0; i < R * C; i++) d[i] /= scale_v; return *this; }


        T det(void);
        mat<R, C, T> &transposed_invert(void);

        mat<R, C, T> transposed_inverse(void) const
        { mat<R, C, T> ret(*this); ret.transposed_invert(); return ret; }
        mat<R, C, T> &invert(void)
        { transposed_invert(); transpose(); return *this; }
        mat<R, C, T> inverse(void) const
        { mat<R, C, T> ret(*this); ret.invert(); return ret; }

        mat<C, R, T> &translate(const vec<3, T> &vec);
        mat<C, R, T> translated(const vec<3, T> &vec) const;
        mat<C, R, T> &rotate(T angle, const vec<3, T> &axis);
        mat<C, R, T> rotated(T angle, const vec<3, T> &axis) const;
        mat<C, R, T> &scale(const vec<3, T> &fac);
        mat<C, R, T> scaled(const vec<3, T> &fac) const;


        void make_identity(void)
        {
            static_assert(R == C, "make_identity() is defined for square matrices only");

            memset(d, 0, sizeof(d));
            for (int i = 0; i < R; i++) {
                d[i * R + i] = T(1);
            }
        }


        mat<C, R, T> transposed(void) const
        {
            mat<C, R, T> tm;

            for (int i = 0; i < C; i++) {
                for (int j = 0; j < R; j++) {
                    tm.d[j * C + i] = d[i * R + j];
                }
            }

            return tm;
        }

        mat<R, C, T> &transpose(void)
        {
            static_assert(R == C, "transpose() is defined for square matrices only");
            return *this = transposed();
        }


        mat<R, C, _double_default(T)> eigenvector_matrix(void) const;
        mat<R, C, _double_default(T)> eigenvalue_matrix(void) const;

        mat<R, C, _double_default(T)> svd_U(void) const;
        mat<R, C, _double_default(T)> svd_V(void) const;
        mat<R, C, _double_default(T)> svd_Sigma(void) const;


        template<typename U>
        vec<3, U> cross(const vec<3, U> &ov) const
        {
            static_assert(R == 3 && C == 1, "cross() is defined for 3-element vectors only");

            return vec<3, decltype(d[0] * ov.d[0])>(
                d[1] * ov[2] - d[2] * ov[1],
                d[2] * ov[0] - d[0] * ov[2],
                d[0] * ov[1] - d[1] * ov[0]
            );
        }

        template<typename U>
        auto dot(const vec<R, U> &ov) const -> decltype(d[0] * ov.d[0])
        {
            static_assert(C == 1, "dot() is defined for vectors only");

            auto ret(d[0] * ov[0]);
            for (int i = 1; i < R; i++) {
                ret += d[i] * ov[i];
            }

            return ret;
        }

        _double_default(T) length(void) const
        {
            static_assert(C == 1, "length() is defined for vectors only");

            _double_default(T) len(d[0] * d[0]);
            for (int i = 1; i < R; i++) {
                len += d[i] * d[i];
            }
            return std::sqrt(len);
        }

        vec<R, T> &normalize(void)
        { static_assert(C == 1 && std::is_floating_point<T>::value, "normalize() is defined for floating-point vectors only"); return *this /= length(); }

        vec<R, T> normalized(void) const
        { static_assert(C == 1 && std::is_floating_point<T>::value, "normalize() is defined for floating-point vectors only"); return *this / length(); }


        operator const T *(void) const { return d; }
        operator T *(void) { return d; }


        bool operator==(const mat<R, C, T> &om) const
        {
            for (int i = 0; i < C; i++) {
                for (int j = 0; j < R; j++) {
                    if (!(d[i * R + j] == om.d[i * R + j])) {
                        return false;
                    }
                }
            }
            return true;
        }

        bool operator!=(const mat<R, C, T> &om) const
        {
            return !(*this == om);
        }


    private:
        char *to_ruby(void) const
        {
            // A bit too much, but who cares
            size_t slength = sizeof("Matrix[]");
            for (int i = 0; i < R; i++) {
                slength += sizeof("[],") - 1;
                for (int j = 0; j < C; j++) {
                    slength += snprintf(nullptr, 0, "%g,", d[j * R + i]);
                }
            }

            char *output = static_cast<char *>(malloc(slength)), *outp = output;
            strcpy(outp, "Matrix["); outp += sizeof("Matrix[") - 1;
            for (int i = 0; i < R; i++) {
                strcpy(outp++, "[");
                for (int j = 0; j < C; j++) {
                    outp += sprintf(outp, "%g%s", d[j * R + i], (j == C - 1) ? "" : ",");
                }
                strcpy(outp, (i == R - 1) ? "]]" : "],"); outp += 2;
            }

            return output;
        }

        template<int Rr, int Cr, typename Tr = _double_default(T)>
        mat<Rr, Cr, Tr> ruby_simple_execute(const char *format) const
        {
            char *mat_src = to_ruby();
            char *cmd = static_cast<char *>(malloc(strlen(mat_src) + strlen(format) + 1));

            sprintf(cmd, format, mat_src);
            free(mat_src);

#ifdef __MINGW32__
            throw std::runtime_error("Windows cannot into popen()");
            FILE *pfp = nullptr;
#else
            FILE *pfp = popen(cmd, "r");
#endif

            free(cmd);

            char *result = static_cast<char *>(malloc(Rr * Cr * 64));
            size_t fread_res = fread(result, 1, Rr * Cr * 64, pfp);
            (void)fread_res; // thank you based gcc

            if (!feof(pfp)) {
                throw std::runtime_error("Could not retrieve result matrix from ruby: Too much data");
            }

            fclose(pfp);

            mat<Rr, Cr, Tr> ret(result);
            free(result);
            return ret;
        }
};


template<> template<> mat2 mat2::operator*(const mat2 &om) const;
template<> template<> mat3 mat3::operator*(const mat3 &om) const;
template<> template<> mat4 mat4::operator*(const mat4 &om) const;

template<> mat2 &mat2::operator*=(const mat2 &om);
template<> mat3 &mat3::operator*=(const mat3 &om);
template<> mat4 &mat4::operator*=(const mat4 &om);

template<> template<> vec2 mat2::operator*(const vec2 &v) const;
template<> template<> vec3 mat3::operator*(const vec3 &v) const;
template<> template<> vec4 mat4::operator*(const vec4 &v) const;


template<int R, int C, typename T> static mat<R, C, T> operator*(T lhs, const mat<R, C, T> &rhs)
{ return rhs * lhs; }


template<int R, int C, typename T>
mat<R, C, _double_default(T)> mat<R, C, T>::eigenvector_matrix(void) const
{
    static_assert(R == C, "eigenvector_matrix() is defined for square matrices only");
    return ruby_simple_execute<R, C>("ruby -e \"require 'matrix'; p %s.eigen.eigenvector_matrix\"");
}

template<int R, int C, typename T>
mat<R, C, _double_default(T)> mat<R, C, T>::eigenvalue_matrix(void) const
{
    static_assert(R == C, "eigenvalue_matrix() is defined for square matrices only");
    return ruby_simple_execute<R, C>("ruby -e \"require 'matrix'; p %s.eigen.eigenvalue_matrix\"");
}

template<int R, int C, typename T>
mat<R, C, _double_default(T)> mat<R, C, T>::svd_U(void) const
{
    static_assert(R == C, "svd_U() is defined for square matrices only");
    return (*this * this->transposed()).eigenvector_matrix();
}

template<int R, int C, typename T>
mat<R, C, _double_default(T)> mat<R, C, T>::svd_V(void) const
{
    static_assert(R == C, "svd_V() is defined for square matrices only");
    return (this->transposed() * *this).eigenvector_matrix();
}

template<int R, int C, typename T>
mat<R, C, _double_default(T)> mat<R, C, T>::svd_Sigma(void) const
{
    static_assert(R == C, "svd_Sigma() is defined for square matrices only");

    auto evmat = (*this * this->transposed()).eigenvalue_matrix();
    for (int i = 0; i < C; i++) {
        evmat.d[i * R + i] = std::sqrt(evmat.d[i * R + i]);
    }

    return evmat;
}


template<typename MT>
auto dotp(const MT &v1, const MT &v2) -> decltype(v1.dot(v2))
{ return v1.dot(v2); }

template<typename MT>
auto crossp(const MT &v1, const MT &v2) -> decltype(v1.cross(v2))
{ return v1.cross(v2); }


#undef _double_default

}
}

#endif
