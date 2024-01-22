#include <thread>

#include "../math/vector.hpp"

class CEntityLoop
{
public:
	void EspThread();

	void ReadLocalPlayer();
	
	void EspLoop();
    void Bone();

	// as discussed, no point in making entire classes for 1 / 2 functions.
	// if they're inheriting, that's a different story.
	void RenderEsp();
	void RenderWorld();
	void RenderFov();
};

inline CEntityLoop entityloop;