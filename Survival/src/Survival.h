#pragma once

#ifndef _SURVIVAL_HEADER
#define _SURVIVAL_HEADER

#include <Sparky.h>

using namespace sp;
using namespace graphics;
using namespace maths;

class Survival : public Application
{
public:
	Survival();

	~Survival();

	void Init() override;
};

#endif