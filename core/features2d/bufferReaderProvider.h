#ifndef BUFFERREADERPROVIDER_H
#define BUFFERREADERPROVIDER_H

#include <string>
#include <vector>

#include "core/buffers/runtimeTypeBuffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

/**
 * This class is depricated. Use BufferFactory instead
 *
 **/

class BufferReader
{
public:
    virtual corecvs::RuntimeTypeBuffer read   (const std::string &s) = 0;
    virtual corecvs::RGB24Buffer       readRgb(const std::string &s) = 0;

    virtual bool write   (const corecvs::RuntimeTypeBuffer& buffer, const std::string &s) = 0;
    virtual bool writeRgb(const corecvs::RGB24Buffer      & buffer, const std::string &s) = 0;
	virtual ~BufferReader() {}
};


class BufferReaderProviderImpl
{
public:
	virtual BufferReader* getBufferReader(const std::string &filename) = 0;

	virtual bool provides(const std::string &filename) = 0;
	virtual ~BufferReaderProviderImpl() {}
};

class BufferReaderProvider
{
public:
    static corecvs::RuntimeTypeBuffer read   (const std::string &s);
    static corecvs::RGB24Buffer       readRgb(const std::string &s);

    static void write   (const corecvs::RuntimeTypeBuffer &buffer, const std::string &s);
    static void writeRgb(const corecvs::RGB24Buffer &buffer, const std::string &s);

	void add(BufferReaderProviderImpl *provider);
	BufferReader* getBufferReader(const std::string &filename);
	static BufferReaderProvider& getInstance();
	~BufferReaderProvider();

private:
	BufferReaderProvider();
	BufferReaderProvider(const BufferReaderProvider&);
	std::vector<BufferReaderProviderImpl*> providers;
};

#endif // BUFFERREADERPROVIDER_H
