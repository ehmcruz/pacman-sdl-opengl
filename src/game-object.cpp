#include "game-world.h"
#include "game-object.h"

void Game::Object::physics (const float dt)
{
	this->x += this->vx * dt;
	this->y += this->vy * dt;
}

void Game::Shape::apply_delta (const uint32_t n_vertices, float *x, float *y, const uint32_t stride)
{
	uint32_t j = 0;

	for (uint32_t i=0; i<n_vertices; i++) {
		x[j] += this->dx;
		y[j] += this->dy;
		j += stride;
	}
}

uint32_t Game::ShapeCircle::get_n_vertices ()
{
	return this->factory->get_n_vertices();
}

void Game::ShapeCircle::push_vertices (float *x, float *y, const uint32_t stride)
{
	const uint32_t n = this->get_n_vertices();

	this->factory->fill_vertex_buffer(this->radius, x, y, stride);
	this->apply_delta(n, x, y, stride);
}

void Game::ShapeRect::push_vertices (float *x, float *y, const uint32_t stride)
{
	const float half_w = this->w * 0.5f;
	const float half_h = this->h * 0.5f;
	uint32_t i = 0;

	// let's draw clockwise

	// first triangle
	x[i] = -half_w;
	y[i] = -half_h;
	i += stride;
	x[i] = half_w;
	y[i] = half_h;
	i += stride;
	x[i] = -half_w;
	y[i] = half_h;

	i += stride;

	// second triangle
	x[i] = -half_w;
	y[i] = -half_h;
	i += stride;
	x[i] = half_w;
	y[i] = -half_h;
	i += stride;
	x[i] = half_w;
	y[i] = half_h;

	this->apply_delta(this->get_n_vertices(), x, y, stride);
}

Game::Player::Player ()
{
	this->shape = new ShapeCircle( this, Config::pacman_radius, Main::get()->get_opengl_circle_factory_low_def() );
	this->x = 0.0f;
	this->y = 0.0f;
	this->vx = 0.0f;
	this->vy = 0.0f;

	dprint( "player created" << std::endl )
}

Game::Player::~Player ()
{
	delete this->shape;
}

void Game::Player::render (const float dt)
{
	Opengl::ProgramTriangle::Vertex *vertices;
	Opengl::ProgramTriangle *program;
	uint32_t n_vertices;

	n_vertices = this->shape->get_n_vertices();
	dprint( "player allocating space for " << n_vertices << " vertices in vertex_buffer" << std::endl )

	program = Main::get()->get_opengl_program_triangle();
	vertices = program->alloc_vertices(n_vertices);

	this->shape->push_vertices( &(vertices->x), &(vertices->y), Opengl::ProgramTriangle::get_stride() );

	for (uint32_t i=0; i<n_vertices; i++) {
		vertices[i].offset_x = this->x;
		vertices[i].offset_y = this->y;
		vertices[i].r = 1.0f;
		vertices[i].g = 0.0f;
		vertices[i].b = 0.0f;
		vertices[i].a = 1.0f;
	}
}