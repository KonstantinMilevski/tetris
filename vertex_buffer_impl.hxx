#pragma once

#include "engine.hxx"

#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>

class vertex_buffer_impl final : public vertex_buffer
{
public:
    vertex_buffer_impl(const vertex* tri, std::size_t n);

    ~vertex_buffer_impl() final {}

    const vertex*  data() const final { return vertexes.data(); }
    virtual size_t size() const final { return vertexes.size(); }

private:
    std::vector<vertex> vertexes;
};
