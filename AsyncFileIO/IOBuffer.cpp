
#include <memory>

#include "IOBuffer.h"
#include "logging.h"

namespace AsyncFileIO
{

inline void AssertValidBufferSize(const size_t size)
{
	CHECK(size > 0);
}
	
struct IOBuffer::Private
{
	std::shared_ptr<void> buffer;
	size_t size{};
	explicit Private(size_t bufferSize);
	Private(void* data, size_t bufferSize, const std::function<void(void*)>& deleter);
};

IOBuffer::Private::Private(const size_t bufferSize)
{
	AssertValidBufferSize(bufferSize);
	buffer = std::shared_ptr<unsigned char>(new unsigned char[bufferSize], std::default_delete<unsigned char[]>());
	size = bufferSize;
}

IOBuffer::Private::Private(void* data, const size_t bufferSize, const std::function<void(void*)>& deleter)
{
	AssertValidBufferSize(bufferSize);
	buffer = std::shared_ptr<void>(data, [deleter, data](void*) {
		if (deleter) (deleter)(data);
		});
	size = bufferSize;
}

IOBuffer::IOBuffer(const size_t bufferSize)
	: m_p(new Private(bufferSize))
{
}

IOBuffer::IOBuffer(void* data, const size_t bufferSize, const std::function<void(void*)>& deleter)
	: m_p(new Private(data, bufferSize, deleter))
{
}

IOBuffer::~IOBuffer()
{
	delete m_p;
	m_p = nullptr;
}

void* IOBuffer::data()
{
	return m_p->buffer.get();
}

const void* IOBuffer::data() const
{
	return m_p->buffer.get();
}

size_t IOBuffer::size() const
{
	return m_p->size;
}
	
}
