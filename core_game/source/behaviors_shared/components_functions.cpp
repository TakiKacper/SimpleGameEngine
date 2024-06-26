#include "components_functions.h"

#include "utilities.h"

#include "source/entities/entity.h"
#include "source/utilities/hash_string.h"

#include "source/assets/assets_manager.h"
#include "source/rendering/renderer.h"
#include "source/audio/audio_manager.h"

#include "source/components/behavior.h"

#include "source/assets/asset.h"
#include "source/assets/behavior_asset.h"
#include "source/assets/collision_config_asset.h"

#include "source/components/camera.h"
#include "source/components/behavior.h"
#include "source/components/collider.h"
#include "source/components/static_mesh.h"
#include "source/components/sprite.h"
#include "source/components/flipbook.h"
#include "source/components/dynamics.h"
#include "source/components/tilemap.h"
#include "source/components/listener.h"
#include "source/components/sound_emitter.h"

namespace behaviors
{
	namespace lua_shared
	{
		namespace components
		{
			/*
				Any Mesh
			*/

			int _c_m_get_scale(lua_State* L)
			{
				auto mesh = load_component<::entities::components::mesh>(L, "[_c_m_get_scale]");
				lua_pushnumber(L, mesh->get_scale().x);
				lua_pushnumber(L, mesh->get_scale().y);
				return 2;
			}

			int _c_m_set_scale(lua_State* L)
			{
				auto mesh = load_component<::entities::components::mesh>(L, "[_c_m_set_scale]");
				float x = static_cast<float>(lua_tonumber(L, 3));
				float y = static_cast<float>(lua_tonumber(L, 4));
				mesh->set_scale({ x, y });
				return 0;
			}

			int _c_m_get_offset(lua_State* L)
			{
				auto mesh = load_component<::entities::components::mesh>(L, "[_c_m_get_offset]");
				lua_pushnumber(L, mesh->get_offset().x);
				lua_pushnumber(L, mesh->get_offset().y);
				return 2;
			}

			int _c_m_set_offset(lua_State* L)
			{
				auto mesh = load_component<::entities::components::mesh>(L, "[_c_m_set_offset]");
				float x = static_cast<float>(lua_tonumber(L, 3));
				float y = static_cast<float>(lua_tonumber(L, 4));
				mesh->set_offset({ x, y });
				return 0;
			}

			int _c_m_get_visible(lua_State* L)
			{
				auto mesh = load_component<::entities::components::mesh>(L, "[_c_m_get_visible]");
				lua_pushboolean(L, mesh->get_visible());
				return 1;
			}

			int _c_m_set_visible(lua_State* L)
			{
				auto mesh = load_component<::entities::components::mesh>(L, "[_c_m_set_visible]");
				mesh->set_visible(lua_toboolean(L, 3));
				return 0;
			}

			/*
				Static Mesh
			*/

			int _c_sm_get_render_config(lua_State* L)
			{
				auto mesh = load_component<::entities::components::static_mesh>(L, "[_c_sm_get_render_config]");
				auto& rc = mesh->get_render_config();

				lua_createtable(L, 0, 0);

				lua_pushstring(L, "shader");
				lua_pushstring(L, rc.material->package_name.c_str());
				lua_settable(L, -3);

				lua_pushstring(L, "mesh");
				lua_pushstring(L, rc.mesh->package_name.c_str());
				lua_settable(L, -3);

				lua_pushstring(L, "textures");
				lua_createtable(L, 0, 0);

				for (int i = 1; i < rc.textures.size() + 1; i++)
				{
					lua_pushinteger(L, 1);
					lua_pushstring(L, rc.textures.at(i - 1)->package_name.c_str());
					lua_settable(L, -3);
				}

				lua_settable(L, -3);

				return 1;
			}

			int _c_sm_set_render_config(lua_State* L)
			{
				auto mesh = load_component<::entities::components::static_mesh>(L, "[_c_sm_set_render_config]");
				rendering::render_config rc = load_render_config(L, 3, "[_c_sm_set_render_config]");

				mesh->set_render_config(rc);

				return 0;
			}

			/*
				Sprite
			*/

			int _c_s_get_sprite(lua_State* L)
			{
				auto sprite = load_component<::entities::components::sprite>(L, "[_c_s_set_sprite]");

				lua_pushinteger(L, sprite->get_sprite_id());

				return 0;
			}

