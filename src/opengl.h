#ifndef __PACMAN_SDL_OPENGL_OPENGL_HEADER_H__
#define __PACMAN_SDL_OPENGL_OPENGL_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <cstring>

#include <string>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>

namespace Opengl
{

// ---------------------------------------------------

class Program;

// ---------------------------------------------------

class Shader
{
protected:
	OO_ENCAPSULATE_READONLY(GLuint, shader_id)
	OO_ENCAPSULATE_READONLY(GLenum, shader_type)
	OO_ENCAPSULATE_REFERENCE(std::string, fname)

public:
	Shader (const GLenum shader_type, const char *fname);
	void compile ();

	friend class Program;
};

// ---------------------------------------------------

class ProjectionMatrix: public Mylib::StaticMatrix<float, 4, 4>
{
public:
	struct Args {
		float left;
		float right;
		float top;
		float bottom;
		float znear;
		float zfar;
	};

	void setup (const Args&& args);
};

// ---------------------------------------------------

class Program
{
protected:
	OO_ENCAPSULATE_READONLY(GLuint, program_id)
	OO_ENCAPSULATE(Shader*, vs)
	OO_ENCAPSULATE(Shader*, fs)

public:
	Program ();
	void attach_shaders ();
	void link_program ();
	void use_program ();
};

// ---------------------------------------------------

template <typename T, int grow_factor=4096>
class VertexBuffer
{
protected:
	uint32_t vertex_buffer_capacity;
	
	OO_ENCAPSULATE_READONLY(T*, vertex_buffer)
	OO_ENCAPSULATE_READONLY(uint32_t, vertex_buffer_used)

	void realloc (const uint32_t target_capacity)
	{
		uint32_t old_capacity = this->vertex_buffer_capacity;
		T *old_buffer = this->vertex_buffer;

		this->vertex_buffer_capacity += grow_factor;

		if (this->vertex_buffer_capacity < target_capacity)
			this->vertex_buffer_capacity = target_capacity;
		this->vertex_buffer = new T[this->vertex_buffer_capacity];

		memcpy(this->vertex_buffer, old_buffer, old_capacity * sizeof(T));

		delete[] old_buffer;
	}

public:
	VertexBuffer ()
	{
		static_assert(grow_factor > 0);

		this->vertex_buffer_capacity = grow_factor; // can't be zero
		this->vertex_buffer = new T[this->vertex_buffer_capacity];

		this->vertex_buffer_used = 0;
	}

	~VertexBuffer ()
	{
		if (this->vertex_buffer != nullptr)
			delete[] this->vertex_buffer;
	}

	inline T* get_vertex (const uint32_t i)
	{
		return (this->vertex_buffer + i);
	}

	inline T* alloc_vertices (const uint32_t n)
	{
		const uint32_t free_space = this->vertex_buffer_capacity - this->vertex_buffer_used;

		if (free_space < n) [[unlikely]]
			this->realloc(this->vertex_buffer_used + n);
		
		T *vertices = this->vertex_buffer + this->vertex_buffer_used;
		this->vertex_buffer_used += n;

		return vertices;
	}

	inline void clear ()
	{
		this->vertex_buffer_used = 0;
	}
};

// ---------------------------------------------------

class ProgramTriangle: public Program
{
protected:
	enum Attrib {
		attrib_position,
		attrib_offset,
		attrib_color
	} t_attrib_id;

public:
	struct Vertex {
		GLfloat x; // local x,y coords
		GLfloat y;
		GLfloat offset_x; // global x,y coords, which are added to the local coords
		GLfloat offset_y;
		GLfloat r;
		GLfloat g;
		GLfloat b;
		GLfloat a; // alpha
	};

	OO_ENCAPSULATE_READONLY(GLuint, vao) // vertex array descriptor id
	OO_ENCAPSULATE_READONLY(GLuint, vbo) // vertex buffer id

protected:
	VertexBuffer<Vertex, 8192> triangle_buffer;

public:
	ProgramTriangle ();

	consteval static uint32_t get_stride ()
	{
		return (sizeof(Vertex) / sizeof(GLfloat));
	}

	inline void clear ()
	{
		this->triangle_buffer.clear();
	}

	inline Vertex* alloc_vertices (const uint32_t n)
	{
		return this->triangle_buffer.alloc_vertices(n);
	}

	void bind_vertex_array ();
	void bind_vertex_buffer ();
	void setup_vertex_array ();
	void upload_vertex_buffer ();
	void upload_projection_matrix (const ProjectionMatrix& m);
	void draw ();

	void debug ();
};

// ---------------------------------------------------

class CircleFactory
{
private:
	float *table_sin;
	float *table_cos;
	const uint32_t n_triangles;

public:
	CircleFactory (const uint32_t n_triangles_);
	~CircleFactory ();

	inline uint32_t get_n_vertices () const
	{
		return (this->n_triangles * 3);
	}

	void fill_vertex_buffer (const float radius, float *x, float *y, const uint32_t stride) const;
};

// ---------------------------------------------------

} // end namespace Opengl

#endif