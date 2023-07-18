#include <iostream>
#include <fstream>
#include <sstream>
#include <numbers>
#include <utility>

#include <cstdlib>
#include <cmath>

#include "opengl.h"

// ---------------------------------------------------

#define DEBUG_SHOW_CENTER_LINE

// ---------------------------------------------------

Graphics::Opengl::Shader::Shader (const GLenum shader_type_, const char *fname_)
: shader_type(shader_type_),
  fname(fname_)
{
	this->shader_id = glCreateShader(this->shader_type);
}

void Graphics::Opengl::Shader::compile ()
{
	// First, read the whole shader file to memory.
	// I usually do this in C, but wanted to do in C++ for a change, but Jesus...

	std::ifstream t(this->fname);
	std::stringstream str_stream;
	str_stream << t.rdbuf();
	std::string buffer = str_stream.str();

	dprint( "loaded shader (" << this->fname << ")" << std::endl )
	//dprint( buffer )
	
	const char *c_str = buffer.c_str();
	glShaderSource(this->shader_id, 1, ( const GLchar ** )&c_str, nullptr);
	glCompileShader(this->shader_id);

	GLint status;
	glGetShaderiv(this->shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		std::cout << this->fname << " shader compilation failed" << std::endl;

		GLint logSize = 0;
		glGetShaderiv(this->shader_id, GL_INFO_LOG_LENGTH, &logSize);

		char *berror = (char*)malloc(logSize);
		ASSERT(berror != nullptr);

		glGetShaderInfoLog(this->shader_id, logSize, nullptr, berror);

		printf("%s\n", berror);
		free(berror);

		exit(1);
	}
}

Graphics::Opengl::Program::Program ()
{
	this->vs = nullptr;
	this->fs = nullptr;
	this->program_id = glCreateProgram();
}

void Graphics::Opengl::Program::attach_shaders ()
{
	glAttachShader(this->program_id, this->vs->shader_id);
	glAttachShader(this->program_id, this->fs->shader_id);
}

void Graphics::Opengl::Program::link_program ()
{
	glLinkProgram(this->program_id);
}

void Graphics::Opengl::Program::use_program ()
{
	glUseProgram(this->program_id);
}

Graphics::Opengl::ProgramTriangle::ProgramTriangle ()
	: Program ()
{
	static_assert(sizeof(Vertex) == 32);
	static_assert((sizeof(Vertex) / sizeof(GLfloat)) == 8);

	this->vs = new Shader(GL_VERTEX_SHADER, "shaders/triangles.vert");
	this->vs->compile();

	this->fs = new Shader(GL_FRAGMENT_SHADER, "shaders/triangles.frag");
	this->fs->compile();

	this->attach_shaders();

	glBindAttribLocation(this->program_id, std::to_underlying(Attrib::Position), "i_position");
	glBindAttribLocation(this->program_id, std::to_underlying(Attrib::Offset), "i_offset");
	glBindAttribLocation(this->program_id, std::to_underlying(Attrib::Color), "i_color");

	this->link_program();

	glGenVertexArrays(1, &(this->vao));
	glGenBuffers(1, &(this->vbo));
}

void Graphics::Opengl::ProgramTriangle::bind_vertex_array ()
{
	glBindVertexArray(this->vao);
}

void Graphics::Opengl::ProgramTriangle::bind_vertex_buffer ()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
}

void Graphics::Opengl::ProgramTriangle::setup_vertex_array ()
{
	uint32_t pos, length;

	glEnableVertexAttribArray( std::to_underlying(Attrib::Position) );
	glEnableVertexAttribArray( std::to_underlying(Attrib::Offset) );
	glEnableVertexAttribArray( std::to_underlying(Attrib::Color) );

	pos = 0;
	length = 2;
	glVertexAttribPointer( std::to_underlying(Attrib::Position), length, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( void * )(pos * sizeof(float)) );
	
	pos += length;
	length = 2;
	glVertexAttribPointer( std::to_underlying(Attrib::Offset), length, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( void * )(pos * sizeof(float)) );
	
	pos += length;
	length = 4;
	glVertexAttribPointer( std::to_underlying(Attrib::Color), length, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( void * )(pos * sizeof(float)) );
}

