#include "vertex_buffer_impl.hxx"

vertex_buffer_impl::vertex_buffer_impl(const vertex* tri, std::size_t n)
    : vertexes(n)
{
    assert(tri != nullptr);
    std::copy_n(tri, n, begin(vertexes));
}
