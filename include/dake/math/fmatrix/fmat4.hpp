#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fmat4.hpp directly!
#endif


class fmat4 {
    public:
        typedef int v4si __attribute__((vector_size(16)));
        typedef float v4sf __attribute__((vector_size(16)));

        union {
            fvec4 cols[4];
            float d[16];
        };


        fmat4(const fvec4 &c1, const fvec4 &c2,
              const fvec4 &c3, const fvec4 &c4):
            cols {c1, c2, c3, c4}
        {}

        fmat4(const fmat4 &om):
            cols {om.cols[0], om.cols[1], om.cols[2], om.cols[3]}
        {}

        fmat4(const mat4 &om):
            cols {fvec4(om[0]), fvec4(om[1]), fvec4(om[2]), fvec4(om[3])}
        {}

        fmat4(void) {}


        static fmat4 identity(void)
        { return fmat4(fvec4(1.f, 0.f, 0.f, 0.f),
                       fvec4(0.f, 1.f, 0.f, 0.f),
                       fvec4(0.f, 0.f, 1.f, 0.f),
                       fvec4(0.f, 0.f, 0.f, 1.f)); }

        static fmat4 from_data(const float *data)
        { return fmat4(fvec4::from_data(data +  0),
                       fvec4::from_data(data +  4),
                       fvec4::from_data(data +  8),
                       fvec4::from_data(data + 12)); }


        fmat4 &operator=(const fmat4 &om)
        { for (int i = 0; i < 4; i++) { cols[i] = om[i]; } return *this; }


        const fvec4 &operator[](int i) const { return cols[i]; }
        fvec4 &operator[](int i) { return cols[i]; }


        fvec4 operator*(const fvec4 &v) const
        { return v[0] * cols[0] + v[1] * cols[1] +
                 v[2] * cols[2] + v[3] * cols[3]; }

        fmat4 operator*(const fmat4 &om) const
        { return fmat4(*this * om[0], *this * om[1],
                       *this * om[2], *this * om[3]); }

        fmat4 &operator*=(const fmat4 &om)
        {
            fvec4 c1 = *this * om[0]; fvec4 c2 = *this * om[1];
            fvec4 c3 = *this * om[2]; fvec4 c4 = *this * om[3];
            cols[0] = c1; cols[1] = c2; cols[2] = c3; cols[3] = c4;
            return *this;
        }


        fmat4 translated(const fvec3 &v) const
        { return fmat4(cols[0], cols[1], cols[2],
                       v[0] * cols[0] + v[1] * cols[1] +
                       v[2] * cols[2] +        cols[3]); }

        fmat4 &translate(const fvec3 &v)
        { cols[3] += v[0] * cols[0] + v[1] * cols[1] + v[2] * cols[2];
          return *this; }

        static fmat4 translation(const fvec3 &v)
        { return fmat4(fvec4(1.f, 0.f, 0.f, 0.f),
                       fvec4(0.f, 1.f, 0.f, 0.f),
                       fvec4(0.f, 0.f, 1.f, 0.f),
                       fvec4::direction(v)); }


        fmat4 scaled(const fvec3 &v) const
        { return fmat4(v[0] * cols[0], v[1] * cols[1],
                       v[2] * cols[2],        cols[3]); }

        fmat4 &scale(const fvec3 &v)
        { cols[0] *= v[0]; cols[1] *= v[1]; cols[2] *= v[2]; return *this; }

        static fmat4 scaling(const fvec3 &v)
        { return fmat4(fvec4(v[0], 0.f, 0.f, 0.f),
                       fvec4(0.f, v[1], 0.f, 0.f),
                       fvec4(0.f, 0.f, v[2], 0.f),
                       fvec4(0.f, 0.f, 0.f,  1.f)); }


        fmat4 rotated_normalized(float angle, const fvec3 &axis) const
        { return *this * fmat4::rotation_normalized(angle, axis); }

