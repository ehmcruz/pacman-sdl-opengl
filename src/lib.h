#ifndef __PACMAN_SDL_OPENGL_LIB_HEADER_H__
#define __PACMAN_SDL_OPENGL_LIB_HEADER_H__

#include <iostream>
#include <string>

namespace Mylib
{

// ---------------------------------------------------

#define DEBUG

// ---------------------------------------------------

#define blikely(x)       __builtin_expect((x),1)
#define bunlikely(x)     __builtin_expect((x),0)

// ---------------------------------------------------

#ifdef DEBUG
	#define dprint(STR) { std::cout << STR; }
#else
	#define dprint(STR)
#endif

// ---------------------------------------------------

#define OO_ENCAPSULATE(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline void set_##VAR (TYPE VAR) { \
			this->VAR = VAR; \
		} \
		inline TYPE get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_REFERENCE(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline void set_##VAR (TYPE& VAR) { \
			this->VAR = VAR; \
		} \
		inline TYPE& get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_REFERENCE_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE& get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define ASSERT(V) ASSERT_PRINT(V, "bye!\n")

#define ASSERT_PRINT(V, STR) \
	if (bunlikely(!(V))) { \
		std::string assert_str_ = (STR); \
		std::cout << "sanity error!" << std::endl << "file " << __FILE__ << " at line " << __LINE__ << " assertion failed!" << std::endl << #V << std::endl; \
		std::cout << assert_str_ << std::endl; \
		exit(1); \
	}

// ---------------------------------------------------

template<typename T, int rows, int cols>
class Static_matrix
{
private:
	T storage[rows * cols];

public:
	inline T* get_raw ()
	{
		return this->storage;
	}

	inline T& operator() (int row, int col)
	{
		return this->storage[row*cols + col];
	}
};

// ---------------------------------------------------

template<typename T>
class Matrix
{
private:
	T *storage;
	OO_ENCAPSULATE_READONLY(uint32_t, rows)
	OO_ENCAPSULATE_READONLY(uint32_t, cols)

public:
	Matrix ()
	{
		this->storage = nullptr;
	}

	Matrix (uint32_t rows, uint32_t cols)
	{
		this->storage = nullptr;
		this->setup(rows, cols);
	}

	~Matrix ()
	{
		if (this->storage != nullptr)
			delete[] this->storage;
	}

	void setup (uint32_t rows, uint32_t cols)
	{
		if (this->storage != nullptr)
			delete[] this->storage;
		this->storage = new T[rows * cols];
		this->rows = rows;
		this->cols = cols;
	}

	inline T* get_raw ()
	{
		return this->storage;
	}

	inline T& get (int row, int col)
	{
		return this->storage[row*this->cols + col];
	}

	inline T& operator() (int row, int col)
	{
		return this->get(row, col);
	}
};

// ---------------------------------------------------

} // end namespace Mylib

#endif