
#ifndef CONFIG_MANAGER_20130421_H_
#define CONFIG_MANAGER_20130421_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <typeinfo>
#include <map>

class ConfigManager {
	
	public:
	ConfigManager();
	virtual ~ConfigManager();
	
	typedef std::pair<std::string, std::string> kv_pair;

public:
	typedef std::map<std::string, std::string> kv_map;
	typedef std::map<std::string, kv_map>      config_map;
	typedef kv_map::iterator                   kv_iterator;
	typedef kv_map::const_iterator             kv_const_iterator;
	typedef config_map::iterator               config_iterator;

public:
	bool Load();
	bool Save();

public:
	bool NewSection(const std::string &section);
	bool DeleteSection(const std::string &section);
	bool NewKey(const std::string &section, kv_pair key);
	bool DeleteKey(const std::string &section, const std::string &keyName);

public:
	template <class T>
	bool ReadKey(const std::string &section, const std::string &key, T &container) {

		if(section.empty() || key.empty()) {
			return false;
		}

		if(map_.find(section) == map_.end()) {
			std::cout << "cant find section" << std::endl;
			return false;
		}

		const kv_map &kvm = map_[section];
		for(kv_const_iterator kvi = kvm.begin(); kvi != kvm.end(); ++kvi) {
			std::cout << key << ", " << kvi->first << std::endl;

			if(key == kvi->first) {
				std::istringstream iss(kvi->second, std::ios::in);
				iss >> container;
				return true;
			}
		}

		return false;
	}

	template <class T>
	bool WriteKey(const std::string &section, const std::string &key, T value) {

		if(section.empty() || key.empty()) {
			return false;
		}

		if(map_.find(section) == map_.end()) {
			return false;
		}

		std::ostringstream oss;
		if(typeid(T) == typeid(double)) {
			oss << std::setprecision(4) << value;
		} else {
			oss << value;
		}

		kv_map &kvm = map_[section];
		for(kv_iterator kvi = kvm.begin(); kvi != kvm.end(); ++kvi) {
			std::cout << key << ", " << kvi->first << std::endl;
			if(key == kvi->first) {
				kvi->second = oss.str();
				return true;
			}
		}

		return false;
	}

private:
	const std::string filename_;
	config_map        map_;
};


#endif
