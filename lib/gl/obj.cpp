#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <libgen.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "dake/gl/find_resource.hpp"
#include "dake/gl/obj.hpp"
#include "dake/gl/vertex_array.hpp"
#include "dake/gl/vertex_attrib.hpp"
#include "dake/math/matrix.hpp"


static dake::gl::obj_material default_mat = {
    "__DEFAULT__",
    dake::math::vec4(0.f, 0.f, 0.f, 1.f),
    dake::math::vec4(1.f, 1.f, 1.f, 1.f),
    dake::math::vec4(1.f, 1.f, 1.f, 1.f), 100.f,
    2,
    nullptr
};


dake::gl::obj dake::gl::load_obj(const char *filename)
{
    std::string fname_str = dake::gl::find_resource_filename(filename).c_str();

    std::ifstream file(fname_str);
    if (!file.is_open()) {
        // let's just hope errno is set
        throw std::invalid_argument(std::string("Could not open OBJ file: ") + strerror(errno));
    }

    std::vector<dake::math::vec3> mesh_positions, mesh_normals, mesh_tex_coords;

    std::vector<dake::math::vec3> positions, normals, tex_coords;
    dake::math::vec3 lower_left ( HUGE_VALF,  HUGE_VALF,  HUGE_VALF);
    dake::math::vec3 upper_right(-HUGE_VALF, -HUGE_VALF, -HUGE_VALF);;

    std::vector<dake::gl::obj_section> sections;

    std::vector<dake::gl::obj_material> materials;


    std::string fname_copy = fname_str;
    std::string obj_dirname(dirname(const_cast<char *>(fname_copy.c_str()))); // no risk no fun


    std::string str_line;
    while (std::getline(file, str_line, '\n')) {
        std::stringstream line(str_line);

        std::string deftype;
        line >> deftype;

        if (deftype == "v") {
            dake::math::vec3 position;
            line >> position.x() >> position.y() >> position.z();
            positions.push_back(position);
        } else if (deftype == "vn") {
            dake::math::vec3 normal;
            line >> normal.x() >> normal.y() >> normal.z();
            normals.push_back(normal);
        } else if (deftype == "vt") {
            dake::math::vec2 tex_coord;
            line >> tex_coord.s() >> tex_coord.t();
            tex_coords.push_back(tex_coord);
        } else if (deftype == "f") {
            if (sections.empty()) {
                sections.emplace_back();
                sections.back().material = default_mat;
            }

            dake::math::vec3 *pos[3] = {nullptr}, *nrm[3] = {nullptr}, *txc[3] = {nullptr};

            std::string entry;
            for (int corner = 0; std::getline(line, entry, ' '); corner++) {
                if (entry.empty()) {
                    corner--;
                    continue;
                }

                if (corner >= 3) {
                    // TODO
                    throw std::runtime_error("Could not load OBJ mesh: Not triangualized");
                }

                std::stringstream entry_stream(entry);
                std::string indexstr;
                for (int i = 0; std::getline(entry_stream, indexstr, '/'); i++) {
                    int index;
                    char *end;

                    if (indexstr.empty()) {
                        index = 0;
                    } else {
                        index = strtol(indexstr.c_str(), &end, 10);
                        if (*end) {
                            throw std::runtime_error("Could not load OBJ mesh: Invalid index given, is not a number");
                        }
                    }

                    if (index > 0) {
                        switch (i) {
                            case 0: pos[corner] = &positions[index - 1]; break;
                            case 1: txc[corner] = &tex_coords[index - 1]; break;
                            case 2: nrm[corner] = &normals[index - 1]; break;
                            default: throw std::runtime_error("Could not load OBJ mesh: Invalid number of vertex attributes given");
                        }
                    }
                }

                if (!pos[corner]) {
                    throw std::runtime_error("Could not load OBJ mesh: No vertex position given");
                }
            }

            bool backwards = false;
            if (nrm[0] || nrm[1] || nrm[2]) {
                dake::math::vec3 *normal = nrm[0] ? nrm[0] : nrm[1] ? nrm[1] : nrm[2];

                dake::math::vec3 nat_norm = (*pos[1] - *pos[0]).cross(*pos[2] - *pos[0]);
                backwards = nat_norm.dot(*normal) < 0.f;
            }

            for (int i = backwards ? 2 : 0; backwards ? i >= 0 : i < 3; backwards ? i-- : i++) {
                for (int j = 0; j < 3; j++) {
                    if ((*pos[i])[j] < lower_left[j]) {
                        lower_left[j] = (*pos[i])[j];
                    }
                    if ((*pos[i])[j] > upper_right[j]) {
                        upper_right[j] = (*pos[i])[j];
                    }
                }

                sections.back().positions.push_back(*pos[i]);
                if (nrm[i]) {
                    sections.back().normals.push_back(*nrm[i]);
                }
                if (txc[i]) {
                    sections.back().tex_coords.push_back(*txc[i]);
                }
            }
        } else if (deftype == "mtllib") {
            std::string remaining;
            line >> remaining;

            if (remaining[0] != '/') {
                remaining = obj_dirname + "/" + remaining;
            }

            std::ifstream mtllib(remaining.c_str());
            if (!mtllib.is_open()) {
                throw std::runtime_error(std::string("Could not load OBJ material library: ") + strerror(errno));
            }

            std::string mtl_str_line;
            while (std::getline(mtllib, mtl_str_line, '\n')) {
                std::stringstream mtl_line(mtl_str_line);

                std::string mtl_deftype;
                mtl_line >> mtl_deftype;

                if (mtl_deftype == "newmtl") {
                    materials.emplace_back();
                    mtl_line >> materials.back().name;
                    materials.back().ambient.a() = 1.f;
                    materials.back().diffuse.a() = 1.f;
                    materials.back().specular.a() = 1.f;
                    materials.back().tex = nullptr;
                } else if (mtl_deftype == "Ka") {
                    mtl_line >> materials.back().ambient.r()
                             >> materials.back().ambient.g()
                             >> materials.back().ambient.b();
                } else if (mtl_deftype == "Kd") {
                    mtl_line >> materials.back().diffuse.r()
                             >> materials.back().diffuse.g()
                             >> materials.back().diffuse.b();
                } else if (mtl_deftype == "Ks") {
                    mtl_line >> materials.back().specular.r()
                             >> materials.back().specular.g()
                             >> materials.back().specular.b();
                } else if (mtl_deftype == "Ni") {
                    mtl_line >> materials.back().specular_coefficient;
                } else if ((mtl_deftype == "d") || (mtl_deftype == "Tr")) {
                    float alpha;
                    mtl_line >> alpha;
                    materials.back().ambient.a() = alpha;
                    materials.back().diffuse.a() = alpha;
                    materials.back().specular.a() = alpha;
                } else if (mtl_deftype == "illum") {
                    mtl_line >> materials.back().illumination;
                } else if (mtl_deftype == "map_Kd") {
                    std::string fname;
                    mtl_line >> fname;
                    if (fname != ".") {
                        if (fname[0] != '/') {
                            fname = obj_dirname + "/" + fname;
                        }
                        materials.back().tex = dake::gl::texture_manager::instance().find_texture(fname);
                    }
                }
            }
        } else if (deftype == "usemtl") {
            std::string name;
            line >> name;

            bool found = false;
            for (const dake::gl::obj_material &mat: materials) {
                if (mat.name == name) {
                    sections.emplace_back();
                    sections.back().material = mat;
                    found = true;
                    break;
                }
            }

            if (!found) {
                throw std::runtime_error("Could not load OBJ mesh: Could not find material " + name);
            }
        }
    }


    for (const dake::gl::obj_section &s: sections) {
        if (!s.normals.empty() && (s.normals.size() != s.positions.size())) {
            throw std::runtime_error("Could not load OBJ mesh: Some normals are given, some aren't");
        }
        if (!s.tex_coords.empty() && (s.tex_coords.size() != s.positions.size())) {
            throw std::runtime_error("Could not load OBJ mesh: Some texture coordinates are given, some aren't");
        }
    }


    return dake::gl::obj(std::move(sections), lower_left, upper_right);
}


