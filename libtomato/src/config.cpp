#include <fstream>

#include "tomato/config.hpp"

namespace tomato
{

Config::Config() : filePath {"./config.json"}
{
	load(filePath);
}

Config::~Config()
{
	save();
}

Config &Config::getInstance()
{
	static auto instance = Config {};

	return instance;
}

void Config::load(const std::filesystem::path &path)
{
	auto stream = std::fstream {path, std::ios_base::in};

	if (stream.is_open())
		options = nlohmann::json::parse(stream).get<Options>();

	filePath = path;
}

void Config::save()
{
	auto stream = std::fstream {filePath, std::ios_base::out};

	if (stream.is_open())
		stream << nlohmann::json(options).dump(1, '\t');
}

} // namespace tomato
