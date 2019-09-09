#include "PxPhysicsAPI.h"

namespace Engine {
	class PxDefaultErrorCallback : public physx::PxErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
	};

}
