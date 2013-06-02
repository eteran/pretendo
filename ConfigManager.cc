
#include "ConfigManager.h"

ConfigManager::ConfigManager() 
	: filename_("pretendo.cfg") 
{
}


ConfigManager::~ConfigManager()
{
}


bool ConfigManager::Load() {

	std::cout << "Loading config from file..." << std::endl;
	std::ifstream file(filename_.c_str());

	if(!file) {
		// file does not exist, make a new one with some defaults
		std::cout << "Couldn't load file. Creating new one..." << std::endl;

		NewSection("App Settings");
		NewKey("App Settings", kv_pair("ShowOpenOnLoad", "false"));
		NewKey("App Settings", kv_pair("AutoRun", "false"));
		NewKey("App Settings", kv_pair("SleepOnLoseFocus", "false"));
		NewKey("App Settings", kv_pair("ROMDirectory", "./roms"));
		
		Save();

		return false;
	}

	std::string linebuffer;
	std::string section;
	unsigned int line = 0;

	while(std::getline(file, linebuffer)) {
		++line;
		if(linebuffer.empty()) {
			continue;
		}

		if(linebuffer.at(0) == '[') {
			linebuffer.erase(0, 1);

			const size_t end = linebuffer.find_last_of(']');

			if(end != std::string::npos) {
				linebuffer.erase(end, 1);
				NewSection(linebuffer);
				section = linebuffer;
			} else {
				std::cout << "ConfigFile: error on line " << line << std::endl;
				continue;
			}
		} else {
			const size_t eqPos = linebuffer.find_first_of("=");
			if(eqPos == std::string::npos) {
				std::cout << "Error loading config file, line " << line << std::endl;
				continue;
			}

			std::string key   = linebuffer;
			std::string value = linebuffer;

			key.erase(eqPos);
			value.erase(0, eqPos + 1);

			if(key.empty()) {
				std::cout << "config file: bad key on line " << line << std::endl;
				continue;
			}

			NewKey(section, kv_pair(key, value));
		}
	}

	return true;
}


bool ConfigManager::Save() {

	std::cout << "Saving Config..." << std::endl;
	std::ofstream file(filename_.c_str(), std::ios::trunc);

	if(!file) {
		std::cout << "Error:  ConfigManager::Save() couldn't open file for writing" << std::endl;
		//FIXME: throw exception or something equally creative
		return false;
	}

	for(config_iterator ci = map_.begin(); ci != map_.end(); ++ci) {

		file << "\n[" << ci->first << "]" << std::endl;

		for(kv_iterator ki = map_[ci->first].begin(); ki != map_[ci->first].end(); ++ki) {
			if(ki->first.empty() || ki->second.empty()) {
				continue;
			}

			file << ki->first << "=" << ki->second << std::endl;
		}
	}

	file.flush();
	return true;
}


bool ConfigManager::DeleteSection(const std::string &section) {

	std::cout << "DeleteSection -> " << section << std::endl;
	const config_iterator ci = map_.find(section);

	if(ci == map_.end()) {
		return false;
	}

	if(!map_[section].empty()) {
		map_[section].clear();
	}

	map_.erase(ci);
	return true;
}


bool ConfigManager::NewSection(const std::string &section) {
	std::cout << "NewSection: " << section << std::endl;

	if(section.empty()) {
		return false;
	}

	if(map_.find(section) != map_.end()) {
		std::cout << "Section: " << section << " already in list." << std::endl;
		return false;
	}

	map_[section] = kv_map();
	return true;
}


bool ConfigManager::NewKey(const std::string &section, kv_pair key) {
	if(section.empty()) {
		return false;
	}

	if(map_.find(section) == map_.end()) {
		return false;
	}

	for(kv_const_iterator it = map_[section].begin(); it != map_[section].end(); ++it) {
		if(it->first == key.first) {
			std::cout << "key " << key.first << " already exists." << std::endl;
			return false;
		}
	}

	std::cout << "NewKey -> adding: " << key.first << ", " << key.second << std::endl;
	map_[section].insert(key);
	return true;
}


bool ConfigManager::DeleteKey(const std::string &section, const std::string &keyName) {
	if(section.empty()) {
		return false;
	}

	for(kv_iterator it = map_[section].begin(); it != map_[section].end(); ++it){
		if(it->first == keyName) {
			std::cout << "DeleteKey -> " << keyName << std::endl;
			map_[section].erase(it);
			return true;
		}
	}

	return false;
}
