#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec2d.hpp directly!
#endif


class fvec2d {
    public:
        typedef double scalar_type;
        typedef double vector_type __attribute__((vector_size(16)));

        union {
            vector_type v;
            scalar_type d[2];
        };


        fvec2d(double xv, double yv): v {xv, yv} {}
        fvec2d(double val): v {val, val} {}
        fvec2d(const fvec2d &ov): v {ov.v} {}
#ifdef __AVX__
        fvec2d(const fvec3d &ov): v {ov.v[0], ov.v[1]} {}
        fvec2d(const fvec4d &ov): v {ov.v[0], ov.v[1]} {}
#else
        fvec2d(const fvec3d &ov): v {ov.vs.v} {}
        fvec2d(const fvec4d &ov): v {ov.v[0]} {}
#endif
        fvec2d(const fvec2 &ov): v {ov.v[0], ov.v[1]} {}
        fvec2d(const fvec3 &ov): v {ov.v[0], ov.v[1]} {}
        fvec2d(const fvec4 &ov): v {ov.v[0], ov.v[1]} {}
        fvec2d(const vector_type &ov): v{ov} {}
        fvec2d(const vec2d &ov): v {ov.d[0], ov.d[1]} {}
        fvec2d(void) {}

        static fvec2d zero(void) { return fvec2d(0., 0.); }
        static fvec2d from_data(const double *f) { return fvec2d(f[0], f[1]); }

        fvec2d &operator=(const fvec2d &ov) { v = ov.v; return *this; }

        double x(void) const { return v[0]; }
        double y(void) const { return v[1]; }
#ifdef __clang__
        double &x(void) { return d[0]; }
        double &y(void) { return d[1]; }
#else
        double &x(void) { return v[0]; }
        double &y(void) { return v[1]; }
#endif
        double r(void) const { return v[0]; }
        double g(void) const { return v[1]; }
#ifdef __clang__
        double &r(void) { return d[0]; }
        double &g(void) { return d[1]; }
#else
        double &r(void) { return v[0]; }
        double &g(void) { return v[1]; }
#endif
        double s(void) const { return v[0]; }
        double t(void) const { return v[1]; }
#ifdef __clang__
        double &s(void) { return d[0]; }
        double &t(void) { return d[1]; }
#else
        double &s(void) { return v[0]; }
        double &t(void) { return v[1]; }
#endif

        double operator[](int i) const { return v[i]; }
#ifdef __clang__
        double &operator[](int i) { return d[i]; }
#else
        double &operator[](int i) { return v[i]; }
#endif

        fvec2d operator+(const fvec2d &ov) const { return v + ov.v; }
        fvec2d &operator+=(const fvec2d &ov) { v += ov.v; return *this; }

        fvec2d operator-(void) const { return -v; }
        fvec2d operator-(const fvec2d &ov) const { return v - ov.v; }
        fvec2d &operator-=(const fvec2d &ov) { v -= ov.v; return *this; }

        fvec2d operator*(const fvec2d &ov) const { return v * ov.v; }
        fvec2d &operator*=(const fvec2d &ov) { v *= ov.v; return *this; }

        fvec2d operator*(double s) const { return v * s; }
        fvec2d &operator*=(double s) { v *= s; return *this; }

        fvec2d operator/(const fvec2d &ov) const { return v / ov.v; }
        fvec2d &operator/=(const fvec2d &ov) { v /= ov.v; return *this; }

        fvec2d operator/(double s) const { return v / s; }
        fvec2d &operator/=(double s) { v /= s; return *this; }

        double dot(const fvec2d &ov) const
        {
#ifdef __SSE4_1__
            return __builtin_ia32_dppd(v, ov.v, 0x31)[0];
#else
            vector_type muled(v * ov.v);
            return muled[0] + muled[1];
#endif
        }

        double length(void) const { return sqrtf(dotp(*this, *this)); }
        double approx_rcp_length(void) const { return apr_rsqrt(dotp(*this, *this)); }

        fvec2d normalized(void) const { return *this / length(); }
        fvec2d &normalize(void) { return *this /= length(); }

        fvec2d approx_normalized(void) const { return *this * approx_rcp_length(); }
        fvec2d &approx_normalize(void) { return *this *= approx_rcp_length(); }

        bool operator==(fvec2d &ov) const
        { return v[0] == ov[0] && v[1] == ov[1]; }
        bool operator!=(fvec2d &ov) const { return !(*this == ov); }

        operator vec2d(void) const { return vec2d(v[0], v[1]); }

        operator double *(void) { return d; }
        operator const double *(void) const { return d; }
};
