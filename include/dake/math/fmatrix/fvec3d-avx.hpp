#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec3d-avx.hpp directly!
#endif


class fvec3d {
    public:
        typedef double scalar_type;
        typedef double vector_type __attribute__((vector_size(32)));

        union {
            vector_type v;
            scalar_type d[3];
        };


        fvec3d(double xv, double yv, double zv): v {xv, yv, zv, 0.} {}
        fvec3d(double val): v {val, val, val} {}
        fvec3d(const fvec3d &ov): v(ov.v) {}
        fvec3d(const fvec4d &ov): v(ov.v) { v[3] = 0.; }
        fvec3d(const fvec3 &ov): v {ov.v[0], ov.v[1], ov.v[2], 0.} {}
        fvec3d(const fvec4 &ov): v {ov.v[0], ov.v[1], ov.v[2], 0.} {}
        fvec3d(const vector_type &ov): v(ov) { v[3] = 0.; }
        fvec3d(const vec3 &ov): v {ov.d[0], ov.d[1], ov.d[2], 0.} {}
        fvec3d(void) {}

        static fvec3d zero(void) { return fvec3(0., 0., 0.); }
#ifdef __clang__
        static fvec3d from_data(const double *f) { return fvec3d(f[0], f[1], f[2]); }
#else
        static fvec3d from_data(const double *f) { return __builtin_ia32_loadupd256(f); }
#endif

        fvec3d &operator=(const fvec3d &ov) { v = ov.v; return *this; }

        double x(void) const { return v[0]; }
        double y(void) const { return v[1]; }
        double z(void) const { return v[2]; }
#ifdef __clang__
        double &x(void) { return d[0]; }
        double &y(void) { return d[1]; }
        double &z(void) { return d[2]; }
#else
        double &x(void) { return v[0]; }
        double &y(void) { return v[1]; }
        double &z(void) { return v[2]; }
#endif
        double r(void) const { return v[0]; }
        double g(void) const { return v[1]; }
        double b(void) const { return v[2]; }
#ifdef __clang__
        double &r(void) { return d[0]; }
        double &g(void) { return d[1]; }
        double &b(void) { return d[2]; }
#else
        double &r(void) { return v[0]; }
        double &g(void) { return v[1]; }
        double &b(void) { return v[2]; }
#endif
        double s(void) const { return v[0]; }
        double t(void) const { return v[1]; }
        double p(void) const { return v[2]; }
#ifdef __clang__
        double &s(void) { return d[0]; }
        double &t(void) { return d[1]; }
        double &p(void) { return d[2]; }
#else
        double &s(void) { return v[0]; }
        double &t(void) { return v[1]; }
        double &p(void) { return v[2]; }
#endif

        double operator[](int i) const { return v[i]; }
#ifdef __clang__
        double &operator[](int i) { return d[i]; }
#else
        double &operator[](int i) { return v[i]; }
#endif

        fvec3d operator+(const fvec3d &ov) const { return v + ov.v; }
        fvec3d &operator+=(const fvec3d &ov) { v += ov.v; return *this; }

        fvec3d operator-(void) const { return -v; }
        fvec3d operator-(const fvec3d &ov) const { return v - ov.v; }
        fvec3d &operator-=(const fvec3d &ov) { v -= ov.v; return *this; }

        fvec3d operator*(const fvec3d &ov) const { return v * ov.v; }
        fvec3d &operator*=(const fvec3d &ov) { v *= ov.v; return *this; }

        fvec3d operator*(double s) const { return v * s; }
        fvec3d &operator*=(double s) { v *= s; return *this; }

        fvec3d operator/(const fvec3d &ov) const { return v / ov.v; }
        fvec3d &operator/=(const fvec3d &ov) { v /= ov.v; return *this; }

        fvec3d operator/(double s) const { return v / s; }
        fvec3d &operator/=(double s) { v /= s; return *this; }

        double dot(const fvec3d &ov) const
        {
            vector_type muled(v * ov.v);
            return muled[0] + muled[1] + muled[2];
        }

        fvec3d cross(const fvec3d &ov) const
        {
            return fvec3d(v[1], v[2], v[0]) * fvec3d(ov.v[2], ov.v[0], ov.v[1]) -
                   fvec3d(v[2], v[0], v[1]) * fvec3d(ov.v[1], ov.v[2], ov.v[0]);
        }

        double length(void) const { return sqrt(dotp(*this, *this)); }

        fvec3d normalized(void) const { return *this / length(); }
        fvec3d &normalize(void) { return *this /= length(); }

        bool operator==(fvec3d &ov) const
        { return v[0] == ov[0] && v[1] == ov[1] && v[2] == ov[2]; }
        bool operator!=(fvec3d &ov) const { return !(*this == ov); }

        operator vec3d(void) const { return vec3d(v[0], v[1], v[2]); }
        operator fvec3(void) const { return fvec3(v[0], v[1], v[2]); }

        operator double *(void) { return d; }
        operator const double *(void) const { return d; }
};