        fmat4 &rotate_normalized(float angle, const fvec3 &axis)
        { return *this *= fmat4::rotation_normalized(angle, axis); }

        static fmat4 rotation_normalized(float angle, const fvec3 &axis)
        {
            float sa = sinf(angle), ca = cosf(angle), omc = 1.f - ca;
            fvec4 axis_omc = omc * fvec4::direction(axis);
            return fmat4(axis[0] * axis_omc + fvec4(ca, axis[2] * sa, -axis[1] * sa, 0.f),
                         axis[1] * axis_omc + fvec4(-axis[2] * sa, ca, axis[0] * sa, 0.f),
                         axis[2] * axis_omc + fvec4(axis[1] * sa, -axis[0] * sa, ca, 0.f),
                         fvec4(0.f, 0.f, 0.f, 1.f));
        }


        fmat4 rotated(float angle, const fvec3 &axis) const
        { return rotated_normalized(angle, axis.normalized()); }

        fmat4 rotate(float angle, const fvec3 &axis)
        { return rotate_normalized(angle, axis.normalized()); }

        static fmat4 rotation(float angle, const fvec3 &axis)
        { return fmat4::rotation_normalized(angle, axis.normalized()); }


        static fmat4 projection(float fovy, float aspect, float zn, float zf)
        {
            float f = 1.f / tanf(fovy / 2.f);
            return fmat4(fvec4(f / aspect, 0.f, 0.f, 0.f),
                         fvec4(0.f, f, 0.f, 0.f),
                         fvec4(0.f, 0.f, (zn + zf) / (zn - zf), -1.f),
                         fvec4(0.f, 0.f, 2.f * (zn * zf) / (zn - zf), 0.f));
        }


        static fmat4 orthographic(float left, float right, float top,
                                  float bottom, float near, float far)
        {
            return fmat4(fvec4(2.f / (right - left), 0.f, 0.f, 0.f),
                         fvec4(0.f, 2.f / (top - bottom), 0.f, 0.f),
                         fvec4(0.f, 0.f, 2.f / (near - far), 0.f),
                         fvec4((right + left) / (left - right),
                               (top + bottom) / (bottom - top),
                               (far + near) / (near - far),
                               1.f));
        }


        fmat4 transposed(void) const
        { return fmat4(fvec4(cols[0][0], cols[1][0], cols[2][0], cols[3][0]),
                       fvec4(cols[0][1], cols[1][1], cols[2][1], cols[3][1]),
                       fvec4(cols[0][2], cols[1][2], cols[2][2], cols[3][2]),
                       fvec4(cols[0][3], cols[1][3], cols[2][3], cols[3][3])); }

        fmat4 &transpose(void)
        { return *this = this->transposed(); }


        float det(void) const
        {
            fvec4 c2 = cols[1], c3 = cols[2], c4 = cols[3];

            fvec4 r1 = c2.shuffle<1, 0, 0, 0>() *
                       (c3.shuffle<2, 3, 1, 2>() *
                        c4.shuffle<3, 2, 3, 1>() -
                        c3.shuffle<3, 2, 3, 1>() *
                        c4.shuffle<2, 3, 1, 2>())
                     + c2.shuffle<2, 2, 1, 1>() *
                       (c3.shuffle<3, 0, 3, 0>() *
                        c4.shuffle<1, 3, 0, 2>() -
                        c3.shuffle<1, 3, 0, 2>() *
                        c4.shuffle<3, 0, 3, 0>())
                     + c2.shuffle<3, 3, 3, 2>();
                       (c3.shuffle<1, 2, 0, 1>() *
                        c4.shuffle<2, 0, 1, 0>() -
                        c3.shuffle<2, 0, 1, 0>() *
                        c4.shuffle<1, 2, 0, 1>());

            return dotp(r1, cols[0]);
        }


