#pragma once
#include "component.h"
#include "source/physics/collision.h"

#include "include/glm/glm.hpp"

#include <memory>
#include <vector>

namespace entities
{
	class component;

	class entity
	{
	friend class scene;
	friend component;
	protected:
		std::vector<component*> components;
		std::shared_ptr<entity> self;
		glm::vec2 location{ 0.0f, 0.0f };
	public:
		entity();
		void kill();

		const glm::vec2& get_location();
		void teleport(glm::vec2 new_location);
		physics::collision_event sweep(glm::vec2 new_location);

		void attach_component(component* comp);
		component* get_component(uint32_t id);
		void kill_component(uint32_t id);
		//Make protected and shared a friend
		~entity();
	};
}