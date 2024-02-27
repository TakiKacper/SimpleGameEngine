#pragma once
#include "source/entities/scene.h"
#include "behaviors_database.h"

#include <memory>

namespace behaviors
{
	struct frame
	{
		std::shared_ptr<database> target_object_database;
		std::unique_ptr<entities::scene>* scene_context;
	};
}