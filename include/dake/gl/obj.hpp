#ifndef DAKE__GL__OBJ_HPP
#define DAKE__GL__OBJ_HPP

#include <string>
#include <vector>

#include "dake/math/matrix.hpp"
#include "dake/gl/texture.hpp"
#include "dake/gl/vertex_array.hpp"


namespace dake
{

namespace gl
{

struct obj_material {
    std::string name;
    math::vec4 ambient, diffuse, specular;
    float specular_coefficient;
    int illumination;
    const texture *tex;
};


struct obj_section {
    obj_material material;
    // let's hope the default move constructor works
    std::vector<math::vec3> positions, normals;
    std::vector<math::vec2> tex_coords;

    void normalize_normals(void);

    vertex_array *make_vertex_array(int pos_idx, int txc_idx = -1, int nrm_idx = -1);
};


struct obj {
    obj(std::vector<obj_section> &&s, const math::vec3 &ll, const math::vec3 &ur):
        sections(s), lower_left(ll), upper_right(ur)
    {}

    std::vector<obj_section> sections;
    math::vec3 lower_left, upper_right;
};


obj load_obj(const char *filename);

}

}

#endif
