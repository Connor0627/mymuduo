#pragma once

#include <algorithm>
#include <string>
#include <vector>

// 网络库底层的缓冲器类型定义
/**
 *  +-------------------+----------------+----------------+
 *  | prependable bytes | readable bytes | writeable bytes|
 *  |                   |   (content)    |                |
 *  +-------------------+----------------+----------------+
 *  0      <=     readerIndex  <=    writerIndex   <=    size
 */
class Buffer {
  public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize), readerIndex_(kCheapPrepend),
          writerIndex_(kCheapPrepend) {}
    size_t prependableBytes() const { return readerIndex_; }
    size_t readableBytes() const { return writerIndex_ - readerIndex_; }
    size_t writeableBytes() const { return buffer_.size() - writerIndex_; }
    const char *peek() const { return begin() + readerIndex_; }
    void retrieve(size_t len) {
        if (len < readableBytes()) {
            readerIndex_ += len; // 应用只读去了可读缓冲区数据的一部分
        } else {                 // len == readableBytes()
            retrieveAll();
        }
    }
    void retrieveAll() { readerIndex_ = writerIndex_ = kCheapPrepend; }
    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }
    std::string retrieveAsString(size_t len) {
        std::string result(peek(), len);
        retrieve(len); // 从缓冲区读取数据后，进行复位操作
        return result;
    }
    void ensureWriteableBytes(size_t len) {
        if (writeableBytes() < len) {
            makeSpace(len);
        }
    }
    // 把[data, data+len]内存上的数据添加到writeable缓冲区中
    void append(const char *data, size_t len) {
        ensureWriteableBytes(len);
        std::copy(data, data + len, beginWrite());
        writerIndex_ += len;
    }
    char *beginWrite() { return begin() + writerIndex_; }
    const char *beginWrite() const { return begin() + writerIndex_; }
    ssize_t readFd(int fd, int *saveErrno);  // 从fd上读取数据
    ssize_t writeFd(int fd, int *saveErrno); // 通过fd发送数据

  private:
    char *begin() { return &*buffer_.begin(); }
    const char *begin() const { return &*buffer_.begin(); }
    void makeSpace(size_t len) {
        if (writeableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writerIndex_ + len);
        } else {
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_, begin() + writerIndex_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};