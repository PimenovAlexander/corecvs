#include "vsfmIo.h"

#include "global.h"

#define BYTES_MAGIC(a, b, c, d) \
	((uint32_t(d) << 24) | (uint32_t(c) << 16) | (uint32_t(b) << 8) | uint32_t(a))


const uint32_t VsfmSiftIO::MAGIC_SIFT = BYTES_MAGIC(   'S', 'I', 'F', 'T');
const uint32_t VsfmSiftIO::MAGIC_V40  = BYTES_MAGIC(   'V', '4', '.', '0');
const uint32_t VsfmSiftIO::MAGIC_EOF  = BYTES_MAGIC('\xff', 'E', 'O', 'F');
#undef BYTES_MAGIC


std::ostream& operator<<(std::ostream& os, const SiftFeature& f)
{
	os << f.x << " " << f.y << " " << f.scale << " " << f.orientation << std::endl;

	for (size_t i = 0; i < SiftFeature::DESCRIPTOR_WIDTH; ++i)
	{
		os << (int)f.data[i] << " ";

		// TODO: do we really need this linebreak?!
		if ((i + 1) % 20 == 0)
		{
			os << std::endl;
		}
	}
	os << std::endl;
	return os;
}

std::istream& operator>>(std::istream& is, SiftFeature& f)
{
	is >> f.x >> f.y >> f.scale >> f.orientation;

	for (size_t i = 0; i < SiftFeature::DESCRIPTOR_WIDTH; ++i)
	{
		int a;
		is >> a;
		f.data[i] = a;
	}
	return is;

}

void VsfmSiftIO::writeAscii(std::ostream& os, const std::vector<SiftFeature>& features)
{
	os << features.size() << " " << SiftFeature::DESCRIPTOR_WIDTH << std::endl;

	for (std::vector<SiftFeature>::const_iterator it = features.begin(); it != features.end(); ++it)
	{
		os << *it;
	}
}

void VsfmSiftIO::readAscii(std::istream& is, std::vector<SiftFeature>& features)
{
	size_t N, sz;
	is >> N >> sz;

	// it seems that VSFM toolkit does not support other descriptors widths,
	// so just "check" that size is correct
    CORE_ASSERT_TRUE_S(sz == SiftFeature::DESCRIPTOR_WIDTH);

	features.resize(N);

	for (std::vector<SiftFeature>::iterator it = features.begin(); it != features.end(); ++it)
	{
        CORE_ASSERT_TRUE_S(is);
		is >> *it;
	}
}

void VsfmSiftIO::readBinary(std::istream& is, std::vector<SiftFeature>& features)
{
	SiftHeader header;
	is.read((char*)&header, sizeof(header));

	features.clear();
	features.resize(header.npoint);

    CORE_ASSERT_TRUE_S(header.descriptorSize == SiftFeature::DESCRIPTOR_WIDTH &&
			header.keyPointSize == SiftFeature::BINARY_FLOATS &&
			header.magicName == VsfmSiftIO::MAGIC_SIFT &&
			header.magicVersion == VsfmSiftIO::MAGIC_V40);

	size_t i = 0;
	for (std::vector<SiftFeature>::iterator f = features.begin(); f != features.end(); ++f){
		is.read((char*)&f->x, sizeof(float) * SiftFeature::BINARY_FLOATS);
		f->importance = i++;
	}

	for (std::vector<SiftFeature>::iterator f = features.begin(); f != features.end(); ++f){
		is.read((char*)f->data.data(), header.descriptorSize);
	}

	uint32_t eof;
	is.read((char*)&eof, sizeof(eof));
    CORE_ASSERT_TRUE_S(eof == VsfmSiftIO::MAGIC_EOF);
}

void VsfmSiftIO::writeBinary(std::ostream& os, const std::vector<SiftFeature>& features)
{
	SiftHeader header;
	header.npoint = (uint32_t)features.size();

	os.write((char*)&header, sizeof(header));
	for (std::vector<SiftFeature>::const_iterator f = features.begin(); f != features.end(); ++f)
	{
		os.write((char*)&f->x, sizeof(float) * SiftFeature::BINARY_FLOATS);
	}

	for (std::vector<SiftFeature>::const_iterator f = features.begin(); f != features.end(); ++f)
	{
		os.write((char*)f->data.data(), SiftFeature::DESCRIPTOR_WIDTH);
	}

	os.write((char*)&VsfmSiftIO::MAGIC_EOF, sizeof(VsfmSiftIO::MAGIC_EOF));
}
