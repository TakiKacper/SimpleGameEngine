#pragma once
#include "mod.h"

#include <string>
#include <vector>

namespace mods
{
	class mods_manager
	{
	public:
		void load_mod(std::string mod_name);
		void load_mod_selection_mod();
		void unload_mod();
		std::string get_current_mod_folder_name();
	};
}