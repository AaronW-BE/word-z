#include "stdafx.h"
#include "wordz.h"
#include "wordzapplication.h"

int main(int argc, char *argv[])
{
    WordZApplication app(argc, argv);

    if (app.isRunning()) {
        return 0;
    }

    QApplication::setStyle("Windows");

    WordZ window;
    window.show();
    return app.exec();
}
