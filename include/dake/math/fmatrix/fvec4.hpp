#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec4.hpp directly!
#endif


class fvec4 {
    public:
        typedef float scalar_type;
        typedef float vector_type __attribute__((vector_size(16)));
        typedef int int_vector_type __attribute__((vector_size(16)));

        union {
            vector_type v;
            scalar_type d[4];
        };


        fvec4(float xv, float yv, float zv, float wv): v {xv, yv, zv, wv} {}
        fvec4(float val): v {val, val, val, val} {}
        fvec4(const fvec4 &ov): v(ov.v) {}
        fvec4(const vector_type &ov): v(ov) {}
        fvec4(const vec4 &ov): v {ov.d[0], ov.d[1], ov.d[2], ov.d[3]} {}
        fvec4(void) {}

        static fvec4 zero(void) { return fvec4(0.f, 0.f, 0.f, 0.f); }
#ifdef __clang__
        static fvec4 from_data(const float *f) { return fvec4(f[0], f[1], f[2], f[3]); }
#else
        static fvec4 from_data(const float *f) { return __builtin_ia32_loadups(f); }
#endif
        static fvec4 direction(const fvec3 &ov);
        static fvec4 position(const fvec3 &ov);

        fvec4 &operator=(const fvec4 &ov) { v = ov.v; return *this; }

        float x(void) const { return v[0]; }
        float y(void) const { return v[1]; }
        float z(void) const { return v[2]; }
        float w(void) const { return v[3]; }
#ifdef __clang__
        float &x(void) { return d[0]; }
        float &y(void) { return d[1]; }
        float &z(void) { return d[2]; }
        float &w(void) { return d[3]; }
#else
        float &x(void) { return v[0]; }
        float &y(void) { return v[1]; }
        float &z(void) { return v[2]; }
        float &w(void) { return v[3]; }
#endif
        float r(void) const { return v[0]; }
        float g(void) const { return v[1]; }
        float b(void) const { return v[2]; }
        float a(void) const { return v[3]; }
#ifdef __clang__
        float &r(void) { return d[0]; }
        float &g(void) { return d[1]; }
        float &b(void) { return d[2]; }
        float &a(void) { return d[3]; }
#else
        float &r(void) { return v[0]; }
        float &g(void) { return v[1]; }
        float &b(void) { return v[2]; }
        float &a(void) { return v[3]; }
#endif
        float s(void) const { return v[0]; }
        float t(void) const { return v[1]; }
        float p(void) const { return v[2]; }
        float q(void) const { return v[3]; }
#ifdef __clang__
        float &s(void) { return d[0]; }
        float &t(void) { return d[1]; }
        float &p(void) { return d[2]; }
        float &q(void) { return d[3]; }
#else
        float &s(void) { return v[0]; }
        float &t(void) { return v[1]; }
        float &p(void) { return v[2]; }
        float &q(void) { return v[3]; }
#endif

        float operator[](int i) const { return v[i]; }
#ifdef __clang__
        float &operator[](int i) { return d[i]; }
#else
        float &operator[](int i) { return v[i]; }
#endif

        fvec4 operator+(const fvec4 &ov) const { return v + ov.v; }
        fvec4 &operator+=(const fvec4 &ov) { v += ov.v; return *this; }

        fvec4 operator-(void) const { return -v; }
        fvec4 operator-(const fvec4 &ov) const { return v - ov.v; }
        fvec4 &operator-=(const fvec4 &ov) { v -= ov.v; return *this; }

        fvec4 operator*(const fvec4 &ov) const { return v * ov.v; }
        fvec4 &operator*=(const fvec4 &ov) { v *= ov.v; return *this; }

#ifdef __clang__
        fvec4 operator*(float sv) const { return *this * fvec4(sv); }
        fvec4 &operator*=(float sv) { return *this *= fvec4(sv); }
#else
        fvec4 operator*(float sv) const { return v * sv; }
        fvec4 &operator*=(float sv) { v *= sv; return *this; }
#endif

        fvec4 operator/(const fvec4 &ov) const { return v / ov.v; }
        fvec4 &operator/=(const fvec4 &ov) { v /= ov.v; return *this; }

#ifdef __clang__
        fvec4 operator/(float sv) const { return *this / fvec4(sv); }
        fvec4 &operator/=(float sv) { return *this /= fvec4(sv); }
#else
        fvec4 operator/(float sv) const { return v / sv; }
        fvec4 &operator/=(float sv) { v /= sv; return *this; }
#endif

        float dot(const fvec4 &ov) const
        {
#ifdef __SSE4_1__
            return __builtin_ia32_dpps(v, ov.v, 0xf1)[0];
#else
            vector_type muled(v * ov.v);
            return muled[0] + muled[1] + muled[2] + muled[3];
#endif
        }

        float length(void) const { return sqrtf(dotp(*this, *this)); }
        float approx_rcp_length(void) const { return apr_rsqrt(dotp(*this, *this)); }

        fvec4 normalized(void) const { return *this / length(); }
        fvec4 &normalize(void) { return *this /= length(); }

        fvec4 approx_normalized(void) const { return *this * approx_rcp_length(); }
        fvec4 &approx_normalize(void) { return *this *= approx_rcp_length(); }

#ifdef __clang__
        template<int xi, int yi, int zi, int wi> fvec4 shuffle(void) const
        { return __builtin_shufflevector(v, v, xi, yi, zi, wi); }
#else
        fvec4 shuffle(const int_vector_type iv) const
        { return __builtin_shuffle(v, iv); }

        template<int xi, int yi, int zi, int wi> fvec4 shuffle(void) const
        { return __builtin_shuffle(v, int_vector_type {xi, yi, zi, wi}); }
#endif

        bool operator==(fvec4 &ov) const
        { return v[0] == ov[0] && v[1] == ov[1] && v[2] == ov[2] && v[3] == ov[3]; }
        bool operator!=(fvec4 &ov) const { return !(*this == ov); }

        operator vec4(void) const { return vec4(v[0], v[1], v[2], v[3]); }

        operator float *(void) { return d; }
        operator const float *(void) const { return d; }
};
