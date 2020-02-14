
#include "Settings.h"
#include <QStandardPaths>
#include <QSettings>

namespace Settings {

std::string romDirectory;
int zoomFactor;
bool showSprites;

std::string configFile() {
	static const QString configDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
	static const auto configFile   = QString("%1/%2/%3").arg(configDir, "pretendo", "config.ini");
	return configFile.toStdString();
}

std::string cacheDirectory() {
	static const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
	static const auto cachePath   = QString("%1/%2").arg(cacheDir, "pretendo");
	return cachePath.toStdString();
}

std::string homeDirectory() {
	static const QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	return homeDir.toStdString();
}

void load() {
	auto filename = QString::fromStdString(configFile());
	QSettings settings(filename, QSettings::IniFormat);

	romDirectory = settings.value("romDirectory", QString::fromStdString(homeDirectory())).toString().toStdString();
	zoomFactor  = settings.value("zoomFactor", 2).toInt();
	showSprites = settings.value("showSprites", true).toBool();
}

void save() {
	auto filename = QString::fromStdString(configFile());
	QSettings settings(filename, QSettings::IniFormat);

	settings.setValue("romDirectory", QString::fromStdString(romDirectory));
	settings.setValue("zoomFactor", zoomFactor);
	settings.setValue("showSprites", showSprites);

	settings.sync();

}

}
