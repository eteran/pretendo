
#include "Config.h"

#include <functional> 
#include <cctype>
#include <locale>
#include <vector>
#include <sys/stat.h>

#ifdef __HAIKU__
	#include <FindDirectory.h>
	#include <fs_info.h>
	#include <Path.h>
	#include <Directory.h>
#endif


namespace {
	#ifdef __HAIKU__
		#define CONFIG_PATH "/boot/home/config/settings/pretendo"
	#else
		#define CONFIG_PATH "~/"
	#endif // __HAIKU__

//------------------------------------------------------------------------------
// Name: ltrim
//------------------------------------------------------------------------------
void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

//------------------------------------------------------------------------------
// Name: ltrim
//------------------------------------------------------------------------------
void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

//------------------------------------------------------------------------------
// Name: trim
//------------------------------------------------------------------------------
void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

//------------------------------------------------------------------------------
// Name: explode
//------------------------------------------------------------------------------
std::vector<std::string> explode(const std::string &delimeter, const std::string &string, int limit) {
	std::vector<std::string> r;

	if(!string.empty()) {
		if(limit >= 0) {
			if(limit == 0) {
				limit = 1;
			}

			std::size_t first = 0;
			std::size_t last  = string.find(delimeter);

			while(last != std::string::npos) {

				if(--limit == 0) {
					break;
				}

				r.push_back(string.substr(first, last - first));
				first = last + delimeter.size();
				last  = string.find(delimeter, last + delimeter.size());
			}

			r.push_back(string.substr(first));
		} else {
			std::size_t first = 0;
			std::size_t last  = string.find(delimeter);

			while(last != std::string::npos) {
				r.push_back(string.substr(first, last - first));
				first = last + delimeter.size();
				last  = string.find(delimeter, last + delimeter.size());
			}

			r.push_back(string.substr(first));
			
			while(limit < 0) {
				r.pop_back();
				++limit;
			}
		}
	}
	
	return r;
}

//------------------------------------------------------------------------------
// Name: explode
//------------------------------------------------------------------------------
inline std::vector<std::string> explode(const std::string &delimeter, const std::string &string) {
	return explode(delimeter, string, std::numeric_limits<int>::max());
}

Config::Config() 
{
#ifdef __HAIKU__
		BPath path;
		BDirectory *dir;
		
		find_directory(B_USER_SETTINGS_DIRECTORY, &path, false);
		filename_ = path.Path();
		dir = new BDirectory(filename_.c_str());
		dir->CreateDirectory("Pretendo", NULL);
		filename_ += "/Pretendo/pretendo.config";
#else
	filename_ = ("~/.pretendo");
	struct stat st;
	
	if (stat(filename_.c_str(), &st) != 0 && ! (S_ISDIR(st.st_mode))) {
		mkdir(filename_.c_str(), 0777);
		filename_ += "/pretendo.conf";
	}	
#endif //__HAIKU__
	
	Load();
}


Config::~Config() {
	Save();
}


bool 
Config::Load() {

	std::cout << "Loading config from file..." << std::endl;
	std::ifstream file(filename_.c_str());
		
	if(! file) {
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
		
			// TODO: handle if there is junk after the closing ']' character
			const size_t end = linebuffer.find_last_of(']');

			if(end != std::string::npos) {
				current_section = linebuffer.substr(1, end - 1);
				NewSection(current_section);
			} else {
				std::cerr << "[Config::Load] Error on line " << line_number << std::endl;
				continue;
			}
		} else if(linebuffer[0] == '#') {
			// skip comments
			continue;
		} else {			
			if(current_section.empty()) {
				std::cerr << "Error: every configuration option must be in a section" << std::endl;
				continue;
			}
			
			const std::vector<std::string> values = explode("=", linebuffer);
			
			if(values.size() != 2) {
				std::cerr << "Error: Every key must have exactly one value" << line_number << std::endl;
				continue;
			}

			std::string key   = values[0];
			std::string value = values[1];
			
			trim(key);
			trim(value);

			if(key.empty()) {
				std::cerr << "Error: bad key on line " << line_number << std::endl;
				continue;
			}

			NewKey(current_section, std::make_pair(key, value));
		}
	}
	
	return true;
}


bool Config::Save() {

	std::cout << "Saving Config..." << std::endl;
	std::ofstream file(filename_.c_str(), std::ios::trunc);

	if(!file) {
		std::cerr << "[Config::Save] Error: couldn't open file for writing" << std::endl;
		// TODO: throw exception or something equally creative
		return false;
	}

	for(auto ci = sections_.begin(); ci != sections_.end(); ++ci) {

		file << "\n[" << ci->first << "]" << std::endl;

		for(auto ki = sections_[ci->first].begin(); ki != sections_[ci->first].end(); ++ki) {
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
	auto it = sections_.find(section);

	if(it == sections_.end()) {
		return false;
	}
	
	sections_.erase(it);
	return true;
}


bool Config::NewSection(const std::string &section) {
	std::cout << "NewSection: " << section << std::endl;

	if(section.empty()) {
		return false;
	}
	
	auto it = sections_.insert(std::make_pair(section, section_type()));

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

	std::cout << "NewKey -> adding: " << key.first << ", " << key.second << std::endl;
	
	auto it = sections_[section].insert(key);
	if(!it.second) {
		std::cout << "key " << key.first << " already exists." << std::endl;
	}
	return true;
}


bool Config::DeleteKey(const std::string &section, const std::string &keyName) {
	if(section.empty()) {
		return false;
	}

	for(auto it = sections_[section].begin(); it != sections_[section].end(); ++it){
		if(it->first == keyName) {
			std::cout << "DeleteKey -> " << keyName << std::endl;
			sections_[section].erase(it);
			return true;
		}
	}

	return false;
}
