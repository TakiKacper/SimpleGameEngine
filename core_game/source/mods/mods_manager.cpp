#include "mods_manager.h"

#include "include/nlohmann/json.hpp"
#include "source/filesystem/filesystem.h"

#include "source/common/crash.h"

#include "source/common/common.h"
#include "source/assets/assets_manager.h"
#include "source/entities/world.h"
#include "source/input/input_manager.h"
#include "source/audio/audio_manager.h"
#include "source/rendering/renderer.h"

#include "source/utilities/hash_string.h"

static std::string current_mod_name = "";

void mods::mods_manager::load_mod(std::string mod_folder_name)
{
	filesystem::set_mod_assets_directory(mod_folder_name);
	auto manifest_file = filesystem::load_file("mod/manifest.json");
	nlohmann::json manifest = nlohmann::json::parse(manifest_file);
	manifest_file.close();

	size_t index = mod_folder_name.find_last_of('/');
	current_mod_name = mod_folder_name.substr(index + 1, mod_folder_name.size());
	filesystem::ensure_mod_saves_folder_exist(current_mod_name);

	if (!(manifest.contains("start_scene") && manifest.at("start_scene").is_string()))
		error_handling::crash(error_handling::error_source::core, "[mods_manager::load_mod]",
			"Invalid mod manifest: missing start_scene / start_scene isn't string");

	std::string start_scene = manifest.at("start_scene");

	if (!(manifest.contains("pixels_per_unit") && manifest.at("pixels_per_unit").is_number()))
		error_handling::crash(error_handling::error_source::core, "[mods_manager::load_mod]",
			"Invalid mod manifest: missing pixels_per_unit / pixels_per_unit isn't number");

	common::pixels_per_world_unit = manifest.at("pixels_per_unit");

	if (!(manifest.contains("top_down") && manifest.at("top_down").is_boolean()))
		error_handling::crash(error_handling::error_source::core, "[mods_manager::load_mod]",
			"Invalid mod manifest: missing top_down / top_down isn't bool");

	common::top_down = manifest.at("top_down");

	if (!(manifest.contains("gravitational_acceleration") && manifest.at("gravitational_acceleration").is_number()))
		error_handling::crash(error_handling::error_source::core, "[mods_manager::load_mod]",
			"Invalid mod manifest: missing gravitational_acceleration / gravitational_acceleration isn't float");

	common::gravitational_acceleration = manifest.at("gravitational_acceleration");

	if (!(manifest.contains("audio_rolloff") && manifest.at("audio_rolloff").is_number()))
		error_handling::crash(error_handling::error_source::core, "[mods_manager::load_mod]",
			"Invalid mod manifest: missing audio_rolloff / audio_rolloff isn't number");

	common::audio_manager->set_audio_rolloff(manifest.at("audio_rolloff"));

	common::assets_manager->load_asset("mod/collision_config");
	common::assets_manager->lock_asset(utilities::hash_string("mod/collision_config"));

	common::assets_manager->load_asset("mod/input_config");
	common::assets_manager->lock_asset(utilities::hash_string("mod/input_config"));

	common::assets_manager->load_asset("mod/rendering_config");
	common::assets_manager->lock_asset(utilities::hash_string("mod/rendering_config"));
	common::renderer->load_config();

	auto input_config = assets::cast_asset<assets::input_config>
		(common::assets_manager->get_asset(utilities::hash_string("mod/input_config")));
	common::input_mananger->load_config(input_config.lock());

	common::world->create_scene(
		utilities::hash_string("start_scene"),
		glm::vec2(0, 0),
		assets::cast_asset<assets::scene>(common::assets_manager->safe_get_asset("mod" + start_scene))
	);
}

void mods::mods_manager::unload_mod()
{
	common::audio_manager.reset();
	common::audio_manager = std::make_unique<audio::audio_manager>();

	common::assets_manager->unlock_asset(utilities::hash_string("mod/input_config"));
	common::assets_manager->unlock_asset(utilities::hash_string("mod/collision_config"));
	common::assets_manager->unlock_asset(utilities::hash_string("mod/rendering_config"));

	current_mod_name = "";
}

std::string mods::mods_manager::get_current_mod_folder_name()
{
	return current_mod_name;
}