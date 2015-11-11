#include <cmath>
#include <cstdint>
#include <cstring>

#include <dake/math/matrix.hpp>


namespace dake
{
namespace math
{

template<> mat4 &mat4::translate(const vec3 &vec)
{
    d[12] += vec.x() * d[ 0] + vec.y() * d[ 4] + vec.z() * d[ 8];
    d[13] += vec.x() * d[ 1] + vec.y() * d[ 5] + vec.z() * d[ 9];
    d[14] += vec.x() * d[ 2] + vec.y() * d[ 6] + vec.z() * d[10];
    d[15] += vec.x() * d[ 3] + vec.y() * d[ 7] + vec.z() * d[11];

    return *this;
}

template<> mat4 mat4::translated(const vec3 &vec) const
{
    return mat4(vec4::from_data(&d[ 0]),
                vec4::from_data(&d[ 4]),
                vec4::from_data(&d[ 8]),
                vec4::from_data(&d[12]) +
                vec4::from_data(&d[ 0]) * vec.x() +
                vec4::from_data(&d[ 4]) * vec.y() +
                vec4::from_data(&d[ 8]) * vec.z());
}

template<> mat4 &mat4::rotate(float angle, const vec3 &axis)
{
    if (!angle) {
        return *this;
    }

    float xr = axis.x(), yr = axis.y(), zr = axis.z();
    // this should actually be exact
    float rlen = 1.f / sqrtf(xr * xr + yr * yr + zr * zr);

    xr *= rlen;
    yr *= rlen;
    zr *= rlen;

    float sa = std::sin(angle);
    float ca = std::cos(angle);
    float omc = 1.f - ca;

    float rm[16] = {
        xr * xr * omc +      ca, xr * yr * omc + zr * sa, xr * zr * omc - yr * sa, 0.f,
        yr * xr * omc - zr * sa, yr * yr * omc +      ca, yr * zr * omc + xr * sa, 0.f,
        zr * xr * omc + yr * sa, zr * yr * omc - xr * sa, zr * zr * omc +      ca, 0.f,
                            0.f,                     0.f,                     0.f, 1.f
    };

    return *this *= mat4::from_data(rm);
}

template<> mat4 mat4::rotated(float angle, const vec3 &axis) const
{
    if (!angle) {
        return mat4(*this);
    }

    float xr = axis.x(), yr = axis.y(), zr = axis.z();
    // this should actually be exact
    float rlen = 1.f / sqrtf(xr * xr + yr * yr + zr * zr);

    xr *= rlen;
    yr *= rlen;
    zr *= rlen;

    float sa = std::sin(angle);
    float ca = std::cos(angle);
    float omc = 1.f - ca;

    float rm[16] = {
        xr * xr * omc +      ca, xr * yr * omc + zr * sa, xr * zr * omc - yr * sa, 0.f,
        yr * xr * omc - zr * sa, yr * yr * omc +      ca, yr * zr * omc + xr * sa, 0.f,
        zr * xr * omc + yr * sa, zr * yr * omc - xr * sa, zr * zr * omc +      ca, 0.f,
                            0.f,                     0.f,                     0.f, 1.f
    };

    return *this * mat4::from_data(rm);
}

template<> mat4 &mat4::scale(const vec3 &fac)
{
    d[0] *= fac.x(); d[4] *= fac.y(); d[ 8] *= fac.z();
    d[1] *= fac.x(); d[5] *= fac.y(); d[ 9] *= fac.z();
    d[2] *= fac.x(); d[6] *= fac.y(); d[10] *= fac.z();
    d[3] *= fac.x(); d[7] *= fac.y(); d[11] *= fac.z();

    return *this;
}

template<> mat4 mat4::scaled(const vec3 &fac) const
{
    return mat4(vec4::from_data(&d[ 0]) * fac.x(),
                vec4::from_data(&d[ 4]) * fac.y(),
                vec4::from_data(&d[ 8]) * fac.z(),
                vec4::from_data(&d[12]));
}


template<> float mat2::det(void)
{
    return d[0] * d[3] - d[1] * d[2];
}


template<> float mat3::det(void)
{
    return d[0] * (d[4] * d[8] - d[5] * d[7]) -
           d[3] * (d[1] * d[8] - d[2] * d[7]) +
           d[6] * (d[1] * d[5] - d[2] * d[4]);
}

template<> mat3 &mat3::transposed_invert(void)
{
    float nd[9], rcp_det = 1.f / det();

    nd[0] = rcp_det * (d[4] * d[8] - d[5] * d[7]);
    nd[1] = rcp_det * (d[5] * d[6] - d[3] * d[8]);
    nd[2] = rcp_det * (d[3] * d[7] - d[4] * d[6]);

    nd[3] = rcp_det * (d[2] * d[7] - d[1] * d[8]);
    nd[4] = rcp_det * (d[0] * d[8] - d[2] * d[6]);
    nd[5] = rcp_det * (d[1] * d[6] - d[0] * d[7]);

    nd[6] = rcp_det * (d[1] * d[5] - d[2] * d[4]);
    nd[7] = rcp_det * (d[2] * d[3] - d[0] * d[5]);
    nd[8] = rcp_det * (d[0] * d[4] - d[1] * d[3]);

    memcpy(d, nd, sizeof(d));

    return *this;
}


#define te(a, b, c) d[a] * d[b] * d[c]
#define row(x, a1, a2, a3, a4, a5, a6, a7, a8, a9) d[x] * (te(a1, a2, a3) + te(a4, a5, a6) + te(a7, a8, a9))

template<> float mat4::det(void)
{
    return row( 0,   5, 10, 15,   9, 14,  7,  13,  6, 11) +
           row( 4,   1, 14, 11,   9,  2, 15,  13, 10,  3) +
           row( 8,   1,  6, 15,   5, 14,  3,  13,  2,  7) +
           row(12,   1, 10,  7,   5,  2, 11,   9,  6,  3) -
           row( 0,   5, 14, 11,   9,  6, 15,  13, 10,  7) -
           row( 4,   1, 10, 15,   9, 14,  3,  13,  2, 11) -
           row( 8,   1, 14,  7,   5,  2, 15,  13,  6,  3) -
           row(12,   1,  6, 11,   5, 10,  3,   9,  2,  7);
}

#define le(a1, a2, a3, a4, a5) d[a1] * (d[a2] * d[a3] - d[a4] * d[a5])
#define ele(x, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF) \
        x * (le(a1, a2, a3, a4, a5) + le(a6, a7, a8, a9, aA) + le(aB, aC, aD, aE, aF))

template<> mat4 &mat4::transposed_invert(void)
{
    float dt = 1.f / det();

    float nd[16] = {
        ele(dt,   5, 10, 15, 11, 14,    6, 11, 13,  9, 15,    7,  9, 14, 10, 13),
        ele(dt,   1, 11, 14, 10, 15,    2,  9, 15, 11, 13,    3, 10, 13,  9, 14),
        ele(dt,   1,  6, 15,  7, 14,    2,  7, 13,  5, 15,    3,  5, 14,  6, 13),
        ele(dt,   1,  7, 10,  6, 11,    2,  5, 11,  7,  9,    3,  6,  9,  5, 10),
        ele(dt,   4, 11, 14, 10, 15,    6,  8, 15, 11, 12,    7, 10, 12,  8, 14),
        ele(dt,   0, 10, 15, 11, 14,    2, 11, 12,  8, 15,    3,  8, 14, 10, 12),
        ele(dt,   0,  7, 14,  6, 15,    2,  4, 15,  7, 12,    3,  6, 12,  4, 14),
        ele(dt,   0,  6, 11,  7, 10,    2,  7,  8,  4, 11,    3,  4, 10,  6,  8),
        ele(dt,   4,  9, 15, 11, 13,    5, 11, 12,  8, 15,    7,  8, 13,  9, 12),
        ele(dt,   0, 11, 13,  9, 15,    1,  8, 15, 11, 12,    3,  9, 12,  8, 13),
        ele(dt,   0,  5, 15,  7, 13,    1,  7, 12,  4, 15,    3,  4, 13,  5, 12),
        ele(dt,   0,  7,  9,  5, 11,    1,  4, 11,  7,  8,    3,  5,  8,  4,  9),
        ele(dt,   4, 10, 13,  9, 14,    5,  8, 14, 10, 12,    6,  9, 12,  8, 13),
        ele(dt,   0,  9, 14, 10, 13,    1, 10, 12,  8, 14,    2,  8, 13,  9, 12),
        ele(dt,   0,  6, 13,  5, 14,    1,  4, 14,  6, 12,    2,  5, 12,  4, 13),
        ele(dt,   0,  5, 10,  6,  9,    1,  6,  8,  4, 10,    2,  4,  9,  5,  8)
    };

    memcpy(d, nd, sizeof(nd));

    return *this;
}

}
}
