
#include "Config.h"
#include <functional> 
#include <cctype>
#include <locale>

namespace {

void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

}

Config::Config() : filename_("pretendo.config") {
	Load();
}


Config::~Config() {
}


bool Config::Load() {

	std::cout << "Loading config from file..." << std::endl;
	std::ifstream file(filename_.c_str());

	if(!file) {
		// file does not exist, make a new one with some defaults
		std::cout << "Couldn't load file. Creating new one..." << std::endl;

		NewSection("App Settings");
		NewKey("App Settings", std::make_pair("ShowOpenOnLoad", "false"));
		NewKey("App Settings", std::make_pair("AutoRun", "false"));
		NewKey("App Settings", std::make_pair("SleepOnLoseFocus", "false"));
		NewKey("App Settings", std::make_pair("ROMDirectory", "./roms"));
		
		Save();

		return false;
	}

	std::string linebuffer;
	std::string current_section;
	unsigned int line_number = 0;

	while(std::getline(file, linebuffer)) {
	
		trim(linebuffer);
	
		++line_number;
		if(linebuffer.empty()) {
			continue;
		}

		if(linebuffer[0] == '[') {
			const size_t end = linebuffer.find_last_of(']');

			if(end != std::string::npos) {
				current_section = linebuffer.substr(1, end - 1);
				NewSection(current_section);
			} else {
				std::cout << "ConfigFile: error on line " << line_number << std::endl;
				continue;
			}
		} else if(linebuffer[0] == '#') {
			// skip comments
			continue;
		} else {
			const size_t eqPos = linebuffer.find_first_of("=");
			if(eqPos == std::string::npos) {
				std::cout << "Error loading config file, line " << line_number << std::endl;
				continue;
			}

			std::string key   = linebuffer;
			std::string value = linebuffer;

			key.erase(eqPos);
			value.erase(0, eqPos + 1);

			if(key.empty()) {
				std::cout << "config file: bad key on line " << line_number << std::endl;
				continue;
			}

			NewKey(current_section, std::pair<std::string, std::string>(key, value));
		}
	}

	return true;
}


bool Config::Save() {

	std::cout << "Saving Config..." << std::endl;
	std::ofstream file(filename_.c_str(), std::ios::trunc);

	if(!file) {
		std::cout << "Error:  Config::Save() couldn't open file for writing" << std::endl;
		//FIXME: throw exception or something equally creative
		return false;
	}

	for(std::map<std::string, section_type>::iterator ci = sections_.begin(); ci != sections_.end(); ++ci) {

		file << "\n[" << ci->first << "]" << std::endl;

		for(section_type::const_iterator ki = sections_[ci->first].begin(); ki != sections_[ci->first].end(); ++ki) {
			if(ki->first.empty() || ki->second.empty()) {
				continue;
			}

			file << ki->first << "=" << ki->second << std::endl;
		}
	}

	return true;
}


bool Config::DeleteSection(const std::string &section) {

	std::cout << "DeleteSection -> " << section << std::endl;
	std::map<std::string, section_type>::iterator it = sections_.find(section);

	if(it == sections_.end()) {
		return false;
	}
	
	it->second.clear();


	sections_.erase(it);
	return true;
}


bool Config::NewSection(const std::string &section) {
	std::cout << "NewSection: " << section << std::endl;

	if(section.empty()) {
		return false;
	}
	
	std::pair<std::map<std::string, section_type>::iterator, bool> it = sections_.insert(std::make_pair(section, section_type()));

	if(!it.second) {
		std::cout << "Section: " << section << " already in list." << std::endl;
		return false;
	}
	
	return true;
}


bool Config::NewKey(const std::string &section, const std::pair<std::string, std::string> &key) {
	if(section.empty()) {
		return false;
	}

	if(sections_.find(section) == sections_.end()) {
		return false;
	}

	for(section_type::const_iterator it = sections_[section].begin(); it != sections_[section].end(); ++it) {
		if(it->first == key.first) {
			std::cout << "key " << key.first << " already exists." << std::endl;
			return false;
		}
	}

	std::cout << "NewKey -> adding: " << key.first << ", " << key.second << std::endl;
	sections_[section].insert(key);
	return true;
}


bool Config::DeleteKey(const std::string &section, const std::string &keyName) {
	if(section.empty()) {
		return false;
	}

	for(section_type::iterator it = sections_[section].begin(); it != sections_[section].end(); ++it){
		if(it->first == keyName) {
			std::cout << "DeleteKey -> " << keyName << std::endl;
			sections_[section].erase(it);
			return true;
		}
	}

	return false;
}