void Graphics::Opengl::ProgramTriangle::upload_vertex_buffer ()
{
	uint32_t n = this->triangle_buffer.get_vertex_buffer_used();
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * n, this->triangle_buffer.get_vertex_buffer(), GL_DYNAMIC_DRAW);
}

void Graphics::Opengl::ProgramTriangle::upload_projection_matrix (const Matrix4d& m)
{
	glUniformMatrix4fv( glGetUniformLocation(this->program_id, "u_projection_matrix"), 1, GL_TRUE, m.get_raw() );
	//dprintln( "projection matrix sent to GPU" )
}

void Graphics::Opengl::ProgramTriangle::draw ()
{
	uint32_t n = this->triangle_buffer.get_vertex_buffer_used();
	glDrawArrays(GL_TRIANGLES, 0, n);
}

void Graphics::Opengl::ProgramTriangle::debug ()
{
	uint32_t n = this->triangle_buffer.get_vertex_buffer_used();

	for (uint32_t i=0; i<n; i++) {
		Vertex *v = this->triangle_buffer.get_vertex(i);

		if ((i % 3) == 0)
			std::cout << std::endl;

		std::cout << "vertex[" << i
			<< "] x=" << v->x
			<< " y= " << v->y
			<< " offset_x= " << v->offset_x
			<< " offset_y= " << v->offset_y
			<< " r= " << v->r
			<< " g= " << v->g
			<< " b= " << v->b
			<< " a= " << v->a
			<< std::endl;
	}
}

Graphics::Opengl::Renderer::Renderer (const uint32_t window_width_px_, const uint32_t window_height_px_)
	: Graphics::Renderer (window_width_px_, window_height_px_)
{
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	this->sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->window_width_px, this->window_height_px, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	this->sdl_gl_context = SDL_GL_CreateContext(this->sdl_window);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}

	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	glDisable(GL_DEPTH_TEST);

	this->background_color = Graphics::config_background_color;

	glClearColor(this->background_color.r, this->background_color.g, this->background_color.b, 1.0);
	glViewport(0, 0, this->window_width_px, this->window_height_px);

	this->circle_factory_low_def = new CircleFactory(64);
	//this->opengl_circle_factory_high_def = new Opengl::CircleFactory(Config::opengl_high_def_triangles);

	this->load_opengl_programs();

	dprintln( "loaded opengl stuff" )

	this->wait_next_frame();
}

void Graphics::Opengl::Renderer::load_opengl_programs ()
{
	this->program_triangle = new ProgramTriangle;

	dprintln( "loaded opengl triangle program" )

	this->program_triangle->use_program();
	
	this->program_triangle->bind_vertex_array();
	this->program_triangle->bind_vertex_buffer();

	this->program_triangle->setup_vertex_array();

	dprintln( "generated and binded opengl world vertex array/buffer" )
}

Graphics::Opengl::Renderer::~Renderer ()
{
	delete this->program_triangle;
	delete this->circle_factory_low_def;

	SDL_GL_DeleteContext(this->sdl_gl_context);
	SDL_DestroyWindow(this->sdl_window);
}

void Graphics::Opengl::Renderer::wait_next_frame ()
{
	glClear( GL_COLOR_BUFFER_BIT );

	this->program_triangle->clear();
}

