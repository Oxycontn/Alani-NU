#pragma once
#include "..\math\vector.hpp"
#include "..\mem\memory.h"

class CWEntity
{
public:
	CWEntity(uintptr_t entityController);

	static CWEntity* EntityController(uintptr_t entityController);

	static uintptr_t DesignerNamePtr(uintptr_t pEntity);
	static std::string ReadDesignerName(uintptr_t designerNameptr);
	static Vector EntityPosition(uintptr_t gameScene);

	int EntityOwner() const noexcept;
	uintptr_t PEntity() const noexcept;
	uintptr_t Gamescene() const noexcept;

private:
	uintptr_t entityController;
};