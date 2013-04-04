/* $Header: /var/cvsroot/pretendo/misc/ConfigManager.h,v 1.10 2006/01/17 02:15:11 eteran Exp $ */
// ConfigFile.h

#ifndef CONFIG_FILE_H_
#define CONFIG_FILE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <typeinfo>
#include <map>

using std::string;
using std::pair;
using std::map;
using std::ostringstream;
using std::istringstream;
using std::ios;
using std::cout;
using std::endl;
using std::fstream;



class ConfigManager
{
	typedef pair<string, string> kv_pair;

	public:
	typedef map<string, string> kv_map;
	typedef map<string, kv_map> config_map;
	typedef kv_map::iterator kv_iterator;
	typedef config_map::iterator config_iterator;
	
	protected:
	ConfigManager();
	
	public:
	bool Load (void);
	bool Save (void);
	
	public:
	bool NewSection (string const &section);
	bool DeleteSection (string const &section);
	bool NewKey (string const &section, kv_pair key);
	bool DeleteKey (string const &section, string const &keyName);
	
	
	public:
	template<typename T> 
	bool ReadKey (string const &section, string const &key, T &container)
	{
		if (section.empty() || key.empty()) {
			return false;
		}
		
		config_iterator ci = mMap.find (section);
		if (ci == mMap.end()) {
			cout << "cant find section" << endl;
			return false;
		}
		
		kv_map *kvm = &mMap[section];
		
		for (kv_iterator kvi = kvm->begin(); kvi != kvm->end(); kvi++) {
			cout << key << ", " << kvi->first << endl;
			if (key == kvi->first) {
				
				istringstream iss (kvi->second, ios::in);
				iss >> container;
				return true;	
			}
		}
		
		return false;
	}
	
	template<typename T> 
	bool WriteKey (string const &section, string const &key, T value)
	{
		if (section.empty() || key.empty()) {
			return false;
		}

		if (mMap.find (section) == mMap.end()) {
			return false;
		}
		
		ostringstream oss;
		if (typeid(T) == typeid(double)) {
			oss << std::setprecision (4) << value;
		} else {
			oss << value;
		}
		
		kv_map *kvm = &mMap[section];
		for (kv_iterator kvi = kvm->begin(); kvi != kvm->end(); kvi++) {
			cout << key << ", " << kvi->first << endl;
			if (key == kvi->first) {
				kvi->second = oss.str();
				return true;	
			}
		}
		
		return false;
	}


	private:
	string mFilename;
	config_map mMap;
};


#endif // CONFIG_FILE_H_
