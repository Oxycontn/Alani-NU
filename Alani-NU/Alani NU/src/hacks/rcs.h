#pragma once
#include "..\math\vector.hpp"

class CRCS
{
public:
	void RCS();

	Vector oldAngle, newAngle;
};

inline CRCS rcs;