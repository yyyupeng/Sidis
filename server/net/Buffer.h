#pragma once

#include "../base/Packet.h"

class Buffer
{
public:
    static const size_t KCheapPrepend = 8;
    static const size_t KInitialSize = 1024;

    Buffer();
    size_t readableBytes() const; 
    size_t writeableBytes() const;
    size_t prependableBytes() const; 
    const char *peek();
    void retrieve(size_t len);
    void retrieveUntil(const char *end);
    void retrieveAll();
    std::string retrieveAllAsString();
    std::string retrieveAsString(size_t len);
    char *beginWrite();
    void hasWritten(size_t len);
    void prepend(const void *data, size_t len);
    void append(const char *data, size_t len);
    ssize_t readFd(int fd);

private:
    std::vector<char> buffer;
    size_t readIndex;
    size_t writeIndex;

    char *begin();
};
