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
    std::vector<vec2> p, q;

    p.push_back(vec2(4.f, 14.f));
    p.push_back(vec2(1.f,  4.f));
    p.push_back(vec2(4.f,  1.f));

    q.push_back(vec2(12.f, 12.f));
    q.push_back(vec2(16.f,  2.f));
    q.push_back(vec2(20.f,  2.f));


    auto p_centroid = inject(p, vec2::zero(), helper::sum) / p.size();
    auto q_centroid = inject(q, vec2::zero(), helper::sum) / q.size();


    auto p_rel = map(p, [&](const vec2 &pi) { return pi - p_centroid; });
    auto q_rel = map(q, [&](const vec2 &qi) { return qi - q_centroid; });

    auto H = inject(map<mat2>(range<>(0, p.size() - 1), [&](int i) { return q_rel[i] * p_rel[i].transposed(); }), mat2::zero(), helper::sum) / p.size();

    printf("H:\n");
    printf("%10g %10g\n", H[0][0], H[1][0]);
    printf("%10g %10g\n", H[0][1], H[1][1]);

    return 0;
}
