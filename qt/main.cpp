
#include <QApplication>
#include <iostream>
#include "Pretendo.h"

namespace {

void usage(const char *arg0) {
	
	std::cout << std::endl;
	std::cout << "usage: " << arg0 << " [OPTIONS] (path | filename.nes)" << std::endl;
	std::cout << std::endl;
	std::cout << "OPTIONS:" << std::endl;
	std::cout << "\t--fps <NUM>" << std::endl;
	std::cout << "\t--help" << std::endl;
	exit(0);

}

}

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	int fps = 60;
	QString rom;
	
	int i = 1;
	for(; i < argc; ++i) {
		if(strcmp(argv[i], "--fps") == 0) {
			if(i + 1 == argc) {
				usage(argv[0]);
			}
			++i;
			fps = atoi(argv[i]);
			if(fps == 0) {
				std::cerr << "ERROR: Invalid FPS" << std::endl;
				usage(argv[0]);
			}
		} else if(strcmp(argv[i], "--help") == 0) {
			usage(argv[0]);
		} else {
			rom = argv[i];
			break;
		}
	}
	
	if(i != (argc - 1)) {
		usage(argv[0]);
	}

	Pretendo w(rom);
	w.setFrameRate(fps);
	w.show();

	return app.exec();
}
