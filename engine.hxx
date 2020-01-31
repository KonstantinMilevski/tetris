#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "color.hxx"
#include "glad/glad.h"
#include <SDL2/SDL.h>

#define GL_CHECK()                                                             \
    {                                                                          \
        const int err = static_cast<int>(glGetError());                        \
        if (err != GL_NO_ERROR)                                                \
        {                                                                      \
            switch (err)                                                       \
            {                                                                  \
                case GL_INVALID_ENUM:                                          \
                    std::cerr << GL_INVALID_ENUM << std::endl;                 \
                    break;                                                     \
                case GL_INVALID_VALUE:                                         \
                    std::cerr << GL_INVALID_VALUE << std::endl;                \
                    break;                                                     \
                case GL_INVALID_OPERATION:                                     \
                    std::cerr << GL_INVALID_OPERATION << std::endl;            \
                    break;                                                     \
                case GL_INVALID_FRAMEBUFFER_OPERATION:                         \
                    std::cerr << GL_INVALID_FRAMEBUFFER_OPERATION              \
                              << std::endl;                                    \
                    break;                                                     \
                case GL_OUT_OF_MEMORY:                                         \
                    std::cerr << GL_OUT_OF_MEMORY << std::endl;                \
                    break;                                                     \
            }                                                                  \
            assert(false);                                                     \
        }                                                                      \
    }

template <typename T>
static void load_gl_func(const char* func_name, T& result)
{
    void* gl_pointer = SDL_GL_GetProcAddress(func_name);
    if (nullptr == gl_pointer)
    {
        throw std::runtime_error(std::string("can't load GL function") +
                                 func_name);
    }
    result = reinterpret_cast<T>(gl_pointer);
}
constexpr size_t screen_width  = 960.f; // 640;
constexpr size_t screen_height = 540.f; // 480;

/// for SDLevent

enum class keys
{
    left,
    right,
    down,
    rotate,
    pause,
    exit
};

struct event
{
    enum keys key;
    bool      is_down;
    double    timestamp;
};
struct vec2
{
    vec2() {}
    vec2(float x_, float y_)
        : x(x_)
        , y(y_)
    {
    }
    float x = 0;
    float y = 0;
    vec2& operator+=(const vec2& l);
    vec2& operator*=(const vec2& l);
    vec2& operator*=(const float& f);
    vec2& operator/=(const float& f);
    vec2& operator+=(const float& f);
    vec2& operator-=(const float& f);
    bool  operator==(const vec2& r);
    float length() const;
};
std::iostream& operator>>(std::iostream& is, vec2& v);
struct vertex
{
    vertex()
        : pos()
        , uv()

    {
    }
    vertex(vec2 p_, vec2 uv_)
        : pos(p_)
        , uv(uv_)

    {
    }
    vertex(float x_, float y_, float tx_, float ty_)
        : pos(x_, y_)
        , uv(tx_, ty_)
    {
    }
    vec2  pos;
    vec2  uv;
    color c = { 1.0, 1.0, 1.0, 1.0 };
};

struct triangle
{
    triangle()
    {
        v[0] = vertex();
        v[1] = vertex();
        v[2] = vertex();
    }
    triangle(vertex v00, vertex v01, vertex v02)
    {
        v[0] = v00;
        v[1] = v01;
        v[2] = v02;
    }
    vertex v[3];
};

struct tri2
{
    tri2();
    tri2(vertex v01, vertex v02, vertex v03);
    vertex v[3];
};
vec2 operator+(const vec2& l, const vec2& r);
vec2 operator-(const vec2& l, const vec2& r);

struct matrix
{
    matrix();
    static matrix identity();
    static matrix scale(float scale);
    static matrix scale(float sx, float sy);
    static matrix rotation(float thetha);
    static matrix move(const vec2& delta);
    vec2          row0;
    vec2          row1;
    vec2          row2;
};

vec2   operator*(const vec2& v, const matrix& m);
matrix operator*(const matrix& m1, const matrix& m2);

std::istream& operator>>(std::istream& is, vertex& v);
std::istream& operator>>(std::istream& is, tri2& t);

std::istream& operator>>(std::istream& is, vertex& v);
std::istream& operator>>(std::istream& is, triangle& t);

class texture
{
public:
    virtual ~texture();
    virtual void          bind() const       = 0;
    virtual std::uint32_t get_width() const  = 0;
    virtual std::uint32_t get_height() const = 0;
};

class vertex_buffer
{
public:
    virtual ~vertex_buffer();
    virtual const vertex* data() const = 0;
    virtual size_t        size() const = 0;
};

class engine;

engine* create_engine();
void    destroy_engine(engine*);

class engine
{
public:
    ~engine();
    virtual std::string initialize(std::string_view)     = 0;
    virtual void        uninitialize()                   = 0;
    virtual bool        read_event(event& e)             = 0;
    virtual bool        is_key_down(const enum keys key) = 0;
    virtual void        render_tetris(const vertex_buffer& buff, texture* tex,
                                      const matrix& m)   = 0;
    virtual float       get_time_from_init()             = 0;

    virtual vertex_buffer* create_vertex_buffer(const vertex*, std::size_t) = 0;
    virtual void           destroy_vertex_buffer(vertex_buffer*)            = 0;

    virtual texture* create_texture(std::string_view path) = 0;

    virtual void destroy_texture(texture* t) = 0;
    virtual void swap_buffer()               = 0;
};
