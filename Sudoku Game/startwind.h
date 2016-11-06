#ifndef STARTWIND
#define STARTWIND

#include "interface.h"

class startWind : public QWidget
{
    Q_OBJECT
public:
    startWind();
    ~startWind();
    QPushButton *Continu;

    friend void setbutton(QPushButton&);
private:
    QPushButton *Play;

    QPushButton *AboutUs;
    QPushButton *exit;
    QLabel *backgrImg;
    Interface *game;


public slots :
    void About();
    void ToPlay();
    void ToContinu();


};

void setbutton(QPushButton&);

#endif // STARTWIND

