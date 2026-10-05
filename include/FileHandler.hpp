#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

inline bool write_file(const fs::path &path, const std::string_view &content) {
	std::ofstream out(path);
	if (!out.is_open()) return false;
	out << content << std::endl;
	return true;
}