#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fmat3.hpp directly!
#endif


class fmat3 {
    public:
        fvec3 cols[3];


        fmat3(const fvec3 &c1, const fvec3 &c2, const fvec3 &c3):
            cols {c1, c2, c3}
        {}

        fmat3(const fmat3 &om):
            cols {om.cols[0], om.cols[1], om.cols[2]}
        {}

        fmat3(const fmat4 &om):
            cols {om.cols[0], om.cols[1], om.cols[2]}
        {}

        fmat3(const mat3 &om):
            cols {fvec3(om[0]), fvec3(om[1]), fvec3(om[2])}
        {}

        fmat3(void) {}


        static fmat3 identity(void)
        { return fmat3(fvec3(1.f, 0.f, 0.f),
                       fvec3(0.f, 1.f, 0.f),
                       fvec3(0.f, 0.f, 1.f)); }

        static fmat3 from_data(const float *data)
        { return fmat3(fvec3::from_data(data + 0),
                       fvec3::from_data(data + 3),
                       fvec3::from_data(data + 6)); }


        fmat3 &operator=(const fmat3 &om)
        { for (int i = 0; i < 3; i++) { cols[i] = om[i]; } return *this; }


        const fvec3 &operator[](int i) const { return cols[i]; }
        fvec3 &operator[](int i) { return cols[i]; }


        fvec3 operator*(const fvec3 &v) const
        { return v[0] * cols[0] + v[1] * cols[1] + v[2] * cols[2]; }

        fmat3 operator*(const fmat3 &om) const
        { return fmat3(*this * om[0], *this * om[1], *this * om[2]); }

        fmat3 &operator*=(const fmat3 &om)
        {
            fvec3 c1 = *this * om[0]; fvec3 c2 = *this * om[1];
            fvec3 c3 = *this * om[2];
            cols[0] = c1; cols[1] = c2; cols[2] = c3;
            return *this;
        }


        fmat3 scaled(const fvec3 &v) const
        { return fmat3(v[0] * cols[0], v[1] * cols[1], v[2] * cols[2]); }

        fmat3 &scale(const fvec3 &v)
        { cols[0] *= v[0]; cols[1] *= v[1]; cols[2] *= v[2]; return *this; }

        static fmat3 scaling(const fvec3 &v)
        { return fmat3(fvec3(v[0], 0.f, 0.f),
                       fvec3(0.f, v[1], 0.f),
                       fvec3(0.f, 0.f, v[2])); }


        fmat3 rotated_normalized(float angle, const fvec3 &axis) const
        { return *this * fmat3::rotation_normalized(angle, axis); }

        fmat3 &rotate_normalized(float angle, const fvec3 &axis)
        { return *this *= fmat3::rotation_normalized(angle, axis); }

        static fmat3 rotation_normalized(float angle, const fvec3 &axis)
        {
            float sa = sinf(angle), ca = cosf(angle), omc = 1.f - ca;
            fvec3 axis_omc = omc * axis;
            return fmat3(axis[0] * axis_omc + fvec3(ca, axis[2] * sa, -axis[1] * sa),
                         axis[1] * axis_omc + fvec3(-axis[2] * sa, ca, axis[0] * sa),
                         axis[2] * axis_omc + fvec3(axis[1] * sa, -axis[0] * sa, ca));
        }


        fmat3 rotated(float angle, const fvec3 &axis) const
        { return rotated_normalized(angle, axis.normalized()); }

        fmat3 rotate(float angle, const fvec3 &axis)
        { return rotate_normalized(angle, axis.normalized()); }

        static fmat3 rotation(float angle, const fvec3 &axis)
        { return fmat3::rotation_normalized(angle, axis.normalized()); }


        fmat3 transposed(void) const
        { return fmat3(fvec3(cols[0][0], cols[1][0], cols[2][0]),
                       fvec3(cols[0][1], cols[1][1], cols[2][1]),
                       fvec3(cols[0][2], cols[1][2], cols[2][2])); }

        fmat3 &transpose(void)
        { return *this = this->transposed(); }


        float det(void) const
        {
            fvec3 c1 = cols[0], c2 = cols[1], c3 = cols[2];

            return dotp(c3.shuffle<2, 1, 0>(), c2.shuffle<1, 0, 2>() * c1.shuffle<0, 2, 1>()) -
                   dotp(c1,                    c2.shuffle<2, 0, 1>() * c3.shuffle<1, 2, 0>());
        }


        fmat3 transposed_inverse(void) const
        {
            fvec3 c1 = cols[0], c2 = cols[1], c3 = cols[2];

            fvec3 c1_120 = c1.shuffle<1, 2, 0>();
            fvec3 c1_201 = c1.shuffle<2, 0, 1>();

            fvec3 c2_120 = c2.shuffle<1, 2, 0>();
            fvec3 c2_201 = c2.shuffle<2, 0, 1>();

            fvec3 c3_120 = c3.shuffle<1, 2, 0>();
            fvec3 c3_201 = c3.shuffle<2, 0, 1>();

            fvec3 oc1 = c2_120 * c3_201 - c3_120 * c2_201;
            fvec3 oc2 = c3_120 * c1_201 - c1_120 * c3_201;
            fvec3 oc3 = c1_120 * c2_201 - c2_120 * c1_201;

            float id = 1.f / det();
            return fmat3(id * oc1, id * oc2, id * oc3);
        }

        fmat3 &transposed_invert(void)
        { return *this = transposed_invert(); }


        fmat3 inverse(void) const
        { return transposed_inverse().transposed(); }

        fmat3 &invert(void)
        { transposed_invert(); return transpose(); }


        operator mat3(void) const { return mat3(cols[0], cols[1], cols[2]); }
};

