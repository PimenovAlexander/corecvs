#ifndef RUNTIMETYPEBUFFER_H
#define RUNTIMETYPEBUFFER_H

#include "global.h"

#include <cstring>
#include <string>
#include <iostream>
#include <cassert>
#include <stdint.h> 

#ifndef CV_8U
#define CV_8U 0
#endif

#ifndef CV_32F
#define CV_32F 5
#endif

namespace BufferType {
	enum RuntimeBufferDataType {
		U8,
		F32
	};
};


// TODO: add row-aligned allocation & getters
class RuntimeTypeBuffer {
public:
	RuntimeTypeBuffer() : data(0), rows(0), cols(0), sz(0), type(BufferType::U8) {}
	RuntimeTypeBuffer(const size_t &rows, const size_t &cols, const int &type =BufferType::U8) : rows(rows), cols(cols), sz(getTypeSize(type)), type(type) {
		allocate();
	}
	RuntimeTypeBuffer(const size_t &rows, const size_t &cols, const void* data, const int &type = BufferType::U8) : rows(rows), cols(cols), sz(getTypeSize(type)), type(type) {
		allocate();
		copy((uint8_t*)data);
	}

	~RuntimeTypeBuffer() {
		free(data);
	}

	RuntimeTypeBuffer(const RuntimeTypeBuffer &b) : rows(b.rows), cols(b.cols), sz(b.sz), type(b.type) {
		allocate();
		copy(b.data);
	}

	RuntimeTypeBuffer& operator=(const RuntimeTypeBuffer &b) {
		if(this == &b)
			return *this;
		free(data);
		rows = b.rows;
		cols = b.cols;
		sz = b.sz;
		type = b.type;

		allocate();
		copy(b.data);
		return *this;
	}

	bool isValid() const {
		return data != 0;
	}

	size_t getRowSize() const {
		return sz * cols;
	}
	static size_t getTypeSize(int type) {
		switch(type) {
			case BufferType::U8:
				return 1;
			case BufferType::F32:
				return 4;
			default:
                CORE_ASSERT_FAIL_P(("RuntimeTypeBuffer::getTypeSize(%d): bad type", type));
                return 0;
		}
	}
	static size_t getTypeFromCvType(int type) {
		switch(type) {
			case CV_8U:
				return BufferType::U8;
			case CV_32F:
				return BufferType::F32;
			default:
                CORE_ASSERT_FAIL_P(("RuntimeTypeBuffer::getCvType(%d): bad type", type));
                return 0;
		}
	}
	size_t getCvType() const {
		switch(type) {
			case BufferType::U8:
				return CV_8U;
			case BufferType::F32:
				return CV_32F;
			default:
                CORE_ASSERT_FAIL_P(("RuntimeTypeBuffer::getCvType(): incorrect type %d", type));
                return 0;
		}
	}
	size_t getElSize() const {
		return sz;
	}

	size_t getDataSize() const {
		return rows * getRowSize();
	}

	template<typename T>
	T& at(const size_t &i, const size_t &j) {
		return *(T*)(data + sz * (i * cols + j));
	}
	template<typename T>
	const T& at(const size_t &i, const size_t &j) const {
		return *(T*)(data + sz * (i * cols + j));
	}
	template<typename T>
	T* row(const size_t &i) {
		return (T*)(data + sz * (i * cols ));
	}
	// FIXME
	template<typename T>
	T* row(const size_t &i) const {
		return (T*)(data + sz * (i * cols));
	}
	inline size_t getRows() const {
		return rows;
	}
	inline size_t getCols() const {
		return cols;
	}
	int getType() const {
		return type;
	}

	void load(std::istream &is);
	void save(std::ostream &os) const;
	void load(const std::string &filename);
	void save(const std::string &filename) const;
	friend std::ostream& operator<<(std::ostream &os, const RuntimeTypeBuffer &b);
	friend std::istream& operator>>(std::istream &is, RuntimeTypeBuffer &b);
private:
	void allocate() {
		size_t bytes = getDataSize();
		data = (uint8_t*)malloc(bytes);
	}
	void copy(uint8_t *src) {
		memcpy(data, src, getDataSize());
	}
	uint8_t *data;
	size_t rows;
	size_t cols;
	size_t sz;
	int type;
};

std::ostream& operator<<(std::ostream &os, const RuntimeTypeBuffer &b);
std::istream& operator>>(std::istream &is, RuntimeTypeBuffer &b);

#endif
