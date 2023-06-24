#include <iostream>
#include "lib.h"

namespace Game
{

// ---------------------------------------------------

Probability::Probability ()
{
	std::random_device rd;

	this->rgenerator.seed( rd() );
}

// ---------------------------------------------------

} // end namespace Game