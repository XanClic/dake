#include <cstdio>
#include <stdexcept>

#include <dake/gl/find_resource.hpp>


std::string dake::gl::find_resource_filename(const std::string &basename)
{
    std::string fname(basename);

    for (int i = 0; i < 5; i++) {
        // could've used access(), but that probably won't work on Windows
        FILE *fp = fopen(fname.c_str(), "rb");
        if (fp) {
            fclose(fp);
            return fname;
        }

        fname = "../" + fname;
    }

    throw std::invalid_argument("Could not locate resource " + basename);
}