void dake::gl::obj_section::normalize_normals(void)
{
    for (dake::math::vec3 &n: normals) {
        if (n.length()) {
            n.normalize();
        }
    }
}


dake::gl::vertex_array *dake::gl::obj_section::make_vertex_array(int pos_idx, int txc_idx, int nrm_idx)
{
    if (pos_idx < 0) {
        throw std::invalid_argument("dake::gl::obj_section::make_vertex_array: pos_idx must be valid");
    }

    dake::gl::vertex_array *va = new dake::gl::vertex_array;
    va->set_elements(positions.size());

    dake::gl::vertex_attrib *va_pos = va->attrib(pos_idx);
    va_pos->format(3);
    va_pos->data(positions.data());
    va_pos->load();

    if (txc_idx >= 0) {
        if (tex_coords.empty()) {
            throw std::invalid_argument("dake::gl::obj_section::make_vertex_array: No texture coordinates found");
        }

        dake::gl::vertex_attrib *va_txc = va->attrib(txc_idx);
        va_txc->format(2);
        va_txc->data(tex_coords.data());
        va_txc->load();
    }

    if (nrm_idx >= 0) {
        if (normals.empty()) {
            throw std::invalid_argument("dake::gl::obj_section::make_vertex_array: No normals found");
        }

        dake::gl::vertex_attrib *va_nrm = va->attrib(nrm_idx);
        va_nrm->format(3);
        va_nrm->data(normals.data());
        va_nrm->load();
    }

    return va;
}
