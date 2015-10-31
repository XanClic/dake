#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec3d-noavx.hpp directly!
#endif


class fvec3d {
    public:
        typedef double scalar_type;
        typedef double vector_type __attribute__((vector_size(16)));

        union {
            struct {
                vector_type v;
                scalar_type s;
            } __attribute__((packed)) vs;
            scalar_type d[3];
        };


        fvec3d(double xv, double yv, double zv): vs {{xv, yv}, zv} {}
        fvec3d(double val): vs {{val, val}, val} {}
        fvec3d(const fvec3d &ov): vs {ov.vs.v, ov.vs.s} {}
        fvec3d(const fvec4d &ov): vs {{ov.v[0][0], ov.v[0][1]}, ov.v[1][0]} {}
        fvec3d(const fvec3 &ov): vs {{ov.v[0], ov.v[1]}, ov.v[2]} {}
        fvec3d(const fvec4 &ov): vs {{ov.v[0], ov.v[1]}, ov.v[2]} {}
        fvec3d(const vector_type &xyv, double zv): vs {xyv, zv} {}
        fvec3d(const vec3d &ov): vs {{ov.d[0], ov.d[1]}, ov.d[2]} {}
        fvec3d(void) {}

        static fvec3d zero(void) { return fvec3d(0., 0., 0.); }
#ifdef __clang__
        static fvec3d from_data(const double *f) { return fvec3d(f[0], f[1], f[2]); }
#else
        static fvec3d from_data(const double *f)
        { return fvec3d(__builtin_ia32_loadupd(f), f[2]); }
#endif

        fvec3d &operator=(const fvec3d &ov) { vs.v = ov.vs.v; vs.s = ov.vs.s; return *this; }

        double x(void) const { return vs.v[0]; }
        double y(void) const { return vs.v[1]; }
        double z(void) const { return vs.s; }
#ifdef __clang__
        double &x(void) { return d[0]; }
        double &y(void) { return d[1]; }
#else
        double &x(void) { return vs.v[0]; }
        double &y(void) { return vs.v[1]; }
#endif
        double &z(void) { return d[2]; }
        double r(void) const { return vs.v[0]; }
        double g(void) const { return vs.v[1]; }
        double b(void) const { return vs.s; }
#ifdef __clang__
        double &r(void) { return d[0]; }
        double &g(void) { return d[1]; }
#else
        double &r(void) { return vs.v[0]; }
        double &g(void) { return vs.v[1]; }
#endif
        double &b(void) { return d[2]; }
        double s(void) const { return vs.v[0]; }
        double t(void) const { return vs.v[1]; }
        double p(void) const { return vs.s; }
#ifdef __clang__
        double &s(void) { return d[0]; }
        double &t(void) { return d[1]; }
#else
        double &s(void) { return vs.v[0]; }
        double &t(void) { return vs.v[1]; }
#endif
        double &p(void) { return d[2]; }

        double operator[](int i) const { return d[i]; }
        double &operator[](int i) { return d[i]; }

        fvec3d operator+(const fvec3d &ov) const { return fvec3d(vs.v + ov.vs.v, vs.s + ov.vs.s); }
        fvec3d &operator+=(const fvec3d &ov) { vs.v += ov.vs.v; vs.s += ov.vs.s; return *this; }

        fvec3d operator-(void) const { return fvec3d(-vs.v, -vs.s); }
        fvec3d operator-(const fvec3d &ov) const { return fvec3d(vs.v - ov.vs.v, vs.s - ov.vs.s); }
        fvec3d &operator-=(const fvec3d &ov) { vs.v -= ov.vs.v; vs.s -= ov.vs.s; return *this; }

        fvec3d operator*(const fvec3d &ov) const { return fvec3d(vs.v * ov.vs.v, vs.s * ov.vs.s); }
        fvec3d &operator*=(const fvec3d &ov) { vs.v *= ov.vs.v; vs.s *= ov.vs.s; return *this; }

#ifdef __clang__
        fvec3d operator*(double sv) const { return *this * fvec3d(sv); }
        fvec3d &operator*=(double sv) { return *this *= fvec3d(sv); }
#else
        fvec3d operator*(double sv) const { return fvec3d(vs.v * sv, vs.s * sv); }
        fvec3d &operator*=(double sv) { vs.v *= sv; vs.s *= sv; return *this; }
#endif

        fvec3d operator/(const fvec3d &ov) const { return fvec3d(vs.v / ov.vs.v, vs.s / ov.vs.s); }
        fvec3d &operator/=(const fvec3d &ov) { vs.v /= ov.vs.v; vs.s /= ov.vs.s; return *this; }

#ifdef __clang__
        fvec3d operator/(double sv) const { return *this / fvec3d(sv); }
        fvec3d &operator/=(double sv) { return *this /= fvec3d(sv); }
#else
        fvec3d operator/(double sv) const { return fvec3d(vs.v / sv, vs.s / sv); }
        fvec3d &operator/=(double sv) { vs.v /= sv; vs.s /= sv; return *this; }
#endif

        double dot(const fvec3d &ov) const
        {
            vector_type muled(vs.v * ov.vs.v);
            return muled[0] + muled[1] + vs.s * ov.vs.s;
        }

        fvec3d cross(const fvec3d &ov) const
        {
            return fvec3d(vs.v[1], vs.s, vs.v[0]) * fvec3d(ov.vs.s, ov.vs.v[0], ov.vs.v[1]) -
                   fvec3d(vs.s, vs.v[0], vs.v[1]) * fvec3d(ov.vs.v[1], ov.vs.s, ov.vs.v[0]);
        }

        double length(void) const { return sqrt(dotp(*this, *this)); }

        fvec3d normalized(void) const { return *this / length(); }
        fvec3d &normalize(void) { return *this /= length(); }

        bool operator==(fvec3d &ov) const
        { return vs.v[0] == ov[0] && vs.v[1] == ov[1] && vs.s == ov[2]; }
        bool operator!=(fvec3d &ov) const { return !(*this == ov); }

        operator vec3d(void) const { return vec3d(vs.v[0], vs.v[1], vs.s); }
        operator fvec3(void) const { return fvec3(vs.v[0], vs.v[1], vs.s); }

        operator double *(void) { return d; }
        operator const double *(void) const { return d; }
};
