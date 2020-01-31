#include "texture_gl_es20.hxx"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/// texture impl

texture_gl_es20::texture_gl_es20(std::string_view path)
    : file_path(path)
{
    stbi_set_flip_vertically_on_load(true);
    int            width      = 0;
    int            height     = 0;
    int            components = 0;
    unsigned char* decoded_img =
        stbi_load(path.data(), &width, &height, &components, 4);

    // if there's an error, display it
    if (decoded_img == nullptr)
    {
        std::cerr << "error: can't load file: " << path << std::endl;
        throw std::runtime_error("can't load texture");
    }

    glGenTextures(1, &tex_handl);
    GL_CHECK()
    glBindTexture(GL_TEXTURE_2D, tex_handl);
    GL_CHECK()

    GLint mipmap_level = 0;
    GLint border       = 0;
    glTexImage2D(GL_TEXTURE_2D, mipmap_level, GL_RGBA, width, height, border,
                 GL_RGBA, GL_UNSIGNED_BYTE, decoded_img);
    GL_CHECK()

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_CHECK()
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CHECK()
}

texture_gl_es20::~texture_gl_es20()
{
    glDeleteTextures(1, &tex_handl);
    GL_CHECK()
}
void texture_gl_es20::bind() const
{
    GLboolean is_texture = glIsTexture(tex_handl);
    SDL_assert(is_texture);
    GL_CHECK()
    glBindTexture(GL_TEXTURE_2D, tex_handl);
    GL_CHECK()
}
