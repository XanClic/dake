#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec3.hpp directly!
#endif


class fvec3 {
    public:
        typedef float scalar_type;
        typedef float vector_type __attribute__((vector_size(16)));
        typedef int int_vector_type __attribute__((vector_size(16)));

        union {
            vector_type v;
            scalar_type d[3];
        };


        fvec3(float xv, float yv, float zv): v {xv, yv, zv, 0.f} {}
        fvec3(float val): v {val, val, val} {}
        fvec3(const fvec3 &ov): v(ov.v) {}
        fvec3(const fvec4 &ov): v(ov.v) { v[3] = 0.f; }
        fvec3(const vector_type &ov): v(ov) { v[3] = 0.f; }
        fvec3(const vec3 &ov): v {ov.d[0], ov.d[1], ov.d[2], 0.f} {}
        fvec3(void) {}

        static fvec3 zero(void) { return fvec3(0.f, 0.f, 0.f); }
        static fvec3 from_data(const float *f) { return fvec3(f[0], f[1], f[2]); }

        fvec3 &operator=(const fvec3 &ov) { v = ov.v; return *this; }

        float x(void) const { return v[0]; }
        float y(void) const { return v[1]; }
        float z(void) const { return v[2]; }
#ifdef __clang__
        float &x(void) { return d[0]; }
        float &y(void) { return d[1]; }
        float &z(void) { return d[2]; }
#else
        float &x(void) { return v[0]; }
        float &y(void) { return v[1]; }
        float &z(void) { return v[2]; }
#endif
        float r(void) const { return v[0]; }
        float g(void) const { return v[1]; }
        float b(void) const { return v[2]; }
#ifdef __clang__
        float &r(void) { return d[0]; }
        float &g(void) { return d[1]; }
        float &b(void) { return d[2]; }
#else
        float &r(void) { return v[0]; }
        float &g(void) { return v[1]; }
        float &b(void) { return v[2]; }
#endif
        float s(void) const { return v[0]; }
        float t(void) const { return v[1]; }
        float p(void) const { return v[2]; }
#ifdef __clang__
        float &s(void) { return d[0]; }
        float &t(void) { return d[1]; }
        float &p(void) { return d[2]; }
#else
        float &s(void) { return v[0]; }
        float &t(void) { return v[1]; }
        float &p(void) { return v[2]; }
#endif

        float operator[](int i) const { return v[i]; }
#ifdef __clang__
        float &operator[](int i) { return d[i]; }
#else
        float &operator[](int i) { return v[i]; }
#endif

        fvec3 operator+(const fvec3 &ov) const { return v + ov.v; }
        fvec3 &operator+=(const fvec3 &ov) { v += ov.v; return *this; }

        fvec3 operator-(void) const { return -v; }
        fvec3 operator-(const fvec3 &ov) const { return v - ov.v; }
        fvec3 &operator-=(const fvec3 &ov) { v -= ov.v; return *this; }

        fvec3 operator*(const fvec3 &ov) const { return v * ov.v; }
        fvec3 &operator*=(const fvec3 &ov) { v *= ov.v; return *this; }

#ifdef __clang__
        fvec3 operator*(float sv) const { return *this * fvec3(sv); }
        fvec3 &operator*=(float sv) { return *this *= fvec3(sv); }
#else
        fvec3 operator*(float sv) const { return v * sv; }
        fvec3 &operator*=(float sv) { v *= sv; return *this; }
#endif

        fvec3 operator/(const fvec3 &ov) const { return v / ov.v; }
        fvec3 &operator/=(const fvec3 &ov) { v /= ov.v; return *this; }

#ifdef __clang__
        fvec3 operator/(float sv) const { return *this / fvec3(sv); }
        fvec3 &operator/=(float sv) { return *this /= fvec3(sv); }
#else
        fvec3 operator/(float sv) const { return v / sv; }
        fvec3 &operator/=(float sv) { v /= sv; return *this; }
#endif

        float dot(const fvec3 &ov) const
        {
#ifdef __SSE4_1__
            return __builtin_ia32_dpps(v, ov.v, 0x71)[0];
#else
            vector_type muled(v * ov.v);
            return muled[0] + muled[1] + muled[2];
#endif
        }

        fvec3 cross(const fvec3 &ov) const
        {
            return fvec3(v[1], v[2], v[0]) * fvec3(ov[2], ov[0], ov[1]) -
                   fvec3(v[2], v[0], v[1]) * fvec3(ov[1], ov[2], ov[0]);
        }

        float length(void) const { return sqrtf(dotp(*this, *this)); }
        float approx_rcp_length(void) const { return apr_rsqrt(dotp(*this, *this)); }

        fvec3 normalized(void) const { return *this / length(); }
        fvec3 &normalize(void) { return *this /= length(); }

        fvec3 approx_normalized(void) const { return *this * approx_rcp_length(); }
        fvec3 &approx_normalize(void) { return *this *= approx_rcp_length(); }

#ifdef __clang__
        template<int xi, int yi, int zi> fvec3 shuffle(void) const
        { return __builtin_shufflevector(v, v, xi, yi, zi, 3); }
#else
        // Note: When calling this, iv[3] *must* be 3
        fvec3 shuffle(const int_vector_type iv) const
        { return __builtin_shuffle(v, iv); }

        template<int xi, int yi, int zi> fvec3 shuffle(void) const
        { return __builtin_shuffle(v, int_vector_type {xi, yi, zi, 3}); }
#endif

        bool operator==(fvec3 &ov) const
        { return v[0] == ov[0] && v[1] == ov[1] && v[2] == ov[2]; }
        bool operator!=(fvec3 &ov) const { return !(*this == ov); }

        operator vec3(void) const { return vec3(v[0], v[1], v[2]); }

        operator float *(void) { return d; }
        operator const float *(void) const { return d; }
};
