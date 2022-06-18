#pragma once

#include <chrono>
#include <filesystem>
#include <string>

#include <nlohmann/json.hpp>

namespace std::chrono
{

inline void to_json(nlohmann::json &json, const milliseconds &duration)
{
	json = duration.count();
}

inline void from_json(const nlohmann::json &json, milliseconds &duration)
{
	duration = milliseconds {json.get<uint64_t>()};
}

} // namespace std::chrono

namespace tomato
{

class Config
{
public:
	~Config();

	struct Options
	{
		struct Sockets
		{
			std::chrono::milliseconds readTimeout, writeTimeout, acceptTimeout;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(Sockets, readTimeout, writeTimeout, acceptTimeout);
		} sockets;

		struct TLS
		{
			std::filesystem::path cert, certKey;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(TLS, cert, certKey);
		} tls;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Options, sockets, tls);
	} options;

	Config(const Config &) = delete;
	Config &operator=(const Config &) = delete;

	static Config &getInstance();

	void load(const std::filesystem::path &path);
	void save();

private:
	Config();

	std::filesystem::path filePath;
};

} // namespace tomato
