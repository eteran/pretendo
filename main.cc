
#include <QApplication>
#include "Pretendo.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	Pretendo w;
	w.show();

	return app.exec();
}
