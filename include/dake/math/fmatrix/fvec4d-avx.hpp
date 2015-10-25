#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec4d-avx.hpp directly!
#endif


class fvec4d {
    public:
        typedef double scalar_type;
        typedef double vector_type __attribute__((vector_size(32)));

        union {
            vector_type v;
            scalar_type d[4];
        };


        fvec4d(double xv, double yv, double zv, double wv): v {xv, yv, zv, wv} {}
        fvec4d(double val): v {val, val, val, val} {}
        fvec4d(const fvec4d &ov): v {ov.v} {}
        fvec4d(const fvec4 &ov): v {ov.v[0], ov.v[1], ov.v[2], ov.v[3]} {}
        fvec4d(const vector_type &ov): v{ov} {}
        fvec4d(const vec4 &ov): v {ov.d[0], ov.d[1], ov.d[2], ov.d[3]} {}
        fvec4d(void);

        static fvec4d zero(void) { return fvec4d(0., 0., 0., 0.); }
#ifdef __clang__
        static fvec4d from_data(const double *f) { return fvec4d(f[0], f[1], f[2], f[3]); }
#else
        static fvec4d from_data(const double *f) { return __builtin_ia32_loadupd256(f); }
#endif
        static fvec4d direction(const fvec3d &ov);
        static fvec4d position(const fvec3d &ov);

        fvec4d &operator=(const fvec4d &ov) { v = ov.v; return *this; }

        double x(void) const { return v[0]; }
        double y(void) const { return v[1]; }
        double z(void) const { return v[2]; }
        double w(void) const { return v[3]; }
#ifdef __clang__
        double &x(void) { return d[0]; }
        double &y(void) { return d[1]; }
        double &z(void) { return d[2]; }
        double &w(void) { return d[3]; }
#else
        double &x(void) { return v[0]; }
        double &y(void) { return v[1]; }
        double &z(void) { return v[2]; }
        double &w(void) { return v[3]; }
#endif
        double r(void) const { return v[0]; }
        double g(void) const { return v[1]; }
        double b(void) const { return v[2]; }
        double a(void) const { return v[3]; }
#ifdef __clang__
        double &r(void) { return d[0]; }
        double &g(void) { return d[1]; }
        double &b(void) { return d[2]; }
        double &a(void) { return d[3]; }
#else
        double &r(void) { return v[0]; }
        double &g(void) { return v[1]; }
        double &b(void) { return v[2]; }
        double &a(void) { return v[3]; }
#endif
        double s(void) const { return v[0]; }
        double t(void) const { return v[1]; }
        double p(void) const { return v[2]; }
        double q(void) const { return v[3]; }
#ifdef __clang__
        double &s(void) { return d[0]; }
        double &t(void) { return d[1]; }
        double &p(void) { return d[2]; }
        double &q(void) { return d[3]; }
#else
        double &s(void) { return v[0]; }
        double &t(void) { return v[1]; }
        double &p(void) { return v[2]; }
        double &q(void) { return v[3]; }
#endif

        double operator[](int i) const { return v[i]; }
#ifdef __clang__
        double &operator[](int i) { return d[i]; }
#else
        double &operator[](int i) { return v[i]; }
#endif

        fvec4d operator+(const fvec4d &ov) const { return v + ov.v; }
        fvec4d &operator+=(const fvec4d &ov) { v += ov.v; return *this; }

        fvec4d operator-(void) const { return -v; }
        fvec4d operator-(const fvec4d &ov) const { return v - ov.v; }
        fvec4d &operator-=(const fvec4d &ov) { v -= ov.v; return *this; }

        fvec4d operator*(const fvec4d &ov) const { return v * ov.v; }
        fvec4d &operator*=(const fvec4d &ov) { v *= ov.v; return *this; }

        fvec4d operator*(double s) const { return v * s; }
        fvec4d &operator*=(double s) { v *= s; return *this; }

        fvec4d operator/(const fvec4d &ov) const { return v / ov.v; }
        fvec4d &operator/=(const fvec4d &ov) { v /= ov.v; return *this; }

        fvec4d operator/(double s) const { return v / s; }
        fvec4d &operator/=(double s) { v /= s; return *this; }

        double dot(const fvec4d &ov) const
        {
            vector_type muled(v * ov.v);
            return muled[0] + muled[1] + muled[2] + muled[3];
        }

        double length(void) const { return sqrt(dotp(*this, *this)); }

        fvec4d normalized(void) const { return *this / length(); }
        fvec4d &normalize(void) { return *this /= length(); }

        bool operator==(fvec4d &ov) const
        { return v[0] == ov[0] && v[1] == ov[1] && v[2] == ov[2] && v[3] == ov[3]; }
        bool operator!=(fvec4d &ov) const { return !(*this == ov); }

        operator vec4d(void) const { return vec4d(v[0], v[1], v[2], v[3]); }
        operator fvec4(void) const { return fvec4(v[0], v[1], v[2], v[3]); }

        operator double *(void) { return d; }
        operator const double *(void) const { return d; }
};
