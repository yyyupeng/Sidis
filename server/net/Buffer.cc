#include "Buffer.h"

Buffer::Buffer()
:   buffer(KInitialSize + KCheapPrepend),
    readIndex(KCheapPrepend),
    writeIndex(KCheapPrepend)
{
    assert(readableBytes() == 0);
    assert(writeableBytes() == KInitialSize);
    assert(prependableBytes() == KCheapPrepend);
}

size_t Buffer::readableBytes() const
{
    return writeIndex - readIndex;
}

size_t Buffer::writeableBytes() const
{
    return buffer.size() - writeIndex;
}

size_t Buffer::prependableBytes() const
{
    return readIndex;
}

const char* Buffer::peek()
{
    return begin() + readIndex;
}

void Buffer::retrieve(size_t len)
{
    assert(len <= readableBytes());
    if(len < readableBytes())
    {
        readIndex += len;
    }
    else
    {
        retrieveAll();
    }
}

void Buffer::retrieveUntil(const char *end)
{
    assert(peek() <= end);
    assert(end <= beginWrite());
    retrieve(end - peek());
}

void Buffer::retrieveAll()
{
    readIndex = KCheapPrepend;
    writeIndex = KCheapPrepend;
}

std::string Buffer::retrieveAllAsString()
{
    return retrieveAsString(readableBytes());
}

std::string Buffer::retrieveAsString(size_t len)
{
    assert(len <= readableBytes());
    std::string result(peek(), len);
    retrieve(len);
    return result;
}

char* Buffer::beginWrite()
{
    return begin() + writeIndex;
}

void Buffer::hasWritten(size_t len)
{
    assert(len <= writeableBytes());
    writeIndex += len;
}

void Buffer::prepend(const void *data, size_t len)
{
    assert(len <= prependableBytes());
    readIndex -= len;
    const char *p = static_cast<const char*>(data);
    std::copy(p, p + len, begin() + readIndex);
}

void Buffer::append(const char *data, size_t len)
{
    if(writeableBytes() < len)
    {
        if(writeableBytes() + prependableBytes() < len + KCheapPrepend)
        {
            buffer.resize(writeIndex + len);
        }
        else
        {
            assert(KCheapPrepend < readIndex);
            size_t readable = readableBytes();
            std::copy(begin() + readIndex, begin() + writeIndex, begin() + KCheapPrepend);
            readIndex = KCheapPrepend;
            writeIndex = readIndex + readable;
            assert(readable == readableBytes());
        }
    }
    assert(writeableBytes() >= len);
    std::copy(data, data + len, beginWrite());
    hasWritten(len);
}

ssize_t Buffer::readFd(int fd)
{
    char buf[65536] = {0};
    struct iovec vec[2];
    const size_t writeable = writeableBytes();
    vec[0].iov_base = begin() + writeIndex;
    vec[0].iov_len = writeable;
    vec[1].iov_base = buf;
    vec[1].iov_len = sizeof(buf);
    const ssize_t n = readv(fd, vec, 2);
    if(n < 0)
    {
        std::cout << "Buffer::readFd: " << __LINE__ << std::endl;
    }
    else if(static_cast<size_t>(n) <= writeable)
    {
        writeIndex += n;
    }
    else
    {
        writeIndex = buffer.size();
        append(buf, n - writeable);
    }
    return n;
}

char* Buffer::begin()
{
    return &*buffer.begin();
}