void Graphics::Opengl::Renderer::draw_circle (const ShapeCircle& circle, const Vector& offset, const Graphics::Color& color)
{
	/*Graphics::ShapeRect rect(circle.get_radius()*2.0f, circle.get_radius()*2.0f);
	rect.set_delta(circle.get_delta());
	this->draw_rect(rect, offset, color);*/

	const Vector local_pos = circle.get_delta();
	const uint32_t n_vertices = this->circle_factory_low_def->get_n_vertices();
	ProgramTriangle::Vertex *vertices = this->program_triangle->alloc_vertices(n_vertices);

	this->circle_factory_low_def->fill_vertex_buffer(
		circle.get_radius(),
		&vertices[0].x,
		&vertices[0].y,
		ProgramTriangle::get_stride_in_floats()
		);

	for (uint32_t i=0; i<n_vertices; i++) {
		vertices[i].x += local_pos.x;
		vertices[i].y += local_pos.y;
		vertices[i].offset_x = offset.x;
		vertices[i].offset_y = offset.y;
		vertices[i].r = color.r;
		vertices[i].g = color.g;
		vertices[i].b = color.b;
		vertices[i].a = color.a;
	}
}

void Graphics::Opengl::Renderer::draw_rect (const ShapeRect& rect, const Vector& offset, const Graphics::Color& color)
{
	const uint32_t n_vertices = 6;
	const Vector local_pos = rect.get_delta();
	//const Vector world_pos = Vector(4.0f, 4.0f);
	
#if 0
	dprint( "local_pos:" )
	Mylib::Math::println(world_pos);

	dprint( "clip_pos:" )
	Mylib::Math::println(clip_pos);
//exit(1);
#endif

	ProgramTriangle::Vertex *vertices = this->program_triangle->alloc_vertices(n_vertices);

	// draw first triangle

	// upper left vertex
	vertices[0].x = local_pos.x - rect.get_w()*0.5f;
	vertices[0].y = local_pos.y - rect.get_h()*0.5f;

	// down right vertex
	vertices[1].x = local_pos.x + rect.get_w()*0.5f;
	vertices[1].y = local_pos.y + rect.get_h()*0.5f;

	// down left vertex
	vertices[2].x = local_pos.x - rect.get_w()*0.5f;
	vertices[2].y = local_pos.y + rect.get_h()*0.5f;

	// draw second triangle

	// upper left vertex
	vertices[3].x = local_pos.x - rect.get_w()*0.5f;
	vertices[3].y = local_pos.y - rect.get_h()*0.5f;

	// upper right vertex
	vertices[4].x = local_pos.x + rect.get_w()*0.5f;
	vertices[4].y = local_pos.y - rect.get_h()*0.5f;

	// down right vertex
	vertices[5].x = local_pos.x + rect.get_w()*0.5f;
	vertices[5].y = local_pos.y + rect.get_h()*0.5f;

	for (uint32_t i=0; i<n_vertices; i++) {
		vertices[i].offset_x = offset.x;
		vertices[i].offset_y = offset.y;
		vertices[i].r = color.r;
		vertices[i].g = color.g;
		vertices[i].b = color.b;
		vertices[i].a = color.a;
	}
}

