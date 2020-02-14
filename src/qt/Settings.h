
#ifndef QT_SETTINGS_H_
#define QT_SETTINGS_H_

#include <string>

namespace Settings {

void load();
void save();

std::string configFile();
std::string cacheDirectory();
std::string homeDirectory();

extern std::string romDirectory;
extern int zoomFactor;
extern bool showSprites;

}

#endif
