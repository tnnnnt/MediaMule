#include "json.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <chrono>
static std::string get_timestamp() {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
	return std::to_string(timestamp);
}
int main(int argc, char* argv[]) {
	const std::string version = "2024.11.24";
	const std::string program_path = std::filesystem::absolute(argv[0]).parent_path().string();
	const std::string biliup_exe_path = program_path + "/biliup.exe";
	const std::string get_video_python_script_path = "\"" + program_path + "/get_video.py\"";
	const std::string cookie_json_path = "\"" + program_path + "/cookie.json\"";
	const std::string config_json_path = program_path + "/config.json";
	std::ifstream config_json_file(config_json_path);
	const nlohmann::json config_json_data = nlohmann::json::parse(config_json_file);
	const bool copyright = config_json_data["copyright"];
	const std::string tags = config_json_data["tags"];
	const std::string video_path = config_json_data["video_path"];
	const int time_mode = config_json_data["time_mode"];
	const int time_interval = config_json_data["time_interval"];
	const std::vector<std::string> timelist = config_json_data["timelist"];
	std::string cmd = "python " + get_video_python_script_path + " \"" + video_path + "\"";
	system(cmd.c_str());
	cmd = biliup_exe_path + " --user-cookie " + cookie_json_path + " upload --copyright 1 --tag \"" + tags + "\" --title " + get_timestamp() + " \"" + video_path + "\"";
	system(cmd.c_str());
	return 0;
}