void Graphics::Opengl::Renderer::setup_projection_matrix (const ProjectionMatrixArgs& args)
{
	const Vector normalized_clip_init = args.clip_init_norm;
	const Vector normalized_clip_end = args.clip_end_norm;

	const Vector normalized_clip_size = normalized_clip_end - normalized_clip_init;
	const float normalized_clip_aspect_ratio = normalized_clip_size.x / normalized_clip_size.y;

	//const float max_norm_length = std::max(normalized_clip_size.x, normalized_clip_size.y);
	//const float max_opengl_length = max_norm_length * 2.0f;
	const float opengl_length = 2.0f;
	const float opengl_window_aspect_ratio = this->window_aspect_ratio;

	/*
		1.0f (norm_length) -> 2.0f (opengl_length)
		norm_coord -> opengl_coord
	*/

	Vector opengl_clip_scale_mirror;

	if (normalized_clip_aspect_ratio >= 1.0f)
		opengl_clip_scale_mirror = Vector(opengl_length, opengl_length*opengl_window_aspect_ratio);
	else
		opengl_clip_scale_mirror = Vector(opengl_length/opengl_window_aspect_ratio, opengl_length);
	
	// mirror y axis
	opengl_clip_scale_mirror.y = -opengl_clip_scale_mirror.y;

	const Vector world_size = args.world_end - args.world_init;
	
	const float world_screen_width = std::min(args.world_screen_width, world_size.x);
	const float world_screen_height = std::min(world_screen_width / normalized_clip_aspect_ratio, world_size.y);

	const Vector world_screen_size = Vector(world_screen_width, world_screen_height);

	const float normalized_scale_factor = normalized_clip_size.x / world_screen_size.x;
	//const float normalized_scale_factor = 1.0f / world_screen_size.x;

	Vector world_camera = args.world_camera_focus - Vector(world_screen_size.x*0.5f, world_screen_size.y*0.5f);

	dprintln( "------------------------------" )
	//dprint( "world_camera PRE: " ) Mylib::Math::println(world_camera);

	if (args.force_camera_inside_world) {
		if (world_camera.x < args.world_init.x)
			world_camera.x = args.world_init.x;
		else if ((world_camera.x + world_screen_size.x) > args.world_end.x)
			world_camera.x = args.world_end.x - world_screen_size.x;

		//dprint( "world_camera POS: " ) Mylib::Math::println(world_camera);

		if (world_camera.y < args.world_init.y)
			world_camera.y = args.world_init.y;
		else if ((world_camera.y + world_screen_size.y) > args.world_end.y)
			world_camera.y = args.world_end.y - world_screen_size.y;
	}

#if 1
	dprint( "normalized_clip_init: " ) Mylib::Math::println(normalized_clip_init);
	dprint( "normalized_clip_end: " ) Mylib::Math::println(normalized_clip_end);
	dprint( "normalized_clip_size: " ) Mylib::Math::println(normalized_clip_size);
	dprintln( "normalized_clip_aspect_ratio: " << normalized_clip_aspect_ratio )
	dprintln( "normalized_scale_factor: " << normalized_scale_factor )
	//dprintln( "max_norm_length: " << max_norm_length )
	//dprintln( "max_opengl_length: " << max_opengl_length )
	dprint( "opengl_clip_scale_mirror: " ) Mylib::Math::println(opengl_clip_scale_mirror);
	dprint( "world_size: " ) Mylib::Math::println(world_size);
	dprint( "world_screen_size: " ) Mylib::Math::println(world_screen_size);
	dprint( "args.world_camera_focus: " ) Mylib::Math::println(args.world_camera_focus);
	dprint( "world_camera: " ) Mylib::Math::println(world_camera);
//exit(1);
#endif

	Matrix4d translate_subtract_one;
	translate_subtract_one.set_translate( Vector(-1.0f, +1.0f) );
//	dprintln( "translation to clip init:" ) translate_to_clip_init.println();

	Matrix4d opengl_scale_mirror;
	opengl_scale_mirror.set_scale(opengl_clip_scale_mirror);
//	dprintln( "scale matrix:" ) Mylib::Math::println(scale);
//exit(1);

	Matrix4d translate_to_normalized_clip_init;
	translate_to_normalized_clip_init.set_translate(normalized_clip_init);
//	dprintln( "translation to clip init:" ) translate_to_clip_init.println();

	Matrix4d scale_normalized;
	scale_normalized.set_scale(Vector(normalized_scale_factor, normalized_scale_factor));
//	dprintln( "scale matrix:" ) Mylib::Math::println(scale);
//exit(1);

	Matrix4d translate_camera;
	translate_camera.set_translate(-world_camera);
//	dprintln( "translation matrix:" ) translate_camera.println();

	this->projection_matrix = 
		(((translate_subtract_one
		* opengl_scale_mirror)
		* translate_to_normalized_clip_init)
		* scale_normalized)
		* translate_camera;
	//this->projection_matrix = scale * translate_camera;
	//dprintln( "final matrix:" ) this->projection_matrix.println();
}

void Graphics::Opengl::Renderer::render ()
{
#ifdef DEBUG_SHOW_CENTER_LINE
{

}
#endif
	this->program_triangle->upload_projection_matrix(this->projection_matrix);
	this->program_triangle->upload_vertex_buffer();
	this->program_triangle->draw();
	SDL_GL_SwapWindow(this->sdl_window);
}
