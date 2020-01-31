#pragma once
#include "engine.hxx"
#include <iostream>
class index_buffer_impl
{
public:
    index_buffer_impl(const std::uint16_t* i, size_t n);
    ~index_buffer_impl();
    void bind() const;

    std::uint32_t size() const;

private:
    std::uint32_t gl_handle{ 0 };

    std::uint32_t count{ 0 };
};
