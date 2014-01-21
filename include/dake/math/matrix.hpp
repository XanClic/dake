#ifndef DAKE__MATH__MATRIX_HPP
#define DAKE__MATH__MATRIX_HPP

#include <cmath>
#include <cstring>

#include <initializer_list>
#include <type_traits>


namespace dake
{
namespace math
{

template<int R, int C, typename T> class mat;


template<int R, typename T> using vec = mat<R, 1, T>;
template<typename T> using scl = mat<1, 1, T>;


typedef vec<2, float> vec2;
typedef vec<3, float> vec3;
typedef vec<4, float> vec4;

typedef vec<2, int> vec2i;
typedef vec<3, int> vec3i;
typedef vec<4, int> vec4i;

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


template<int R, int C, typename T> class mat
{
    public:
        T d[R * C];


        template<class...Tv, typename std::enable_if<sizeof...(Tv) == C, int>::type = 0>
        mat(Tv... cols)
        {
            int i = 0;
            for (const mat<R, 1, T> &col: {cols...})
            {
                memcpy(&d[i], col, sizeof(col));
                i += R;
            }
        }

        template<class...Tv, typename std::enable_if<sizeof...(Tv) == R && C == 1, int>::type = 0>
        mat(Tv... vals)
        {
            int i = 0;
            for (const T &val: {vals...})
                d[i++] = val;
        }

        mat(const T *f)
        { memcpy(d, f, sizeof(d)); }

        template<int Ro, int Co, typename To>
        mat(const mat<Ro, Co, To> &mo)
        {
            for (int i = 0; i < C; i++)
                for (int j = 0; j < R; j++)
                    if ((i < Co) && (j < Ro))
                        d[i * R + j] = mo.d[i * Ro + j];
        }

        mat(void)
        {}


        static mat<R, C, T> zero(void)
        {
            mat<R, C, T> z;
            memset(z.d, 0.f, sizeof(z.d));
            return z;
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

            for (int i = 0; i < Co; i++)
            {
                for (int j = 0; j < R; j++)
                {
                    float val = 0.f;

                    for (int k = 0; k < C; k++)
                        val += d[k * R + j] * om.d[i * C + k];

                    ret.d[i * R + j] = val;
                }
            }

            return ret;
        }

        template<typename U>
        auto operator*(const U &scale) const -> mat<R, C, decltype(d[0] * scale)>
        {
            mat<R, C, decltype(d[0] * scale)> ret;

            for (int i = 0; i < R * C; i++)
                ret.d[i] = d[i] * scale;

            return ret;
        }

        template<typename U>
        mat<R, C, T> &operator*=(const U &scale)
        { for (int i = 0; i < R * C; i++) d[i] *= scale; return *this; }

        mat<R, C, T> &operator*=(const mat<R, C, T> &om);


        template<typename U>
        auto operator/(const U &scale) const -> mat<R, C, decltype(d[0] / scale)>
        {
            mat<R, C, decltype(d[0] / scale)> ret;

            for (int i = 0; i < R * C; i++)
                ret.d[i] = d[i] / scale;

            return ret;
        }

        template<typename U>
        mat<R, C, T> &operator/=(const U &scale)
        { for (int i = 0; i < R * C; i++) d[i] /= scale; return *this; }


        T det(void);
        void transposed_invert();

        mat<C, R, T> &translate(const vec<3, T> &vec);
        mat<C, R, T> translated(const vec<3, T> &vec) const;
        mat<C, R, T> &rotate(T angle, const vec<3, T> &axis);
        mat<C, R, T> rotated(T angle, const vec<3, T> &axis) const;
        mat<C, R, T> &scale(const vec<3, T> &fac);
        mat<C, R, T> scaled(const vec<3, T> &fac) const;


        void identity(void)
        {
            static_assert(R == C, "identity() is defined for square matrices only");

            memset(d, 0, sizeof(d));
            for (int i = 0; i < R; i++)
                d[i * R + i] = (T)1;
        }


        mat<C, R, T> transposed(void) const
        {
            mat<C, R, T> t;

            for (int i = 0; i < C; i++)
                for (int j = 0; j < R; j++)
                    t.d[j * C + i] = d[i * R + j];

            return t;
        }

        mat<R, C, T> &transpose(void)
        {
            static_assert(R == C, "transpose() is defined for square matrices only");
            return *this = transposed();
        }


        template<typename U>
        auto cross(const vec<3, U> &ov) const -> decltype(d[0] * ov.d[0])
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

            decltype(d[0] * ov.d[0]) ret(0);
            for (int i = 0; i < R; i++)
                ret += d[i] * ov[i];

            return ret;
        }

        typename std::conditional<std::is_floating_point<T>::value, T, double>::type length(void)
        {
            static_assert(C == 1, "length() is defined for vectors only");

            typename std::conditional<std::is_floating_point<T>::value, T, double> len(0);
            for (int i = 0; i < R; i++)
                len += d[i] * d[i];
            return std::sqrt(len);
        }

        void normalize(void)
        { static_assert(C == 1 && std::is_floating_point<T>::value, "normalize() is defined for floating-point vectors only"); *this /= length(); }

        vec<R, T> normalized(void) const
        { static_assert(C == 1 && std::is_floating_point<T>::value, "normalize() is defined for floating-point vectors only"); return *this / length(); }


        operator const T *(void) const { return d; }
        operator T *(void) { return d; }
};


template<> template<> mat2 mat2::operator*(const mat2 &om) const;
template<> template<> mat3 mat3::operator*(const mat3 &om) const;
template<> template<> mat4 mat4::operator*(const mat4 &om) const;

template<> template<> mat2 &mat2::operator*=(const mat2 &om);
template<> template<> mat3 &mat3::operator*=(const mat3 &om);
template<> template<> mat4 &mat4::operator*=(const mat4 &om);

template<> template<> vec2 mat2::operator*(const vec2 &v) const;
template<> template<> vec3 mat3::operator*(const vec3 &v) const;
template<> template<> vec4 mat4::operator*(const vec4 &v) const;

}
}

#endif
