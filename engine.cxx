#include "engine.hxx"
#include "shader_gl_es20.hxx"
#include "texture_gl_es20.hxx"
#include "vertex_buffer_impl.hxx"

#include <algorithm>
#include <cassert>
#include <exception>
//#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>



//unsigned long text_width  = 0;
//unsigned long text_height = 0;

// kod for SDL_events

struct bind
{
    bind() {}
    bind(SDL_Keycode _key, std::string_view _name, keys _k)
        : key_sdl(_key)
        , name(_name)
        , selected_key(_k)

    {
    }
    SDL_Keycode      key_sdl;
    std::string_view name;
    keys             selected_key;
};

const std::array<bind, 6> keys_list{
    bind{ SDLK_LEFT, "left", keys::left },
    bind{ SDLK_DOWN, "down", keys::down },
    bind{ SDLK_RIGHT, "right", keys::right },
    bind{ SDLK_UP, "rotate", keys::rotate },
    bind{ SDLK_p, "pause", keys::pause },
    bind{ SDLK_ESCAPE, "exit", keys::exit },
}

;
static bool check_input(const SDL_Event& e, const bind*& result)
{
    using namespace std;

    const auto it =
        find_if(begin(keys_list), end(keys_list),
                [&](const ::bind& b) { return b.key_sdl == e.key.keysym.sym; });

    if (it != end(keys_list))
    {
        result = &(*it);
        return true;
    }
    return false;
}

