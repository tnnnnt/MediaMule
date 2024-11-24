#pragma once
#include "json.hpp"
#include "spdlog/sinks/rotating_file_sink.h"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
static std::string get_timestamp() {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
	return std::to_string(timestamp);
}
static int time_to_minutes(const std::string& time_str) {
	int hour, minute;
	char colon;
	std::istringstream(time_str) >> hour >> colon >> minute;
	return hour * 60 + minute;
}
static int get_current_minutes() {
	auto now = std::chrono::system_clock::now();
	time_t current_time = std::chrono::system_clock::to_time_t(now);
	tm local_time;
	localtime_s(&local_time, &current_time);
	return local_time.tm_hour * 60 + local_time.tm_min;
}
static int get_sleep_duration(const std::vector<std::string>& time_list) {
	int current_minutes = get_current_minutes();
	std::vector<int> times_in_minutes;
	for (const auto& time_str : time_list) {
		times_in_minutes.push_back(time_to_minutes(time_str));
	}
	for (int time_min : times_in_minutes) {
		if (current_minutes < time_min) {
			return time_min - current_minutes;
		}
	}
	return 24 * 60 - current_minutes + times_in_minutes.front();
}
static bool get_video_and_upload(const std::string& get_video_python_script_path, const std::string& video_path, const std::string& biliup_exe_path, const std::string& cookie_json_path, const std::string& tags, std::shared_ptr<spdlog::logger>&logger) {
	logger->info("getting video");
	std::string cmd = "python \"" + get_video_python_script_path + "\" \"" + video_path + "\"";
	if (system(cmd.c_str())) {
		return false;
	}
    logger->info("uploading video");
	cmd = biliup_exe_path + " --user-cookie \"" + cookie_json_path + "\" upload --copyright 1 --tag \"" + tags + "\" --title " + get_timestamp() + " \"" + video_path + "\"";
	if (system(cmd.c_str())) {
		return false;
	}
	return true;
}
static bool are_files_exist(const std::vector<std::string>& file_paths, std::vector<std::string>& miss_file_paths) {
	for (const auto& path : file_paths) {
		if (!std::filesystem::exists(path)) {
			miss_file_paths.push_back(path);
		}
	}
	return miss_file_paths.empty();
}
int main(int argc, char* argv[]) {
	const std::string version = "2024.11.24";
	const std::string program_path = std::filesystem::absolute(argv[0]).parent_path().string();
	const std::string log_file_path = program_path + "/logs/log.txt";
	const size_t max_size = static_cast<size_t>(1048576 * 5);
	const size_t max_files = 3;
	auto logger = spdlog::rotating_logger_mt("MediaMule", log_file_path, max_size, max_files);
	logger->info("MediaMule v{} start running", version);
	const std::string biliup_exe_path = program_path + "/biliup.exe";
	const std::string get_video_python_script_path = program_path + "/get_video.py";
	const std::string cookie_json_path = program_path + "/cookie1.json";
	const std::string config_json_path = program_path + "/config1.json";
	logger->info("start check file integrity");
	std::vector<std::string> miss_file_paths;
	if (!are_files_exist({ biliup_exe_path ,get_video_python_script_path ,cookie_json_path ,config_json_path }, miss_file_paths)) {
		for (const auto& path : miss_file_paths) {
            logger->error("file {} is missing", path);
		}
		return 1;
	}
	logger->info("end check file integrity");
	logger->info("start reading json data");
	std::ifstream config_json_file(config_json_path);
	const nlohmann::json config_json_data = nlohmann::json::parse(config_json_file);
	const bool copyright = config_json_data["copyright"];
	const std::string tags = config_json_data["tags"];
	const std::string video_path = config_json_data["video_path"];
	const int time_mode = config_json_data["time_mode"];
	const int time_interval = config_json_data["time_interval"];
	const std::vector<std::string> timelist = config_json_data["timelist"];
	logger->info("end reading json data");
	if (time_mode == 0) {
		while (true) {
			if (!are_files_exist({ biliup_exe_path ,get_video_python_script_path ,cookie_json_path }, miss_file_paths)) {
				for (const auto& path : miss_file_paths) {
					logger->error("file {} is missing", path);
				}
				return 1;
			}
			if (!get_video_and_upload(get_video_python_script_path, video_path, biliup_exe_path, cookie_json_path, tags, logger)) {
                logger->error("program failed, please submit issue to https://github.com/tnnnnt/MediaMule/issues");
				return 3;
			}
			std::this_thread::sleep_for(std::chrono::minutes(time_interval));
		}
	}else if (time_mode == 1) {
		while (true) {
			int sleep_duration = get_sleep_duration(timelist);
			std::this_thread::sleep_for(std::chrono::minutes(sleep_duration));
			if (!are_files_exist({ biliup_exe_path ,get_video_python_script_path ,cookie_json_path }, miss_file_paths)) {
				for (const auto& path : miss_file_paths) {
					logger->error("file {} is missing", path);
				}
				return 1;
			}
			if (!get_video_and_upload(get_video_python_script_path, video_path, biliup_exe_path, cookie_json_path, tags, logger)) {
				logger->error("program failed, please submit issue to https://github.com/tnnnnt/MediaMule/issues");
				return 3;
			}
		}
	}
	else {
        logger->error("invalid time mode");
		return 2;
	}
	return 0;
}
