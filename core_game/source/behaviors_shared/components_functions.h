#include "source/entities/entity.h"
#include "source/utilities/hash_string.h"

#include "source/assets/assets_manager.h"
#include "source/rendering/renderer.h"

#include "source/components/behavior.h"

#include "source/assets/asset.h"
#include "source/assets/behavior_asset.h"

namespace behaviors
{
	namespace lua_functions
	{
		namespace components
		{
			/*
				Behavior
			*/

			int _c_b_set_behavior(lua_State* L)
			{
				auto ptr = load_component<::entities::components::behavior>(L);
				auto asset = load_id(L, 3);
				if (ptr != nullptr)
					ptr->behavior_asset = ::assets::cast_asset<::assets::behavior>(::common::assets_manager->get_asset(asset));

				return 0;
			}

			/*
				Camera
			*/

			int _c_c_set_ortho_width(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L);
				float width = lua_tonumber(L, 3);
				if (camera != nullptr)
					camera->ortho_width = width;
				return 0;
			}

			int _c_c_get_ortho_width(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L);
				if (camera != nullptr)
					lua_pushnumber(L, camera->ortho_width);
				else
					lua_pushnumber(L, 0.0f);
				return 1;
			}

			int _c_c_set_active(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L);
				if (camera != nullptr)
					common::renderer->set_active_camera(camera);
				return 0;
			}

			void register_functions(lua_State* L)
			{
				lua_register(L, "_c_b_set_behavior", _c_b_set_behavior);

				lua_register(L, "_c_c_set_ortho_width", _c_c_set_ortho_width);
				lua_register(L, "_c_c_get_ortho_width", _c_c_get_ortho_width);
				lua_register(L, "_c_c_set_active", _c_c_set_active);
			}
		}
	}
}