			int _c_s_set_sprite(lua_State* L)
			{
				auto sprite = load_component<::entities::components::sprite>(L, "[_c_s_set_sprite]");
				int sprite_id = static_cast<int>(lua_tointeger(L, 3));

				sprite->set_sprite_id(sprite_id);

				return 0;
			}

			int _c_s_set_shader(lua_State* L)
			{
				auto sprite = load_component<::entities::components::sprite>(L, "[_c_s_set_sprite]");
				auto shader_path = load_asset_path(L, 3, "[_c_s_set_sprite]");
				auto shader = ::assets::cast_asset<::assets::shader>(::common::assets_manager->safe_get_asset(shader_path)).lock();

				sprite->set_shader(shader);

				return 0;
			}

			/*
				Flipbook
			*/

			int _c_f_get_animation(lua_State* L)
			{
				auto flipbook = load_component<::entities::components::flipbook>(L, "[_c_f_get_animation]");

				lua_pushinteger(L, flipbook->get_animation());

				return 0;
			}

			int _c_f_set_animation(lua_State* L)
			{
				auto flipbook = load_component<::entities::components::flipbook>(L, "[_c_f_set_animation]");
				uint32_t animation = load_id(L, 3, "[_c_f_set_animation]", "Animation");

				flipbook->set_animation(animation);

				return 0;
			}

			int _c_f_get_looping(lua_State* L)
			{
				auto flipbook = load_component<::entities::components::flipbook>(L, "[_c_f_get_animation]");

				lua_pushboolean(L, flipbook->looping);

				return 0;
			}

			int _c_f_set_looping(lua_State* L)
			{
				auto flipbook = load_component<::entities::components::flipbook>(L, "[_c_f_set_animation]");
				bool looping = lua_toboolean(L, 3);

				flipbook->looping = looping;

				return 0;
			}

			/*
				Camera
			*/

			int _c_c_get_ortho_width(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L, "[_c_c_get_ortho_width]");
				if (camera != nullptr)
					lua_pushnumber(L, camera->ortho_width);
				else
					lua_pushnumber(L, 0.0f);
				return 1;
			}

			int _c_c_set_ortho_width(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L, "[_c_c_set_ortho_width]");
				float width = static_cast<float>(lua_tonumber(L, 3));
				if (camera != nullptr)
					camera->ortho_width = width;
				return 0;
			}

