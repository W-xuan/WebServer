#include "buffer.h"

namespace MicroWS
{

    Buffer::Buffer(int initBufferSize) : buffer_(initBufferSize), readPos_(0), writePos_(0) {}
}