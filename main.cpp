#include "TelegramWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TelegramWindow w;
    w.show();

    return a.exec();
}
