#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec2.hpp directly!
#endif


class fvec2 {
    public:
        typedef float scalar_type;
        typedef float vector_type __attribute__((vector_size(8)));

        union {
            vector_type v;
            scalar_type d[2];
        };


        fvec2(float xv, float yv): v {xv, yv} {}
        fvec2(float val): v {val, val} {}
        fvec2(const fvec2 &ov): v(ov.v) {}
        fvec2(const fvec3 &ov): v {ov.v[0], ov.v[1]} {}
        fvec2(const fvec4 &ov): v {ov.v[0], ov.v[1]} {}
        fvec2(const vector_type &ov): v(ov) {}
        fvec2(const vec2 &ov): v {ov.d[0], ov.d[1]} {}
        fvec2(void) {}

        static fvec2 zero(void) { return fvec2(0.f, 0.f); }
        static fvec2 from_data(const float *f) { return fvec2(f[0], f[1]); }

        fvec2 &operator=(const fvec2 &ov) { v = ov.v; return *this; }

        float x(void) const { return v[0]; }
        float y(void) const { return v[1]; }
#ifdef __clang__
        float &x(void) { return d[0]; }
        float &y(void) { return d[1]; }
#else
        float &x(void) { return v[0]; }
        float &y(void) { return v[1]; }
#endif
        float r(void) const { return v[0]; }
        float g(void) const { return v[1]; }
#ifdef __clang__
        float &r(void) { return d[0]; }
        float &g(void) { return d[1]; }
#else
        float &r(void) { return v[0]; }
        float &g(void) { return v[1]; }
#endif
        float s(void) const { return v[0]; }
        float t(void) const { return v[1]; }
#ifdef __clang__
        float &s(void) { return d[0]; }
        float &t(void) { return d[1]; }
#else
        float &s(void) { return v[0]; }
        float &t(void) { return v[1]; }
#endif

        float operator[](int i) const { return v[i]; }
#ifdef __clang__
        float &operator[](int i) { return d[i]; }
#else
        float &operator[](int i) { return v[i]; }
#endif

        fvec2 operator+(const fvec2 &ov) const { return v + ov.v; }
        fvec2 &operator+=(const fvec2 &ov) { v += ov.v; return *this; }

        fvec2 operator-(void) const { return -v; }
        fvec2 operator-(const fvec2 &ov) const { return v - ov.v; }
        fvec2 &operator-=(const fvec2 &ov) { v -= ov.v; return *this; }

        fvec2 operator*(const fvec2 &ov) const { return v * ov.v; }
        fvec2 &operator*=(const fvec2 &ov) { v *= ov.v; return *this; }

#ifdef __clang__
        fvec2 operator*(float sv) const { return *this * fvec2(sv); }
        fvec2 &operator*=(float sv) { return *this *= fvec2(sv); }
#else
        fvec2 operator*(float sv) const { return v * sv; }
        fvec2 &operator*=(float sv) { v *= sv; return *this; }
#endif

        fvec2 operator/(const fvec2 &ov) const { return v / ov.v; }
        fvec2 &operator/=(const fvec2 &ov) { v /= ov.v; return *this; }

#ifdef __clang__
        fvec2 operator/(float sv) const { return *this / fvec2(sv); }
        fvec2 &operator/=(float sv) { return *this /= fvec2(sv); }
#else
        fvec2 operator/(float sv) const { return v / sv; }
        fvec2 &operator/=(float sv) { v /= sv; return *this; }
#endif

        float dot(const fvec2 &ov) const
        {
            vector_type muled(v * ov.v);
            return muled[0] + muled[1];
        }

        float length(void) const { return sqrtf(dotp(*this, *this)); }
        float approx_rcp_length(void) const { return apr_rsqrt(dotp(*this, *this)); }

        fvec2 normalized(void) const { return *this / length(); }
        fvec2 &normalize(void) { return *this /= length(); }

        fvec2 approx_normalized(void) const { return *this * approx_rcp_length(); }
        fvec2 &approx_normalize(void) { return *this *= approx_rcp_length(); }

        bool operator==(fvec2 &ov) const
        { return v[0] == ov[0] && v[1] == ov[1]; }
        bool operator!=(fvec2 &ov) const { return !(*this == ov); }

        operator vec2(void) const { return vec2(v[0], v[1]); }

        operator float *(void) { return d; }
        operator const float *(void) const { return d; }
};
