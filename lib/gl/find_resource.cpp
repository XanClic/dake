#include <stdexcept>

extern "C"
{
#include <unistd.h>
}

#include <dake/gl/find_resource.hpp>


std::string dake::gl::find_resource_filename(const std::string &basename)
{
    std::string fname(basename);

    for (int i = 0; i < 5; i++) {
        if (!access(fname.c_str(), F_OK)) {
            return fname;
        }

        fname = "../" + fname;
    }

    throw std::invalid_argument("Could not locate resource " + basename);
}