//
std::ostream& operator<<(std::ostream& os, const SDL_version& v)
{
    os << v.major << ':';
    os << v.minor << ':';
    os << v.patch;
    return os;
}
std::iostream& operator>>(std::iostream& is, vec2& v)
{
    is >> v.x;
    is >> v.y;
    return is;
}
std::istream& operator>>(std::istream& is, vertex& v)
{
    is >> v.pos.x;
    is >> v.pos.y;
    is >> v.uv.x;
    is >> v.uv.y;
    // is >> v.c;
    return is;
}
std::istream& operator>>(std::istream& is, triangle& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

tri2::tri2()
    : v{ vertex(), vertex(), vertex() }
{
}
tri2::tri2(vertex v01, vertex v02, vertex v03)
    : v{ vertex(v01), vertex(v02), vertex(v03) }
{
}

std::istream& operator>>(std::istream& is, tri2& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}
vec2& vec2::operator+=(const vec2& l)
{
    x += l.x;
    y += l.y;
    return *this;
}

vec2& vec2::operator*=(const vec2& l)
{
    x *= l.x;
    y *= l.y;
    return *this;
}

vec2& vec2::operator*=(const float& f)
{
    x *= f;
    y *= f;
    return *this;
}

vec2& vec2::operator/=(const float& f)
{
    x /= f;
    y /= f;
    return *this;
}

vec2& vec2::operator+=(const float& f)
{
    x += f;
    y += f;
    return *this;
}
vec2& vec2::operator-=(const float& f)
{
    x -= f;
    y -= f;
    return *this;
}

bool vec2::operator==(const vec2& r)
{
    return std::abs(x - r.x) < 0.0001f && std::abs(y - r.y) < 0.0001f;
}
float vec2::length() const
{
    return std::sqrt(x * x + y * y);
}
vec2 operator+(const vec2& l, const vec2& r)
{
    vec2 result;
    result.x = l.x + r.x;
    result.y = l.y + r.y;
    return result;
}

vec2 operator-(const vec2& l, const vec2& r)
{
    vec2 result;
    result.x = l.x - r.x;
    result.y = l.y - r.y;
    return result;
}
//
matrix::matrix()
    : row0(1.0f, 0.f)
    , row1(0.f, 1.f)
    , row2(0.f, 0.f)
{
}
matrix matrix::scale(float scale)
{
    matrix result;
    result.row0.x = scale;
    result.row1.y = scale;
    return result;
}
matrix matrix::identity()
{
    return matrix::scale(1.f);
}
matrix matrix::scale(float sx, float sy)
{
    matrix r;
    r.row0.x = sx;
    r.row1.y = sy;
    return r;
}
matrix matrix::rotation(float thetha)
{
    matrix result;

    result.row0.x = std::cos(thetha);
    result.row0.y = std::sin(thetha);

    result.row1.x = -std::sin(thetha);
    result.row1.y = std::cos(thetha);

    return result;
}
matrix matrix::move(const vec2& delta)
{
    matrix r = matrix::identity();
    r.row2   = delta;
    return r;
}
vec2 operator*(const vec2& v, const matrix& m)
{
    vec2 result;
    result.x = v.x * m.row0.x + v.y * m.row0.y + m.row2.x;
    result.y = v.x * m.row1.x + v.y * m.row1.y + m.row2.y;
    return result;
}
matrix operator*(const matrix& m1, const matrix& m2)
{
    matrix r;

    r.row0.x = m1.row0.x * m2.row0.x + m1.row1.x * m2.row0.y;
    r.row1.x = m1.row0.x * m2.row1.x + m1.row1.x * m2.row1.y;
    r.row0.y = m1.row0.y * m2.row0.x + m1.row1.y * m2.row0.y;
    r.row1.y = m1.row0.y * m2.row1.x + m1.row1.y * m2.row1.y;

    r.row2.x = m1.row2.x * m2.row0.x + m1.row2.y * m2.row0.y + m2.row2.x;
    r.row2.y = m1.row2.x * m2.row1.x + m1.row2.y * m2.row1.y + m2.row2.y;

    return r;
}
//
vertex_buffer::~vertex_buffer() {}
texture::~texture() {}
engine::~engine() {}
// engine

// engine_impl

class engine_impl final : public engine
{
public:
    std::string initialize(std::string_view /*str*/) override final
    {

        using namespace std;

        stringstream serr;

        SDL_version compiled = { 0, 0, 0 };
        SDL_version linked   = { 0, 0, 0 };
        SDL_VERSION(&compiled)
        SDL_GetVersion(&linked);
        if (SDL_COMPILEDVERSION !=
            SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
        {
            serr << "warning: SDL2 compiled and linked version mismatch: "
                 << compiled << " " << linked << endl;
        }

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            serr << "error: failed call SDL_Init: " << SDL_GetError() << endl;
            return serr.str();
        }

        window = SDL_CreateWindow("tetris", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, screen_width,
                                  screen_height, ::SDL_WINDOW_OPENGL);
        if (window == nullptr)
        {
            serr << "error: failed call SDL_CreateWindow: " << SDL_GetError()
                 << endl;
            return serr.str();
        }

        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr)
        {
            std::string msg("can't create opengl context: ");
            msg += SDL_GetError();
            serr << msg << endl;
            return serr.str();
        }

        int gl_major_ver = 2;

        int result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
        SDL_assert(result == 0);
        int gl_minor_ver = 0;
        result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
        SDL_assert(result == 0);

        if (gl_major_ver <= 2 && gl_minor_ver < 1)
        {
            serr << "current context opengl version: " << gl_major_ver << '.'
                 << gl_minor_ver << '\n'
                 << "need opengl version at least: 2.1\n"
                 << std::flush;
            return serr.str();
        }

        if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0)
        {
            std::cerr << "error: failed to initialize glad" << std::endl;
        }

        // end glGenBuffers
        shader01 = new shader_gl_es20(
            R"(
                    uniform mat3 u_matrix;
                    attribute vec2 a_position;
                    attribute vec2 a_tex_coord;
                    attribute vec4 a_color;
                    varying vec4 v_color;
                    varying vec2 v_tex_coord;
                    void main()
                    {
                    v_tex_coord = a_tex_coord;
                    v_color = a_color;
                    vec3 pos = vec3(a_position, 1.0) * u_matrix;
                    gl_Position = vec4(pos, 1.0);
                    }
                    )",
            R"(
                    varying vec2 v_tex_coord;
                    varying vec4 v_color;
                    uniform sampler2D s_texture;
                    void main()
                    {
                    gl_FragColor = texture2D(s_texture, v_tex_coord) * v_color;
                    }
                    )",
            { { 0, "a_position" }, { 1, "a_color" }, { 2, "a_tex_coord" } });

        glEnable(GL_BLEND);
        GL_CHECK()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GL_CHECK()
        return "";
    }
    void uninitialize() override final
    {
        // glDeleteProgram(program_id_);
        delete shader00;
        delete shader01;
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void render_tetris(const vertex_buffer& buff, texture* tex,
                       const matrix& m) final
    {

        shader01->use();
        texture_gl_es20* texture = static_cast<texture_gl_es20*>(tex);
        texture->bind();
        shader01->set_uniform("s_texture", texture);
        shader01->set_uniform("u_matrix", m);

        const vertex* t = buff.data();

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                              &t->pos);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), &t->uv);
        glEnableVertexAttribArray(2);
        GL_CHECK()

        // colors
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex),
                              &t->c);
        GL_CHECK()
        glEnableVertexAttribArray(1);
        GL_CHECK()

        // texture coordinates

        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(buff.size()));
        GL_CHECK()

        glDisableVertexAttribArray(1);
        GL_CHECK()
        glDisableVertexAttribArray(2);
        GL_CHECK()
    }

    void swap_buffer() override final
    {
        SDL_GL_SwapWindow(window);
        GL_CHECK()
        glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
        GL_CHECK()
        glClear(GL_COLOR_BUFFER_BIT);
        GL_CHECK()
    }
    bool read_event(event& e) override final
    {
        using namespace std;
        SDL_Event sdl_event;
        if (SDL_PollEvent(&sdl_event))
        {
            const ::bind* binding = nullptr;

            if (sdl_event.type == SDL_QUIT)
            {
                e.key = keys::exit;
                return true;
            }
            else if (sdl_event.type == SDL_KEYDOWN ||
                     sdl_event.type == SDL_KEYUP)
            {
                if (check_input(sdl_event, binding))
                {
                    e.key     = binding->selected_key;
                    e.is_down = sdl_event.type == SDL_KEYDOWN;
                    // e.timestamp = sdl_event.common.timestamp * 0.001;
                    return true;
                }
                return false;
            }
        }
        return false;
    }
    bool is_key_down(const enum keys key) final
    {
        const auto it =
            std::find_if(begin(keys_list), end(keys_list),
                         [&](const bind& b) { return b.selected_key == key; });
        if (it != end(keys_list))
        {
            const std::uint8_t* state = SDL_GetKeyboardState(nullptr);
            int sdl_scan_code         = SDL_GetScancodeFromKey(it->key_sdl);
            return state[sdl_scan_code];
        }
        return false;
    }
    texture* create_texture(std::string_view path) final
    {
        return new texture_gl_es20(path);
    }

    void destroy_texture(texture* t) final { delete t; }

    vertex_buffer* create_vertex_buffer(const vertex* vert, std::size_t count) override
    {
        assert(vert != nullptr);
        return new vertex_buffer_impl(vert, count);
    }
    void destroy_vertex_buffer(vertex_buffer* buffer)override { delete buffer; }

    float get_time_from_init() final
    {
        std::uint32_t ms_from_library_initialization = SDL_GetTicks();
        float         seconds = ms_from_library_initialization * 0.001f;
        return seconds;
    }
~engine_impl(){}
private:
    SDL_Window*     window         = nullptr;
    SDL_GLContext   gl_context     = nullptr;
    //GLuint          program_id_    = 0;
    shader_gl_es20* shader00       = nullptr;
    shader_gl_es20* shader01       = nullptr;
    //uint32_t        gl_default_vbo = 0;
};

static bool already_exist = false;
engine*     create_engine()
{
    if (true == already_exist)
    {
        throw std::runtime_error("engine allredy exist");
    }
    engine* eng   = new engine_impl();
    already_exist = true;
    return eng;
}
void destroy_engine(engine* eng)
{
    if (false == already_exist)
    {
        throw std::runtime_error("engine no exist");
    }
    if (nullptr == eng)
    {
        throw std::runtime_error("engine is null");
    }
    delete eng;
}
