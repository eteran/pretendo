
#include "Pretendo.h"
#include <QApplication>
#include <iostream>

namespace {

[[noreturn]] void usage(const char *arg0) {

	std::cout << std::endl;
	std::cout << "usage: " << arg0 << " [OPTIONS] (path | filename.nes)\n";
	std::cout << "\n";
	std::cout << "OPTIONS:\n";
	std::cout << "\t--fps <NUM>\n";
	std::cout << "\t--test-frames <NUM>\n";
	std::cout << "\t--help" << std::endl;
	exit(0);
}

}

int main(int argc, char *argv[]) {

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication app(argc, argv);

	int fps              = 60;
	uint64_t test_cycles = 0;
	QString rom;

	int i = 1;
	for (; i < argc; ++i) {
		if (strcmp(argv[i], "--fps") == 0) {
			if (i + 1 == argc) {
				usage(argv[0]);
			}
			++i;
			fps = atoi(argv[i]);
			if (fps == 0) {
				std::cerr << "ERROR: Invalid FPS" << std::endl;
				usage(argv[0]);
			}
		} else if (strcmp(argv[i], "--test-frames") == 0) {
			if (i + 1 == argc) {
				usage(argv[0]);
			}
			++i;
			test_cycles = strtoul(argv[i], nullptr, 10);
			if (test_cycles == 0) {
				std::cerr << "ERROR: Invalid test cycles" << std::endl;
				usage(argv[0]);
			}
		} else if (strcmp(argv[i], "--help") == 0) {
			usage(argv[0]);
		} else {
			rom = argv[i++];
			break;
		}
	}

	if (i != argc) {
		usage(argv[0]);
	}

	Pretendo w(rom);
	w.setFrameRate(fps);
	w.setFrameLimit(test_cycles);
	w.show();

	return app.exec();
}
