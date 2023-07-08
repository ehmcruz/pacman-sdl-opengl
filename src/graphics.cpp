#include <iostream>
#include <fstream>
#include <sstream>
#include <numbers>
#include <utility>

#include <cstdlib>
#include <cmath>

#include "graphics.h"


Graphics::CircleFactory::CircleFactory (const uint32_t n_triangles_)
	: n_triangles(n_triangles_)
{
	this->table_cos = new float[this->n_triangles];
	this->table_sin = new float[this->n_triangles];

	/*
		cos(angle) = x / radius
		sin(angle) = y / radius
		
		x = cos(angle) * radius
		y = sin(angle) * radius

		2*pi radians is equal to 360 degrees
	*/

	const double delta = (2.0 * std::numbers::pi) / static_cast<double>(this->n_triangles);
	double angle = delta;

/*
	dprint( std::endl )
	dprint( "delta = " << delta << std::endl )
	dprint( std::endl )
*/

	for (uint32_t i=0; i<this->n_triangles; i++) {
		this->table_cos[i] = static_cast<float>( cos(angle) );
		this->table_sin[i] = static_cast<float>( sin(angle) );

	/*
		dprint( "cos(" << angle << ") = " << this->table_cos[i] << std::endl )
		dprint( "sin(" << angle << ") = " << this->table_sin[i] << std::endl )
		dprint( std::endl )
	*/

		angle += delta;
	}
}

Graphics::CircleFactory::~CircleFactory ()
{
	delete[] this->table_cos;
	delete[] this->table_sin;
}

void Graphics::CircleFactory::fill_vertex_buffer (const float radius, float *x, float *y, const uint32_t stride) const
{
	uint32_t j;
	float previous_x, previous_y;

	/*
		For each triangle:
			- first vertex is the center (0.0f, 0.0f)
			- second vertex is the previous calculated vertex (from previous triangle)
			- third vertex is the new vertex
	*/

	// for the first triangle
	previous_x = radius;
	previous_y = 0.0f;

	j = 0;
	for (uint32_t i=0; i<this->n_triangles; i++) {
		// first vertex
		x[j] = 0.0f;
		y[j] = 0.0f;

		j += stride;

		// second vertex
		x[j] = previous_x;
		y[j] = previous_y;

		j += stride;

		// third vertex
		x[j] = this->table_cos[i] * radius;
		y[j] = this->table_sin[i] * radius;

		previous_x = x[j];
		previous_y = y[j];

		j += stride;
	}
}

void Graphics::ProjectionMatrix::setup (const Args&& args)
{
	ProjectionMatrix& m = *this;

	m(0,0) = 2.0f / (args.right - args.left);
	m(0,1) = 0.0f;
	m(0,2) = 0.0f;
	m(0,3) = 0.0f;

	m(1,0) = 0.0f;
	m(1,1) = 2.0f / (args.top - args.bottom);
	m(1,2) = 0.0f;
	m(1,3) = 0.0f;

	m(2,0) = 0.0f;
	m(2,1) = 0.0f;
	m(2,2) = -2.0f / (args.zfar - args.znear);
	m(2,3) = 0.0f;

	m(3,0) = -(args.right + args.left) / (args.right - args.left);
	m(3,1) = -(args.top + args.bottom) / (args.top - args.bottom);
	m(3,2) = -(args.zfar + args.znear) / (args.zfar - args.znear);
	m(3,3) = 1.0f;
}
