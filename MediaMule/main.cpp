#include "json.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <windows.h>
int main(int argc, char* argv[]) {
	const std::string program_path = std::filesystem::absolute(argv[0]).parent_path().string();
	const std::string biliup_exe_path = "\"" + program_path + "/biliup.exe\"";
	const std::string cookie_json_path = "\"" + program_path + "/cookie.json\"";
	const std::string config_json_path = program_path + "/config.json";
	std::ifstream config_json_file(config_json_path);
	const nlohmann::json config_json_data = nlohmann::json::parse(config_json_file);
	const std::string version = config_json_data["version"];
	const bool copyright = config_json_data["copyright"];
	const std::string title = config_json_data["title"];
	const std::string tmp_dir = config_json_data["tmp_dir"];
	const int time_mode = config_json_data["time_mode"];
	const int time_interval = config_json_data["time_interval"];
	const std::vector<std::string> timelist = config_json_data["timelist"];
	//std::string cmd = "ls";
	//int result = system("ls");
	return 0;
}
