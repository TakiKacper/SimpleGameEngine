#pragma once
#include "source/common/common.h"
#include "source/common/crash.h"

#include "source/assets/asset.h"
#include "source/assets/assets_manager.h"

#include "source/assets/mesh_asset.h"
#include "source/assets/shader_asset.h"
#include "source/assets/texture_asset.h"

extern "C"
{
#include "include/lua_5_4_2/include/lua.h"
#include "include/lua_5_4_2/include/lauxlib.h"
#include "include/lua_5_4_2/include/lualib.h"
}

#include "source/entities/entity.h"
#include "source/entities/component.h"

#include "source/utilities/hash_string.h"

#include "source/rendering/render_config.h"

inline std::shared_ptr<::entities::entity> load_entity(lua_State* L, int arg_id, const std::string parent_function)
{
	if (!lua_isinteger(L, arg_id))
		error_handling::crash(error_handling::error_source::mod, parent_function,
			"Render config is missing shader");
	auto ptr = reinterpret_cast<std::weak_ptr<::entities::entity>*>(lua_tointeger(L, arg_id));
	if (ptr->expired())
		error_handling::crash(error_handling::error_source::mod, parent_function, "Trying to perform operations on dead entity.");
	return ptr->lock();
}

inline uint32_t load_id(lua_State* L, int arg_id, std::string parent_function, const std::string id_of_what)
{
	if (lua_isnumber(L, arg_id) || lua_isinteger(L, arg_id))
		return lua_tointeger(L, arg_id);
	else if (lua_isstring(L, arg_id))
		return ::utilities::hash_string(lua_tostring(L, arg_id));
	error_handling::crash(error_handling::error_source::mod, parent_function, id_of_what + " id should be integer or string");
}

inline std::string load_asset_path(lua_State* L, int arg_id, std::string parent_function)
{
	if (lua_isstring(L, arg_id))
		return lua_tostring(L, arg_id);
	error_handling::crash(error_handling::error_source::mod, parent_function, "Asset path should be string");
}

template<class comp_class>
inline comp_class* load_component(lua_State* L, const std::string parent_function, int entity_ptr_pos = 1, int component_id_pos = 2)
{
	auto e = load_entity(L, entity_ptr_pos, parent_function);
	uint32_t comp = load_id(L, component_id_pos, parent_function, "Component");
	return dynamic_cast<comp_class*>(e->get_component(comp));
}

inline rendering::render_config load_render_config(lua_State* L, int arg_id, const std::string parent_function)
{
	rendering::render_config rc{};

	lua_pushnil(L);
	if (lua_istable(L, arg_id))
	{
		while (lua_next(L, arg_id) != 0)
		{
			if (lua_isstring(L, -2))
			{
				std::string key = lua_tostring(L, -2);
				if (key == "shader")
				{
					rc.material = assets::cast_asset<assets::shader>(common::assets_manager->safe_get_asset(
						load_asset_path(L, -1, parent_function)
					)).lock();
				}
				else if (key == "mesh")
				{
					rc.mesh = assets::cast_asset<assets::mesh>(common::assets_manager->safe_get_asset(
						load_asset_path(L, -1, parent_function)
					)).lock();
				}
				else if (key == "textures")
				{
					if (lua_istable(L, arg_id + 2))
					{
						lua_pushnil(L);
						while (lua_next(L, arg_id + 2) != 0)
						{
							rc.textures.push_back(
								assets::cast_asset<assets::texture>(common::assets_manager->safe_get_asset(
									load_asset_path(L, -1, parent_function)
								)).lock()
							);
							lua_pop(L, 1);
						}
					}
				}
			}

			lua_pop(L, 1);
		}
	}

	if (rc.material == nullptr)
		error_handling::crash(error_handling::error_source::mod, parent_function,
			"Render config is missing shader");
	else if (rc.mesh == nullptr)
		error_handling::crash(error_handling::error_source::mod, parent_function,
			"Render config is missing mesh");

	return rc;
}