        fmat4 transposed_inverse(void) const
        {
            fvec4 c1 = cols[0], c2 = cols[1], c3 = cols[2], c4 = cols[3];

            fvec4 c1_1000 = c1.shuffle<1, 0, 0, 0>();
            fvec4 c1_2211 = c1.shuffle<2, 2, 1, 1>();
            fvec4 c1_3332 = c1.shuffle<3, 3, 3, 2>();

            fvec4 c2_1000 = c2.shuffle<1, 0, 0, 0>();
            fvec4 c2_2211 = c2.shuffle<2, 2, 1, 1>();
            fvec4 c2_3332 = c2.shuffle<3, 3, 3, 2>();

            fvec4 c2_1201 = c2.shuffle<1, 2, 0, 1>();
            fvec4 c2_1302 = c2.shuffle<1, 3, 0, 2>();
            fvec4 c2_2010 = c2.shuffle<2, 0, 1, 0>();
            fvec4 c2_2312 = c2.shuffle<2, 3, 1, 2>();
            fvec4 c2_3030 = c2.shuffle<3, 0, 3, 0>();
            fvec4 c2_3231 = c2.shuffle<3, 2, 3, 1>();

            fvec4 c3_1201 = c3.shuffle<1, 2, 0, 1>();
            fvec4 c3_1302 = c3.shuffle<1, 3, 0, 2>();
            fvec4 c3_2010 = c3.shuffle<2, 0, 1, 0>();
            fvec4 c3_2312 = c3.shuffle<2, 3, 1, 2>();
            fvec4 c3_3030 = c3.shuffle<3, 0, 3, 0>();
            fvec4 c3_3231 = c3.shuffle<3, 2, 3, 1>();

            fvec4 c4_1201 = c4.shuffle<1, 2, 0, 1>();
            fvec4 c4_1302 = c4.shuffle<1, 3, 0, 2>();
            fvec4 c4_2010 = c4.shuffle<2, 0, 1, 0>();
            fvec4 c4_2312 = c4.shuffle<2, 3, 1, 2>();
            fvec4 c4_3030 = c4.shuffle<3, 0, 3, 0>();
            fvec4 c4_3231 = c4.shuffle<3, 2, 3, 1>();

            fvec4 oc1 = (c2_1000 * (c3_2312 * c4_3231 - c3_3231 * c4_2312)
                       + c2_2211 * (c3_3030 * c4_1302 - c3_1302 * c4_3030)
                       + c2_3332 * (c3_1201 * c4_2010 - c3_2010 * c4_1201));

            fvec4 oc2 = (c1_1000 * (c4_2312 * c3_3231 - c4_3231 * c3_2312)
                       + c1_2211 * (c4_3030 * c3_1302 - c4_1302 * c3_3030)
                       + c1_3332 * (c4_1201 * c3_2010 - c4_2010 * c3_1201));

            fvec4 oc3 = (c1_1000 * (c2_2312 * c4_3231 - c2_3231 * c4_2312)
                       + c1_2211 * (c2_3030 * c4_1302 - c2_1302 * c4_3030)
                       + c1_3332 * (c2_1201 * c4_2010 - c2_2010 * c4_1201));

            fvec4 oc4 = (c1_1000 * (c3_2312 * c2_3231 - c3_3231 * c2_2312)
                       + c1_2211 * (c3_3030 * c2_1302 - c3_1302 * c2_3030)
                       + c1_3332 * (c3_1201 * c2_2010 - c3_2010 * c2_1201));

            float id = 1.f / dotp(oc1, c1);
            return fmat4(id * oc1, id * oc2, id * oc3, id * oc4);
        }

        fmat4 &transposed_invert(void)
        { return *this = transposed_inverse(); }


        fmat4 inverse(void) const
        { return transposed_inverse().transposed(); }

        fmat4 &invert(void)
        { transposed_invert(); return transpose(); }


        operator mat4(void) const
        { return mat4(cols[0], cols[1], cols[2], cols[3]); }
};
