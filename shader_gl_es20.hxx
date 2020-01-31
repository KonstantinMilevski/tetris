#pragma once

#include "engine.hxx"
#include "texture_gl_es20.hxx"

#include <tuple>

class shader_gl_es20
{
public:
    shader_gl_es20(
        std::string_view vertex_src, std::string_view fragment_src,
        const std::vector<std::tuple<GLuint, const GLchar*>>& attributes);
    void use() const;
    void set_uniform(std::string_view uniform_name, texture_gl_es20* texture);
    void set_uniform(std::string_view uniform_name, const matrix& m);

    void   set_uniform(std::string_view uniform_name, const color& c);
    GLuint get_program_id() const;

private:
    GLuint compile_shader(GLenum shader_type, std::string_view src);
    GLuint link_shader_program(
        const std::vector<std::tuple<GLuint, const GLchar*>>& attributes);

    GLuint vert_shader = 0;
    GLuint frag_shader = 0;
    GLuint program_id  = 0;
};
