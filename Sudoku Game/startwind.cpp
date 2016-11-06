#include "startwind.h"



///////////////////////////// Fenetre d'Accueil

startWind::startWind():QWidget()
{


    setFixedSize(550, 670);// dimensions de la fenetre
    setWindowFlags(Qt::FramelessWindowHint); // na pas de barre

    // boutons Play
    Play = new QPushButton(this);
    setbutton(*Play);
    Play->setText("Play");
    Play->move(200, 400);

    // boutons Continu
    Continu = new QPushButton(this);
    setbutton(*Continu);
    Continu->setText("Continue");
    Continu->move(200, 450);

    // fenetre de type Interface
    game = new Interface();

    // si le fichier contient qlq chose , c'est donc il y a une grille à continuer ==> bouton Continu activé , sinon désactivé
    game->file.open("saved Grid.txt",ios::in);
    string s;
    game->file>>s;
    game->file.close();
    if(s.empty())
        Continu->setEnabled(false);
    else
        Continu->setEnabled(true);


    // boutons About us
    AboutUs = new QPushButton(this);
    setbutton(*AboutUs);
    AboutUs->setText("About us");
    AboutUs->move(200, 500);

    // boutons exit
    exit =new QPushButton (this);
    setbutton(*exit);
    exit->setText("Exit");
    exit->move(200, 570);

    //le background de la fenetre
    backgrImg = new QLabel(this);
    backgrImg->setGeometry(QRect(0, 0, 550, 670));
    backgrImg->setPixmap(QPixmap("back1.png"));
    backgrImg->lower();
    backgrImg->setTextInteractionFlags(Qt::NoTextInteraction);

//---------------------------Connexions liants les boutons et les slots

    connect(Play,SIGNAL(clicked()),this,SLOT(ToPlay()));
    connect(Continu,SIGNAL(clicked()),this,SLOT(ToContinu()));
    connect(AboutUs,SIGNAL(clicked()),this,SLOT(About()));
    connect(exit,SIGNAL(clicked()),qApp,SLOT(quit()));

}

//---------------------------------------------------------Personnaliser les Boutons
void setbutton(QPushButton& b)
{
    b.setFixedSize(150,50); //dimensions
    b.setStyleSheet(QLatin1String("font: 80 20pt \"Minion Pro\";\n background-color: rgb(73, 181, 231);")); // style
    b.setFlat(true); // apparait comme un label
    b.raise(); // mettre dans le premier plan
}

//------------------------------------------Affiche Les infos à props de Nous
void startWind::About()
{
    QMessageBox::about(this,"Who are we ?","Copyright© 2015 All Rights Reserved \n\n 1st Year Computing Sciences \n\n"
                                           " ENSA Marrakech Morocco.\n\n This project was a collaboration between :  "
                                           "      \n\n  °  Yassine CHEQRI \n          cheqriyass@gmail.com\n\n"
                                           "  °  Samira EL AABIDI \n          ElaabidiSamira@gmail.com\n\n"
                                           "  °  Azz-eddine BERRAMOU \n          Azedine.berramou@gmail.com\n\n"
                                           "  °  Souad BOUTANE \n          Souad.boutane@gmail.com\n\n"
                                           "  °  Chaimaa ASKAOUI \n          meowchay@gmail.com\n\n"
                                           "  °  Abdallah BENTALEB\n          bentalebabdallah@gmail.com");
}

//---------------------------------------slot pour lancer le jeu
void startWind::ToPlay()
{
    this->close(); // fermer la fenetre d'accueil
    game->Open(); // ouvrir la fenetre du choix de niveau
    game->show(); // afficher la fenetre de jeu
}

//--------------------------------------------Slot pour continuer
void startWind::ToContinu()
{
    this->close();    // fermer la fenetre d'accueil
    game->getGrid();  // extraire la grille du fichier
    game->show();    // afficher la fenetre de jeu
}

//------------------------------------------- Le Slot de la classe interface qui fait retourner le joueur vers la fenetre d'accueil
void Interface::Menu()
{
    this->close(); // fermer la fenetre du jeu
    startWind *menu=new startWind(); // creer une fenetre d'accueil
    menu->show(); // et l'afficher

}


//----------------------------------------------------- Destructeur
startWind::~startWind()
{
    delete Play;
    delete Continu;
    delete AboutUs;
    delete backgrImg;
    delete exit;

}

