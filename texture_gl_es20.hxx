#pragma once

#include "engine.hxx"

#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>

/// texture
#pragma pack(push, 4)
class texture_gl_es20 final : public texture
{
public:
    explicit texture_gl_es20(std::string_view path);
    ~texture_gl_es20() override;

    void bind() const override;

    std::uint32_t get_width() const final { return tex_width; }
    std::uint32_t get_height() const final { return tex_height; }

private:
    std::string   file_path;
    GLuint        tex_handl  = 0;
    std::uint32_t tex_width  = 0;
    std::uint32_t tex_height = 0;
};
#pragma pack(pop)
