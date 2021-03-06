#include <dake/container/algorithm.hpp>
#include <dake/container/range.hpp>
#include <dake/helper/function.hpp>
#include <dake/math/matrix.hpp>

#include <cstdio>
#include <vector>


using namespace dake;
using namespace dake::container;
using namespace dake::math;


int main(void)
{
    std::vector<vec2>
    p{
        { 4.f, 14.f },
        { 1.f,  4.f },
        { 4.f,  1.f }
    },
    q{
        { 12.f, 12.f },
        { 16.f,  2.f },
        { 20.f,  2.f }
    };


    auto p_centroid = inject(p, helper::sum) / p.size();
    auto q_centroid = inject(q, helper::sum) / q.size();


    auto p_rel = map(p, [&](const vec2 &pi) { return pi - p_centroid; });
    auto q_rel = map(q, [&](const vec2 &qi) { return qi - q_centroid; });

    auto H = inject(map<mat2>(range<>(0, p.size() - 1), [&](int i) { return q_rel[i] * p_rel[i].transposed(); }), helper::sum) / p.size();

    printf("H:\n");
    printf("( %10g %10g )\n", H[0][0], H[1][0]);
    printf("( %10g %10g )\n", H[0][1], H[1][1]);

    printf("---\n");


    auto U = H.svd_U();
    auto V = H.svd_V().transposed();

    // matX::diagonal(1.f, ..., (V * U).det())
    auto diag = decltype(U)::identity();
    diag[diag.columns - 1][diag.rows - 1] = (V * U).det();

    auto R = V * diag * U;

    if (R.det() < 0.f)
    {
        diag[diag.columns - 1][diag.rows - 1] *= -1.f;
        R = V * diag * U;
    }


    auto t = p_centroid - R * q_centroid;


    printf("R:\n");
    printf("( %10g %10g )\n", R[0][0], R[1][0]);
    printf("( %10g %10g )\n", R[0][1], R[1][1]);

    printf("t:\n");
    printf("( %10g )\n", t[0]);
    printf("( %10g )\n", t[1]);

    printf("---\n");


    printf("p:\n");
    for (auto pi: p)
        printf("( %10g )  ", pi[0]);
    putchar('\n');
    for (auto pi: p)
        printf("( %10g ), ", pi[1]);
    putchar('\n');

    auto q_trans = map(q, [&](const vec2 &qi) { return R * qi + t; });

    printf("q:\n");
    for (auto qi: q_trans)
        printf("( %10g )  ", qi[0]);
    putchar('\n');
    for (auto qi: q_trans)
        printf("( %10g ), ", qi[1]);
    putchar('\n');


    return 0;
}
