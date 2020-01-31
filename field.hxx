#pragma once
#include "engine.hxx"
#include <array>

// global

static float window_scale = static_cast<float>(screen_height) / screen_width;
// field 10x25 GL from -0.5 to 0.5 gl_width     =1.f
// field 10x25 GL from -0.1 to 0.1 gl_width     =2.f
constexpr size_t field_height = 20.f;
constexpr size_t field_width  = 10.f;
constexpr size_t cell_size    = 10.f;

struct rect
{
    rect()
        : pos{ 0.f, 0.f }
        , size{ 0.f, 0.f }
    {
    }
    rect(vec2 p, vec2 s)
        : pos{ p }
        , size{ s }
    {
    }
    vec2 pos;
    vec2 size;
};

struct block
{
    block() = default;
    block(rect pos, rect uv_pos)
        : xy_rect_(pos)
        , uv_rect_(uv_pos)
    {
    }

    std::vector<vertex> build_block();
    void                set_position(const vec2& new_pos);
    void                set_texture_pos(const rect& new_pos);
    // centr of cell
    rect xy_rect_;
    // position of texture, first-point 0-left, down; second-size
    rect uv_rect_;
};

struct cell
{
    block cell_;
    bool  is_empty = true;
};
class field;

class figure
{

    friend class field;

public:
    figure();
    figure(std::array<size_t, 4>& coord, size_t f_width);

    void figure_change_position(const size_t& pos);
    void figure_rotate(const size_t& f_width);
    bool compare_position(const figure& fig);

private:
    std::array<size_t, 4> coord_;
};

class field
{
public:
    field(const size_t col, const size_t row);
    void                set_block_on_field(block& bl, const size_t& pos);
    void                set_figure(const figure& fig, block& bl);
    std::vector<vertex> occupied_cells();
    void                clear_position(const figure& fig);
    void                clear_field();
    vec2                return_cell_pos(const size_t& n);
    bool                check_field_border(const figure& fig);
    bool                check_empty_cell(const figure& fig);
    void                check_field_line();
    rect                field_rect();
    bool check_figure_horizont(const figure& old, const figure& next);

private:
    bool check_full_line(std::vector<cell>::iterator line);
    void set_texture();

    size_t            col_;
    size_t            row_;
    std::vector<cell> field_;
};
