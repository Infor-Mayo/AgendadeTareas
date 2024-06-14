#include "adet.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AdeT w;
    w.show();
    return a.exec();
}
