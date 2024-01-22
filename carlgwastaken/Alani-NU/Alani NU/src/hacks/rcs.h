#pragma once
#include "..\math\vector.hpp"

class CRCS
{
public:
	void RcsLoop();

	Vector oldAngle, newAngle;
};

inline CRCS rcs;