			int _c_c_get_lowest_layer(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L, "[_c_c_get_rendered_layers]");
				lua_pushinteger(L, camera->lowest_layer);
				return 1;
			}

			int _c_c_set_lowest_layer(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L, "[_c_c_set_rendered_layers]");
				camera->lowest_layer = static_cast<int>(lua_tointeger(L, 3));
				return 0;
			}

			int _c_c_get_highest_layer(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L, "[_c_c_get_rendered_layers]");
				lua_pushinteger(L, camera->highest_layer);
				return 1;
			}

			int _c_c_set_highest_layer(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L, "[_c_c_set_rendered_layers]");
				camera->highest_layer = static_cast<int>(lua_tointeger(L, 3));
				return 0;
			}

			int _c_c_get_active(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L, "[_c_c_get_active]");
				lua_pushboolean(L, common::renderer->get_active_camera() == camera);
				return 1;
			}

			int _c_c_set_active(lua_State* L)
			{
				auto camera = load_component<::entities::components::camera>(L, "[_c_c_set_active]");
				common::renderer->set_active_camera(camera);
				return 0;
			}

			/*
				Behavior
			*/

			int _c_b_get_behavior(lua_State* L)
			{
				auto ptr = load_component<::entities::components::behavior>(L, "[_c_b_get_behavior]");
				lua_pushstring(L, ptr->behavior_asset->package_name.c_str());
				return 1;
			}

			int _c_b_set_behavior(lua_State* L)
			{
				auto ptr = load_component<::entities::components::behavior>(L, "[_c_b_set_behavior]");
				auto asset = load_asset_path(L, 3, "[_c_b_set_behavior]");
				if (ptr != nullptr)
					ptr->behavior_asset = ::assets::cast_asset<::assets::behavior>(::common::assets_manager->safe_get_asset(asset)).lock();
				return 0;
			}
			
			int _c_b_call(lua_State* L)
			{
				auto ptr = load_component<::entities::components::behavior>(L, "[_c_b_call]");
				const char* name = lua_tostring(L, 3);

				lua_remove(L, 1);	//Remove entity
				lua_remove(L, 1);	//Remove component id
				lua_remove(L, 1);	//Remove event name

				std::string event_name = "event_";
				event_name += name;
				int args = luaL_ref(L, LUA_REGISTRYINDEX);
				ptr->call_custom_function(event_name, args);
				luaL_unref(L, LUA_REGISTRYINDEX, args);
				return 1;
			}

			/*
				Collider
			*/

			int _c_cl_get_collision_preset(lua_State* L)
			{
				auto ptr = load_component<::entities::components::collider>(L, "[_c_cl_get_collision_preset]");
				auto config = ::assets::cast_asset<::assets::collision_config>(::common::assets_manager->get_asset(
					utilities::hash_string("mod/collision_config"))).lock();

				auto it = std::find_if(config->collision_presets.begin(), config->collision_presets.end(),
					[&ptr](auto&& p) { return p.second == ptr->preset; });

				if (it == config->collision_presets.end())
					lua_pushinteger(L, ptr->preset);
				else
					lua_pushstring(L, config->collision_presets_names.at(it->first).c_str());

				return 1;
			}

			int _c_cl_set_collision_preset(lua_State* L)
			{
				auto ptr = load_component<::entities::components::collider>(L, "[_c_cl_set_collision_preset]");
				auto preset = lua_tostring(L, 3);

				auto config = ::assets::cast_asset<::assets::collision_config>(
					::common::assets_manager->get_asset(utilities::hash_string("mod/collision_config")));

				if (ptr != nullptr)
					ptr->preset = config.lock()->get_preset(utilities::hash_string(preset));
				return 0;
			}

			int _c_cl_get_offset(lua_State* L)
			{
				auto cl = load_component<::entities::components::collider>(L, "[_c_cl_get_offset]");
				lua_pushnumber(L, cl->entity_offset.x);
				lua_pushnumber(L, cl->entity_offset.y);
				return 2;
			}

			int _c_cl_set_offset(lua_State* L)
			{
				auto cl = load_component<::entities::components::collider>(L, "[_c_cl_set_offset]");
				cl->entity_offset.x = static_cast<float>(lua_tonumber(L, 3));
				cl->entity_offset.y = static_cast<float>(lua_tonumber(L, 4));
				return 0;
			}

			int _c_cl_get_extend(lua_State* L)
			{
				auto cl = load_component<::entities::components::collider>(L, "[_c_cl_get_extend]");
				lua_pushnumber(L, cl->extend.x);
				lua_pushnumber(L, cl->extend.y);
				return 2;
			}

			int _c_cl_set_extend(lua_State* L)
			{
				auto cl = load_component<::entities::components::collider>(L, "[_c_cl_set_extend]");
				cl->extend.x = static_cast<float>(lua_tonumber(L, 3));
				cl->extend.y = static_cast<float>(lua_tonumber(L, 4));
				return 0;
			}

			int _c_cl_get_layer_offset(lua_State* L)
			{
				auto cl = load_component<::entities::components::collider>(L, "[_c_cl_get_layer_offset]");
				lua_pushinteger(L, cl->layer_offset);
				return 1;
			}

			int _c_cl_set_layer_offset(lua_State* L)
			{
				auto cl = load_component<::entities::components::collider>(L, "[_c_cl_set_layer_offset]");
				cl->layer_offset = static_cast<int>(lua_tointeger(L, 3));
				return 0;
			}

			/*
				Dynamics
			*/

			int _c_d_add_force(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_add_force]");
				float x = static_cast<float>(lua_tonumber(L, 3));
				float y = static_cast<float>(lua_tonumber(L, 4));
				d->add_force({ x, y });
				return 0;
			}

			int _c_d_add_movement_input(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_add_movement_force]");

				float dir_x = static_cast<float>(lua_tonumber(L, 3));
				float dir_y = static_cast<float>(lua_tonumber(L, 4));
				float speed = static_cast<float>(lua_tonumber(L, 5));

				glm::vec2 vec = { dir_x, dir_y };

				if (glm::length(vec) == 0.0f)
					return 0;

				vec = glm::normalize(vec) * speed;
				d->add_force(vec);

				return 0;
			}

			int _c_d_get_drag(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_get_drag]");
				lua_pushnumber(L, d->drag);
				return 1;
			}

			int _c_d_set_drag(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_drag]");
				auto drag = static_cast<float>(lua_tonumber(L, 3));
				d->drag = drag;
				return 0;
			}

			int _c_d_get_mass(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_get_mass]");
				lua_pushnumber(L, d->mass);
				return 1;
			}

			int _c_d_set_mass(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_mass]");
				auto mass = static_cast<float>(lua_tonumber(L, 3));
				d->mass = mass;
				return 0;
			}

			int _c_d_get_use_max_vel(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_get_use_max_vel]");
				lua_pushboolean(L, d->use_maximum_velocity);
				return 1;
			}

			int _c_d_set_use_max_vel(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_use_max_vel]");
				auto use = lua_toboolean(L, 3);
				d->use_maximum_velocity = use;
				return 0;
			}

			int _c_d_get_max_vel(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_get_max_vel]");
				lua_pushnumber(L, d->maximum_velocity);
				return 1;
			}

			int _c_d_get_vel(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_max_vel]");
				auto vel = d->get_velocity();
				lua_pushnumber(L, vel.x);
				lua_pushnumber(L, vel.y);
				return 2;
			}

			int _c_d_set_vel(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_max_vel]");
				auto x = static_cast<float>(lua_tonumber(L, 3));
				auto y = static_cast<float>(lua_tonumber(L, 4));
				
				d->set_velocity({ x, y });

				return 0;
			}

			int _c_d_set_max_vel(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_max_vel]");
				auto vel = static_cast<float>(lua_tonumber(L, 3));
				d->maximum_velocity = vel;
				return 0;
			}

			int _c_d_get_gravity_enabled(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_max_vel]");
				lua_pushboolean(L, d->gravity_enabled);
				return 1;
			}

			int _c_d_set_gravity_enabled(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_max_vel]");
				bool enabled = lua_toboolean(L, 3);
				d->gravity_enabled = enabled;
				return 0;
			}

			int _c_d_get_grounded(lua_State* L)
			{
				auto d = load_component<::entities::components::dynamics>(L, "[_c_d_set_max_vel]");
				lua_pushboolean(L, d->get_grounded());
				return 1;
			}

			/*
				Tilemap
			*/
			
			int _c_t_get_layers_stride(lua_State* L)
			{
				auto t = load_component<::entities::components::tilemap>(L, "[_c_t_set_layers_stride]");
				lua_pushnumber(L, t->get_layers_stride());
				return 1;
			}

			int _c_t_set_layers_stride(lua_State* L)
			{
				auto t = load_component<::entities::components::tilemap>(L, "[_c_t_set_layers_stride]");
				int new_stide = static_cast<int>(lua_tointeger(L, 3));
				if (new_stide <= 0)
					error_handling::crash(error_handling::error_source::core, "[_c_t_set_layers_stride]",
						"Layers stride must be greater than 0");
				t->set_layers_stride(new_stide);
				return 0;
			}

			/*
				Listener
			*/

			int _c_l_get_active(lua_State* L)
			{
				auto listener = load_component<::entities::components::listener>(L, "[_c_l_get_active]");
				lua_pushboolean(L, common::audio_manager->is_active_listner(listener));
				return 1;
			}

			int _c_l_set_active(lua_State* L)
			{
				auto listener = load_component<::entities::components::listener>(L, "[_c_l_set_active]");
				common::audio_manager->set_active_listener(listener);
				return 0;
			}

			/*
				Emitter
			*/
			int _c_se_emit_sound(lua_State* L)
			{
				auto emitter = load_component<::entities::components::sound_emitter>(L, "[_c_se_emit_sound]");
				auto sound_path = load_asset_path(L, 3, "[_a_play_sound_at_channel]");
				float volume_precent = static_cast<float>(lua_tonumber(L, 4));

				auto sound = assets::cast_asset<assets::sound>(common::assets_manager->safe_get_asset(sound_path));
				emitter->emit_sound(sound, volume_precent);
				return 0;
			}

			/*
				Register
			*/

			void register_shared(lua_State* L)
			{
				lua_register(L, "_c_m_get_visible", _c_m_get_visible);
				lua_register(L, "_c_m_set_visible", _c_m_set_visible);
				lua_register(L, "_c_m_get_scale", _c_m_get_scale);
				lua_register(L, "_c_m_set_scale", _c_m_set_scale);
				lua_register(L, "_c_m_set_offset", _c_m_set_offset);
				lua_register(L, "_c_m_get_offset", _c_m_get_offset);

				lua_register(L, "_c_sm_get_render_config", _c_sm_get_render_config);
				lua_register(L, "_c_sm_set_render_config", _c_sm_set_render_config);

				lua_register(L, "_c_s_get_sprite", _c_s_get_sprite);
				lua_register(L, "_c_s_set_sprite", _c_s_set_sprite);
				lua_register(L, "_c_s_set_shader", _c_s_set_shader);
				
				lua_register(L, "_c_f_get_animation", _c_f_get_animation);
				lua_register(L, "_c_f_set_animation", _c_f_set_animation);
				lua_register(L, "_c_f_get_looping", _c_f_get_looping);
				lua_register(L, "_c_f_set_looping", _c_f_set_looping);

				lua_register(L, "_c_c_get_ortho_width", _c_c_get_ortho_width);
				lua_register(L, "_c_c_set_ortho_width", _c_c_set_ortho_width);
				lua_register(L, "_c_c_get_lowest_layer", _c_c_get_lowest_layer);
				lua_register(L, "_c_c_set_lowest_layer", _c_c_set_lowest_layer);
				lua_register(L, "_c_c_get_highest_layer", _c_c_get_highest_layer);
				lua_register(L, "_c_c_set_highest_layer", _c_c_set_highest_layer);
				lua_register(L, "_c_c_get_active", _c_c_get_active);
				lua_register(L, "_c_c_set_active", _c_c_set_active);
				
				lua_register(L, "_c_b_get_behavior", _c_b_get_behavior);
				lua_register(L, "_c_b_set_behavior", _c_b_set_behavior);
				lua_register(L, "_c_b_call", _c_b_call);

				lua_register(L, "_c_cl_get_collision_preset", _c_cl_get_collision_preset);
				lua_register(L, "_c_cl_set_collision_preset", _c_cl_set_collision_preset);
				lua_register(L, "_c_cl_get_offset", _c_cl_get_offset);
				lua_register(L, "_c_cl_set_offset", _c_cl_set_offset);
				lua_register(L, "_c_cl_get_extend", _c_cl_get_extend);
				lua_register(L, "_c_cl_set_extend", _c_cl_set_extend);
				lua_register(L, "_c_cl_get_layer_offset", _c_cl_get_layer_offset);
				lua_register(L, "_c_cl_set_layer_offset", _c_cl_set_layer_offset);

				lua_register(L, "_c_d_add_force", _c_d_add_force);
				lua_register(L, "_c_d_add_movement_input", _c_d_add_movement_input);
				lua_register(L, "_c_d_get_drag", _c_d_get_drag);
				lua_register(L, "_c_d_set_drag", _c_d_set_drag);
				lua_register(L, "_c_d_get_mass", _c_d_get_mass);
				lua_register(L, "_c_d_set_mass", _c_d_set_mass);
				lua_register(L, "_c_d_get_use_max_vel", _c_d_get_use_max_vel);
				lua_register(L, "_c_d_set_use_max_vel", _c_d_set_use_max_vel);
				lua_register(L, "_c_d_get_max_vel", _c_d_get_max_vel);
				lua_register(L, "_c_d_set_max_vel", _c_d_set_max_vel);
				lua_register(L, "_c_d_get_vel", _c_d_get_vel);
				lua_register(L, "_c_d_set_vel", _c_d_set_vel);
				lua_register(L, "_c_d_get_gravity_enabled", _c_d_get_gravity_enabled);
				lua_register(L, "_c_d_set_gravity_enabled", _c_d_set_gravity_enabled);
				lua_register(L, "_c_d_get_grounded", _c_d_get_grounded);
				
				lua_register(L, "_c_t_get_layers_stride", _c_t_get_layers_stride);
				lua_register(L, "_c_t_set_layers_stride", _c_t_set_layers_stride);

				lua_register(L, "_c_l_get_active", _c_l_get_active);
				lua_register(L, "_c_l_set_active", _c_l_set_active);

				lua_register(L, "_c_se_emit_sound", _c_se_emit_sound);
			}
		}
	}
}