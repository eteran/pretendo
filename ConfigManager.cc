/* $Header: /var/cvsroot/pretendo/misc/ConfigManager.cpp,v 1.10 2005/09/17 21:23:00 eli Exp $ */

#include "ConfigManager.h"


ConfigManager::ConfigManager()
	: mFilename("pretendo.cfg")
{
} 


bool
ConfigManager::Load (void)
{
	cout << "Loading config from file..." << endl;

	fstream file;
	file.open (mFilename.c_str(), fstream::in);	
	
	if (! file.is_open()) {
		// file does not exist, make a new one with some defaults
		cout << "Couldn't load file.  Creating new one..." << endl;
		
		NewSection("App Settings");
		NewKey("App Settings", kv_pair("ShowOpenOnLoad", "false"));
		NewKey("App Settings", kv_pair("AutoRun", "false"));
		NewKey("App Settings", kv_pair("SleepOnLoseFocus", "false"));
		NewKey("App Settings", kv_pair("ROMDirectory", "./roms"));
		NewKey("App Settings", kv_pair("UseROMDB", "false"));
		NewKey("App Settings", kv_pair("DBFile", "./roms.db"));
		Save();
		
		return false;
	}
	
	string linebuffer;
	unsigned int line = 0;
	string section;
	
	while (! getline (file, linebuffer).eof()) {
		++line;
		if (linebuffer.empty()) {
			continue;
		}
		
		if (linebuffer.at(0) == '[') {
			linebuffer.erase (0, 1);
			
			size_t end = linebuffer.find_last_of(']');
			
			if (end != string::npos) {
				linebuffer.erase (end, 1);
				NewSection (linebuffer);
				section = linebuffer;
			} else {
				cout << "ConfigFile: error on line " << line << endl;
				continue;
			}
		} else {
			size_t eqPos = linebuffer.find_first_of ("=");
			if (eqPos == string::npos) {
				cout << "Error loading config file, line " << line << endl;
				continue;
			}
			
			string key = linebuffer;
			string value = linebuffer;
			
			key.erase (eqPos);
			value.erase (0, eqPos+1);
			
			if (key.empty()) {
				cout << "config file: bad key on line " << line << endl;
				continue;
			}
			
			NewKey (section, kv_pair(key, value));		
		}	
	}
	
	file.close();

	return true;
}


bool
ConfigManager::Save (void)
{
	cout << "Saving Config..." << endl;	
	
	fstream file;
	file.open(mFilename.c_str(), fstream::out | fstream::trunc);
	
	if (! file.is_open()) {
		cout << "Error:  Config::Save() couldn't open file for writing" << endl;
		//FIXME: throw exception or something equally creative
		return false;
	}
	
	for (config_iterator ci = mMap.begin(); ci != mMap.end(); ci++) {
		kv_iterator kvm_start = mMap[ci->first].begin();
		kv_iterator kvm_end = mMap[ci->first].end();
		
		file << "\n[" << ci->first << "]" << endl;
		
		for (kv_iterator ki = kvm_start; ki != kvm_end; ki++) {
			if (ki->first.empty() || ki->second.empty()) {
				continue;
			}
			
			file << ki->first << "=" << ki->second << endl;
		}
	}
		
	file.flush();
	file.close();
	
	return true;
}

	
bool 
ConfigManager::DeleteSection (string const &section)
{
	cout << "DeleteSection -> " << section << endl;
	config_iterator ci = mMap.find (section);
	
	if (ci == mMap.end()) {
		return false;
	}
	
	if (! mMap[section].empty()) {
		mMap[section].clear();		
	}
	
	mMap.erase(ci);

	return true;
}


bool
ConfigManager::NewSection (string const &section)
{
	cout << "NewSection: " << section << endl;
	
	if (section.empty()) {
		return false;
	}

	if (mMap.find (section) != mMap.end()) {
		cout << "Section: " << section << " already in list." << endl;
		return false;
	}
	
	mMap[section] = kv_map();
	return true;
}


bool 
ConfigManager::NewKey (string const &section, kv_pair key)
{
	if (section.empty()) {
		return false;
	}
	
	if (mMap.find(section) == mMap.end()) {
		return false;
	}
	
	kv_map kvm = mMap[section];
	for (kv_iterator i = kvm.begin(); i != kvm.end(); i++) {
		if (i->first == key.first) {
			cout << "key " << key.first << " already exists." << endl;
			return false;
		}
	}

	cout << "NewKey -> adding: " << key.first << ", " << key.second << endl;
	mMap[section].insert(key);
	return true;
}
	
	
bool
ConfigManager::DeleteKey (string const &section, string const &keyName)
{
	if (section.empty()) {
		return false;
	}
	
	kv_map kvm = mMap[section];
	kv_iterator kvi;
	for (kvi = kvm.begin(); kvi != kvm.end(); kvi++){
		if (kvi->first == keyName) {
			cout << "DeleteKey -> " << keyName << endl;
			mMap[section].erase(kvi);
			return true;
		}
	}
	
	return false;
}
