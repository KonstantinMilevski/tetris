#include "field.hxx"

#include <algorithm>
#include <array>
#include <cassert>

std::vector<vertex> block::build_block()
{
    std::array<vertex, 4> quad;
    // you can change colore of texture with shift uv coord
    ///   0            1
    ///   *------------*
    ///   |           /|
    ///   |         /  |
    ///   |      P/    |  // P - pos_ center - OpenGL (0,0)
    ///   |     /      |  // vertex 3 - texture (0,0)
    ///   |   /        |
    ///   | /          |
    ///   *------------*
    ///   3            2
    ///
    /// 0 left-up, clockwise
    /// vec2.pos
    quad[0].pos.x = xy_rect_.pos.x - xy_rect_.size.x * 0.5f;
    quad[0].pos.y = xy_rect_.pos.y + xy_rect_.size.y * 0.5f;
    quad[1].pos.x = xy_rect_.pos.x + xy_rect_.size.x * 0.5f;
    quad[1].pos.y = xy_rect_.pos.y + xy_rect_.size.y * 0.5f;

    quad[2].pos.x = xy_rect_.pos.x + xy_rect_.size.x * 0.5f;
    quad[2].pos.y = xy_rect_.pos.y - xy_rect_.size.y * 0.5f;
    quad[3].pos.x = xy_rect_.pos.x - xy_rect_.size.x * 0.5f;
    quad[3].pos.y = xy_rect_.pos.y - xy_rect_.size.y * 0.5f;

    /// vec2.uv, OpenGL texture lower left angle is (0, 0) coordinate

    quad[3].uv.x = uv_rect_.pos.x;
    quad[3].uv.y = uv_rect_.pos.y;
    quad[0].uv.x = quad[3].uv.x;
    quad[0].uv.y = quad[3].uv.y + uv_rect_.size.y;
    quad[1].uv.x = quad[3].uv.x + uv_rect_.size.x;
    quad[1].uv.y = quad[3].uv.y + uv_rect_.size.y;

    quad[2].uv.x = quad[3].uv.x + uv_rect_.size.x;
    quad[2].uv.y = quad[3].uv.y;

    std::vector<vertex> quad_tri = { quad[1], quad[2], quad[3],
                                     quad[0], quad[1], quad[3] };
    return quad_tri;
}

void block::set_position(const vec2& new_pos)
{
    xy_rect_.pos = new_pos;
    // uv_rect_.pos = new_pos;
}

void block::set_texture_pos(const rect& new_pos)
{
    uv_rect_.size = new_pos.size;
    uv_rect_.pos  = new_pos.pos;
}
///////////////////////////////
field::field(const size_t col, const size_t row)
    : col_(col)
    , row_(row)
{
    {
        field_.resize(col * row);
        for (size_t i = 0; i < col * row; i++)
        {
            float x = i % col * cell_size + 0.5f * cell_size;
            float y = i / col * cell_size + 0.5f * cell_size;
            rect  rect_xy({ x, y }, { cell_size, cell_size });
            rect  rect_uv({ x, y }, { cell_size, cell_size });
            field_.at(i).cell_.xy_rect_ = rect_xy;
            field_.at(i).cell_.uv_rect_ = rect_uv;
        }
    }
}

void field::set_block_on_field(block& bl, const size_t& pos)
{
    assert(pos >= 0);
    assert(pos < col_ * row_);
    vec2 cell_pos = this->return_cell_pos(pos);
    bl.set_position(cell_pos);
    field_.at(pos).cell_.uv_rect_.size = bl.uv_rect_.size;
    field_.at(pos).cell_.uv_rect_.pos  = bl.uv_rect_.pos;
    field_.at(pos).is_empty            = false;
}

void field::set_figure(const figure& figur, block& bl)
{
    for (size_t i = 0; i < 4; i++)
    {
        this->set_block_on_field(bl, figur.coord_.at(i));
    }
}

std::vector<vertex> field::occupied_cells()
{
    std::vector<vertex> occupied_pos;
    for (auto var : field_)
    {
        if (!var.is_empty)
        {
            std::vector<vertex> temp_vertexes = var.cell_.build_block();
            occupied_pos.insert(end(occupied_pos), begin(temp_vertexes),
                                end(temp_vertexes));
        }
    }

    return occupied_pos;
}

void field::clear_position(const figure& fig)
{
    for (auto cell : fig.coord_)
    {
        field_.at(cell).is_empty = true;
    }
}

void field::clear_field()
{
    for (auto& var : field_)
    {
        var.is_empty = true;
    }
}

vec2 field::return_cell_pos(const size_t& n)
{
    return field_.at(n).cell_.xy_rect_.pos;
}

bool field::check_field_border(const figure& fig)
{
    for (size_t i = 0; i < 4; i++)
    {
        if (fig.coord_.at(i) >= row_ * col_ /*|| fig.coord_.at(i) < 0*/)
            return false;
    }
    return true;
}

