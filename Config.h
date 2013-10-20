
#ifndef CONFIG_20130421_H_
#define CONFIG_20130421_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <typeinfo>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

class Config : boost::noncopyable {
public:
	Config();
	virtual ~Config();

private:
	typedef std::map<std::string, std::string> section_type;

public:
	bool Load();
	bool Save();

public:
	bool NewSection(const std::string &section);
	bool DeleteSection(const std::string &section);
	bool NewKey(const std::string &section, const std::pair<std::string, std::string> &key);
	bool DeleteKey(const std::string &section, const std::string &keyName);

public:
	template <class T>
	bool ReadKey(const std::string &section, const std::string &key, T &container) {

		if(section.empty() || key.empty()) {
			return false;
		}
				
		std::map<std::string, section_type>::const_iterator it = sections_.find(section);

		if(it == sections_.end()) {
			std::cout << "cant find section" << std::endl;
			return false;
		}

		const section_type &kvm = it->second;
		for(section_type::const_iterator kvi = kvm.begin(); kvi != kvm.end(); ++kvi) {
			std::cout << key << ", " << kvi->first << std::endl;

			if(key == kvi->first) {
				container = boost::lexical_cast<T>(kvi->second);
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
		
		std::map<std::string, section_type>::iterator it = sections_.find(section);

		if(it == sections_.end()) {
			return false;
		}

		std::ostringstream oss;
		if(typeid(T) == typeid(double)) {
			oss << std::setprecision(4) << value;
		} else {
			oss << value;
		}

		section_type &kvm = it->second;
		for(section_type::iterator kvi = kvm.begin(); kvi != kvm.end(); ++kvi) {
			std::cout << key << ", " << kvi->first << std::endl;
			if(key == kvi->first) {
				kvi->second = oss.str();
				return true;
			}
		}

		return false;
	}

private:
	const std::string                   filename_;
	std::map<std::string, section_type> sections_;
};


#endif
