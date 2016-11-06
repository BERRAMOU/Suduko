#include<QApplication>
#include"interface.h"
#include <QStyleFactory>
#include <QPixmap>
#include "startwind.h"


int main(int a,char* b[])
{
    QApplication app(a,b);
    app.setStyle(QStyleFactory::create("Fusion"));

    startWind win;

    win.show();

    return app.exec();


}
