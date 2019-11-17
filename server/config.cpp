#include "config.hpp"

namespace toml {

template<>
struct from<client_position> {
	static client_position from_toml(const value& value) {
		const auto& str = value.as_string();

		if (str == "top")
			return client_position::TOP;

		if (str == "bottom")
			return client_position::BOTTOM;

		if (str == "left")
			return client_position::LEFT;

		if (str == "right")
			return client_position::RIGHT;

		throw type_error("Invalid client position", value.location());
	}
};

template<>
struct from<client_set> {
	static client_set from_toml(const value& value) {
		client_set clients;
		
		const auto& array = value.as_array();
		for (const auto& v : array) {
			auto name = find<std::string>(v, "name");
			auto position = find<client_position>(v, "position") ;

			for (const auto& [cname, cposition] : clients.positions_) {
				if (position == cposition)
					throw type_error("Multiple clients with the same position", v.location());
			}

			auto [it, inserted] = clients.positions_.insert({ name, position });
			if (!inserted)
				throw type_error("Multiple clients with the same name", v.location());
		}

		return clients;
	}
};

}

std::optional<client_position> client_set::get(const char* name) const {
	auto it = positions_.find(name);
	if (it == positions_.end())
		return std::nullopt;

	return it->second;
}

size_t client_set::length() const {
	return positions_.size();
}

config::config(const char* path) {
	auto data = toml::parse(path);

	port_ = toml::find<uint16_t>(data, "port");
	password_ = toml::find<std::string>(data, "password");
	clients_ = toml::find<client_set>(data, "clients");
}

uint16_t config::port() const {
	return port_;
}

const char* config::password() const {
	return password_.c_str();
}

const client_set& config::clients() const {
	return clients_;
}