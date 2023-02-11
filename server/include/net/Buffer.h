#pragma once
#include <string>
#include "noncopyable.h"

/**
 * Buffer实现缓冲区的功能，提供常用接口
 */
class Buffer
{
public:
    DISALLOW_COPY_AND_MOVE(Buffer);
    Buffer() = default;
    ~Buffer() = default;

    void append(const char* str, const int &size);
    void append(const std::string &str);
    ssize_t size() const;
    const char* c_str() const;
    void clear();
    void getline();
    void setBuf(const char*);
    const std::string &buf() const;

private:
    std::string buf_;

};