bool field::check_empty_cell(const figure& fig)
{
    for (size_t i = 0; i < 4; i++)
    {
        if (field_.at(fig.coord_.at(i)).is_empty == false)
            return false;
    }
    return true;
}
bool field::check_figure_horizont(const figure& old, const figure& next)
{
    for (size_t i = 0; i < 4; i++)
    {
        if (old.coord_.at(i) / col_ != next.coord_.at(i) / col_)
            return false;
    }
    return true;
}

bool field::check_full_line(std::vector<cell>::iterator line)
{
    size_t                      count{ 0 };
    std::vector<cell>::iterator beg = line;
    std::advance(line, col_);
    for (; beg != line; beg++)
    {
        if (!beg->is_empty)
        {
            count++;
        }
    }
    if (col_ == count)
        return true;
    else
        return false;
}

void field::check_field_line()
{
    std::vector<cell> temp;
    auto              it = begin(field_);
    for (; it != end(field_); std::advance(it, col_))
    {
        if (!check_full_line(it))
        {
            std::copy_n(it, col_, std::back_inserter(temp));
        }
    }
    if (!temp.empty())
    {
        temp.resize(row_ * col_);
        this->clear_field();
        for (size_t i = 0; i < row_ * col_; i++)
        {
            field_.at(i).is_empty            = temp.at(i).is_empty;
            field_.at(i).cell_.uv_rect_.size = temp.at(i).cell_.uv_rect_.size;
        }
    }
}

rect field::field_rect()
{
    float width = field_.at(col_ * row_ - 1).cell_.uv_rect_.pos.x +
                  0.5f * cell_size - field_.at(0).cell_.uv_rect_.pos.x -
                  0.5f * cell_size;
    float height = field_.at(col_ * row_ - 1).cell_.uv_rect_.pos.y +
                   0.5f * cell_size - field_.at(0).cell_.uv_rect_.pos.y -
                   0.5f * cell_size;
    vec2 left_down(
        field_.at(0).cell_.uv_rect_.pos.x - 0.5f * cell_size + 0.5f * width,
        field_.at(0).cell_.uv_rect_.pos.y - 0.5f * cell_size + 0.5f * height);
    vec2 right_up(
        field_.at(col_ * row_ - 1).cell_.uv_rect_.pos.x + 0.5f * cell_size,
        field_.at(col_ * row_ - 1).cell_.uv_rect_.pos.y + 0.5f * cell_size);
    return { left_down, { width, height } };
}

figure::figure()
{
    coord_ = { 0, 0, 0, 0 };
}

figure::figure(std::array<size_t, 4>& coord, size_t f_width)
{
    assert(std::all_of(begin(coord), end(coord),
                       [](const size_t& a) { return (a < 8); }));
    size_t x{ 0 }, y{ 0 };
    for (size_t i = 0; i < 4; i++)
    {
        x            = coord.at(i) % 2;
        y            = coord.at(i) / 2;
        coord_.at(i) = x + y * f_width;
    }
}

void figure::figure_change_position(const size_t& pos)
{

    for (auto& block : coord_)
    {
        block += pos;
    }
}

void figure::figure_rotate(const size_t& f_width)
{
    std::array<vec2, 4> coord_XY;
    for (size_t i = 0; i < 4; i++)
    {
        float x          = coord_.at(i) % f_width;
        float y          = coord_.at(i) / f_width;
        coord_XY.at(i).x = x;
        coord_XY.at(i).y = y;
    }
    vec2 centr = coord_XY.at(1);
    int  new_pos_x(0);
    int  new_pos_y(0);
    int  move_left  = 0;
    int  move_right = 0;
    int  compare    = 0;
    for (int i = 0; i < 4; i++)
    {
        new_pos_x = static_cast<int>(
            centr.x - (coord_XY.at(static_cast<size_t>(i)).y - centr.y));
        new_pos_y = static_cast<int>(
            centr.y + (coord_XY.at(static_cast<size_t>(i)).x - centr.x));
        if (new_pos_x < 0)
        {
            compare = new_pos_x + 0;
            if (move_right > compare)
                move_right = compare;
        }
        if (new_pos_x > static_cast<int>(f_width) - 1)
        {
            compare = new_pos_x - (static_cast<int>(f_width) - 1);
            if (move_left < compare)
                move_left = compare;
        }

        coord_.at(static_cast<size_t>(i)) =
            static_cast<size_t>(new_pos_x) +
            static_cast<size_t>(new_pos_y) * f_width;
    }
    if (move_right)
    {
        std::for_each(begin(coord_), end(coord_), [&move_right](size_t& i) {
            i -= static_cast<size_t>(move_right);
        });
    }
    if (move_left)
        for (size_t i = 0; i < 4; i++)
        {
            coord_.at(i) -= static_cast<size_t>(move_left);
        }
}

bool figure::compare_position(const figure& fig)
{
    auto result = std::find_first_of(begin(coord_), end(coord_),
                                     begin(fig.coord_), end(fig.coord_));
    return result == end(coord_) ? false : true;
}
