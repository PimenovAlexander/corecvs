#include "bufferReaderProvider.h"

BufferReader* BufferReaderProvider::getBufferReader(const std::string &filename)
{
    for (std::vector<BufferReaderProviderImpl*>::iterator p = providers.begin(); p != providers.end(); ++p)
    {
        if ((*p)->provides(filename))
        {
            return (*p)->getBufferReader(filename);
		}
	}
    CORE_ASSERT_FAIL_P(("BufferReaderProvider::getBufferReader(%s): no providers", filename.c_str()));
	return 0;
}

void BufferReaderProvider::add(BufferReaderProviderImpl *provider)
{
	providers.push_back(provider);
}

BufferReaderProvider::~BufferReaderProvider()
{
    for (std::vector<BufferReaderProviderImpl*>::iterator p = providers.begin(); p != providers.end(); ++p)
    {
        delete *p;
    }
	providers.clear();
}

BufferReaderProvider& BufferReaderProvider::getInstance()
{
	static BufferReaderProvider provider;
	return provider;
}

BufferReaderProvider::BufferReaderProvider()
{
}

RuntimeTypeBuffer BufferReaderProvider::read(const std::string &s)
{
    BufferReader* reader = getInstance().getBufferReader(s);
    RuntimeTypeBuffer buffer = reader->read(s);
    delete reader;
    return buffer;
}

corecvs::RGB24Buffer BufferReaderProvider::readRgb(const std::string &s)
{
    BufferReader* reader = getInstance().getBufferReader(s);
    auto buffer = reader->readRgb(s);
    delete reader;
    return buffer;
}

void BufferReaderProvider::write(const RuntimeTypeBuffer& buffer, const std::string &s)
{
    BufferReader* reader = getInstance().getBufferReader(s);
    reader->write(buffer, s);
    delete reader;
}

void BufferReaderProvider::writeRgb(const corecvs::RGB24Buffer& buffer, const std::string &s)
{
    BufferReader* reader = getInstance().getBufferReader(s);
    reader->writeRgb(buffer, s);
    delete reader;
}
