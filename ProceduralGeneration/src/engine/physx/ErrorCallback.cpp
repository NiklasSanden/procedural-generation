#include "ErrorCallback.h"

#include "PxPhysicsAPI.h"

#include <iostream>

#include "engine/Debug.h"
using namespace Engine;

void PxDefaultErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
	LogManager::LogError("PhysX error code: " + std::to_string(code) + "\nMessage: " + message + "\nIn file: " + file + " and line: " + std::to_string(line) + "\n-----------------------------");
}