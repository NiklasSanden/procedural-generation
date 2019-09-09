#include "ErrorCallback.h"

#include "PxPhysicsAPI.h"

#include <iostream>

#include "engine/Debug.h"
using namespace Engine;

void PxDefaultErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
	std::cout << "---------> Error - PhysX error code: " << code << "\nMessage: " << message << "\nIn file: " << file << " and line: " << line << "\n-----------------------------" << std::endl;
}