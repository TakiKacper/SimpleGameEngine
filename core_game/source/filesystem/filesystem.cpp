#include "filesystem.h"
#include "source/common/crash.h"
#include "include/stb/stb_image.h"
#include "include/dirent.h"

static std::string mod_assets_path;
static std::string core_assets_path;
static std::string mods_path;

void filesystem::set_mod_assets_directory(std::string path)
{
	mod_assets_path = path;
}

void filesystem::set_core_assets_directory(std::string path)
{
	core_assets_path = path;
}

void filesystem::set_mods_directory(std::string path)
{
	mods_path = path;
}

std::string filesystem::get_package(std::string path)
{
	int split_index = path.find('/');
	std::string package = path.substr(0, split_index);
	return package;
}

std::string filesystem::get_global_path(std::string path)
{
	int split_index = path.find('/');
	std::string package = path.substr(0, split_index);
	if (package == "mod")
		return  mod_assets_path + path.substr(split_index, path.size());
	else if (package == "core")
		return core_assets_path + path.substr(split_index, path.size());
}

std::fstream filesystem::load_file(std::string path)
{
	int split_index = path.find('/');
	std::string global_path = filesystem::get_global_path(path);

	std::fstream file;
	file.open(global_path);

	if (file.fail())
		error_handling::crash(error_handling::error_source::core, "[filesystem::load_file]", 
			"No such file: \n" + path + "\n" + global_path);

	return file;
}

std::vector<std::string> filesystem::get_all_subfolders(std::string folder)
{
	std::vector<std::string> result;

	DIR* dir = opendir(folder.c_str());

	struct dirent* entry = readdir(dir);

	while (entry != NULL)
	{
		if (entry->d_type == DT_DIR)
			result.push_back(entry->d_name);

		entry = readdir(dir);
	}

	closedir(dir);

	return result;
}

filesystem::image_file::~image_file()
{
	stbi_image_free(image_source_pointer);
}

std::unique_ptr<filesystem::image_file> filesystem::load_image(std::string path)
{
	std::unique_ptr<image_file> file = std::make_unique<image_file>();
	stbi_set_flip_vertically_on_load(true);

	file->image_source_pointer = stbi_load(filesystem::get_global_path(path).c_str(),
		&file->width,
		&file->height, 
		&file->color_channels, 
	0);

	if (file->image_source_pointer == nullptr)
		error_handling::crash(error_handling::error_source::core, "[filesystem::load_file]",
			"No such image: \n" + path + "\n" + filesystem::get_global_path(path));

	return file;
}