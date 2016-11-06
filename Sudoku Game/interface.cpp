#include "interface.h"

// Constructeur de la classe sudokuCell qui represente une case de grille dans l'interface

sudokuCell::sudokuCell(QWidget *p):QLineEdit(p)
{
    id=0; //retient la position de la case dans la grille

    // signal par défaut de la classe QLineEdit lancé chaque fois que la valeur de la case a changé , le slot declenche reçoit ce signal
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(declenche(QString)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------slot recevant le signal textChanged ,il extrait la nouvelle valeur du la case et
                                            // l'envoie comme signal avec l'id de la case
void sudokuCell::declenche(QString s)
{
    int v=0;
    if(s.compare("")!=0)
    {
        v=StringToInt(s.toStdString());
    }
    emit valueChanged(id,v);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------La classe Principale du jeu

Interface::Interface():QWidget()

{
    setFixedSize(650, 570);         // dimensions de la fenetre
    setWindowTitle("Sudoku");       // titre de la fenetre
    setWindowIcon(QIcon("Im.png"));  // icone de la fenetre

    // Parametre du backgroud de la fenetre
    backgr = new QLabel(this);
    backgr->setGeometry(QRect(0, 0, 650, 570));
    backgr->setPixmap(QPixmap("back2.png"));
    backgr->lower();
    backgr->setTextInteractionFlags(Qt::NoTextInteraction);


    // Parametre du bouton de generation
    Generate = new QPushButton(this);                                                   //creation du bouton
    Generate->setText(" New Puzzle");                                                   /// le texte au sein du bouton
    Generate->setToolTip("Generate a Sudoku grid according to the level selected");     // l'info bulle lors du survol du bouton
    Generate->setToolTipDuration(3000);                                                 /// durée d'apparaition de l'info bulle
    Generate->setIcon(QIcon("start.png"));                                              // l'icone du bouton
    Generate->setGeometry(QRect(60, 520, 110, 30));                                     // dimension et position du bouton sur l'interface

        // Parametre du bouton de resolution
    Solve = new QPushButton();
    Solve->setText(" Solve Puzzle");
    Solve->setToolTip("Solve the Current Sudoku grid");
    Solve->setToolTipDuration(3000);
    Solve->setIcon(QIcon("solve.png"));
    Solve->setGeometry(QRect(220, 520, 110, 30));

    // Parametre du bouton de recomentcement
    restart = new QPushButton();
    restart->setText(" Restart Game");
    restart->setToolTip("Reset the original Sudoku grid ");
    restart->setToolTipDuration(3000);
    restart->setIcon(QIcon("restart.png"));
    restart->setGeometry(QRect(520, 70, 110, 30));

        // Parametre du bouton de Redo
    redob = new QPushButton();
    redob->setIcon(QIcon("redo.png"));
    redob->setToolTipDuration(3000);
    redob->setText(" Redo");
    redob->setGeometry(QRect(520, 120, 110, 30));

        // Parametre du bouton de Undo
    undob = new QPushButton();
    undob->setText("Undo");
    undob->setToolTipDuration(3000);
    undob->setIcon(QIcon("undo.png"));
    undob->setGeometry(QRect(520, 170, 110, 30));

        // Parametre du bouton de Save
    save = new QPushButton();
    save->setText(" Save puzzle");
    save->setToolTip("Save the Current Played Sudoku grid");
    save->setToolTipDuration(3000);
    save->setIcon(QIcon("save.png"));
    save->setGeometry(QRect(520, 330, 110, 30));

    //    // Parametre du bouton de reteour au menu
    back = new QPushButton(this);
    back->setText(" Menu");
    back->setToolTipDuration(3000);
    back->setIcon(QIcon("b.png"));
    back->setGeometry(QRect(520, 380, 110, 30));

        // Parametre du chronometre
    time=new QTime();
    time->setHMS(0,0,0,0);
    timer=new QTimer(this);

    // Parametre de l'afficheur du chrono
    elaps=new QLabel(this);
    elaps->setText("");
    elaps->setGeometry(10,16,350,30);
    elaps->setStyleSheet(QLatin1String("font: 75 16pt \"Arial\";"));

    // Parametre de l'afficheur du niveau selectioné
    lev=new QLabel(this);
    lev->setGeometry(320,16,200,30);
    lev->setStyleSheet(QLatin1String("font: 75 16pt \"Arial\";"));


    // la widget contenant le GridLayout
    GLW = new QWidget(this);
    GLW->setGeometry(QRect(10, 50, 460, 460));

    // le grid layout qui contiendra les 81 cases
    GL = new QGridLayout(GLW);

//------------------------------------------Select level window : fenetre de choix du niveau
        w=new QWidget();
        w->setWindowModality(Qt::ApplicationModal);
        w->setFixedSize(320,320);
        w->setWindowTitle("Level");

        // le background
        bg = new QLabel(w);
        bg->setGeometry(QRect(0, 0, 320, 320));
        bg->setPixmap(QPixmap("Back3.png"));
        bg->lower();
        bg->setTextInteractionFlags(Qt::NoTextInteraction);

        // bouton start pour valider le choix du niveau et commencer le jeu
        start=new QPushButton("Start");
        start->setFixedSize(80,30);

        // les Boutons radio ,chacun pour un niveau
        L1=new QRadioButton("Easy");
        L1->setStyleSheet(QLatin1String("font: 75 16pt \"Bell MT\";"));
        L2=new QRadioButton("Medium");
        L2->setStyleSheet(QLatin1String("font: 75 16pt \"Bell MT\";"));
        L3=new QRadioButton("Hard");
        L3->setStyleSheet(QLatin1String("font: 75 16pt \"Bell MT\";"));
        L4=new QRadioButton("Evil");
        L4->setStyleSheet(QLatin1String("font: 75 16pt \"Bell MT\";"));

        // le layout vertical qui contient les 4 boutons radio
        vbox=new QVBoxLayout();

        // l'ajout des boutons radio
        vbox->addWidget(L1);
        vbox->addWidget(L2);
        vbox->addWidget(L3);
        vbox->addWidget(L4);

        // le widget qui regroupe les boutons radio sous le titre "Choose a Level"
        GB=new QGroupBox();
        GB->setTitle("Choose a Level");
        GB->setStyleSheet(QLatin1String("font: 75 24pt \"Bell MT\";"));
        GB->setLayout(vbox);
        GB->setAlignment(Qt::AlignCenter);
        GB->setMaximumHeight(300);

        // le layout de la fenetre , il contient le groupement et le bouton start
        VL=new QVBoxLayout();
        VL->addWidget(GB);
        VL->addWidget(start);

        VL->setAlignment(Qt::AlignCenter);

        w->setLayout(VL);

//--------------------------------------------------------- Connexions : chaque signal d'un des boutons est relié au slot equivalent

       connect(Generate,SIGNAL(clicked()),this,SLOT(Open()));
       connect(start,SIGNAL(pressed()) ,this,SLOT(getLevel()));
       connect(back,SIGNAL(clicked()),this,SLOT(Menu()));
       connect(restart,SIGNAL(clicked()),this,SLOT(Restart()));
       connect(Solve,SIGNAL(clicked()),this,SLOT(SolveP()));
       connect(save,SIGNAL(clicked()),this,SLOT(saveGrid()));
       connect(redob,SIGNAL(clicked()),this,SLOT(RedoMove()));
       connect(undob,SIGNAL(clicked()),this,SLOT(UndoMove()));

//------------------------------------------------------- Setting Up Cells : creation des 81 cases et les personnaliser

       for(int i=0;i<9;i++)
           for(int j=0;j<9;j++)
       {
           int v=i*9+j;

           T[v] = new sudokuCell(GLW);      // parent de chaque case est GLW
           T[v]->id=v;                      /// la postion est retenue
           T[v]->setEnabled(false);         // case desactivée : n'accepte pas la saisie
           QRegExp r("^[1-9]$");                               // expression regulière n'acceptant qu'une des valeurs 1 à 9
           QRegExpValidator *val=new  QRegExpValidator(r,T[v]);  // crer les validateur de champ selon l'expression reguliere

           T[v]->setFixedSize(40,40);  // dimension de la case
           T[v]->setFrame(true);       // elle a une bordure
           T[v]->setAlignment(Qt::AlignCenter);    //texte centré
           T[v]->setValidator(val);            // applique le validateur

           // les cases blues
           if(v<3 ||(v>5 && v<12) || (v>14 && v<21) ||(v>23 && v<27)||(v>53 && v<57)||(v>59 && v<66)||(v>68 && v<75) ||v>77 || (v>29 && v<33) || (v>38 && v<42) ||(v>47 && v<51))

               // appliquer un style
               T[v]->setStyleSheet(QLatin1String("background-color: rgb(222, 233, 255);\n font: 75 16pt \"Arial\";"));


           else
               // les oranges
               T[v]->setStyleSheet(QLatin1String("background-color: rgb(255, 211, 166);\n""font: 75 16pt \"Arial\";"));

           // sauvegarder le style pour des utilisations ultérieures
           T[v]->color = T[v]->styleSheet();

           // ajouter les cases au Gridlayout
           GL->addWidget(T[v],i+1,j+1);
       }

}

//------------------------------------ settime : le reglage du chrono

void Interface::settime()
{
    QTime newT;
    sec++;
    newT = time->addSecs(sec);
    QString txt = newT.toString("mm:ss");
    elaps->setText("Elapsed Time:  " + txt);
}


//------------------------------------ setGrid : fait en sorte que la grille de l'interface est parfaitement preparer pour le jeu

void Interface::setGrid(MatrixOfInt& g,MatrixOfString& p)
{
    for (int r=0; r<9; r++)
    {
        for (int c=0; c<9; c++)
        {

            QString s=intToQstr(g.grid[r][c]);          //extraire la valeur de la case
            T[r*9+c]->setStyleSheet(T[r*9+c]->color);   // definir le style par défaut de la case

            // deconnecter le signal pour effectuer les changements sans emettre de signal
            disconnect(T[r*9+c],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));

            // pour les cases vides (=0)
            if((s.toStdString()).compare("0")==0)
            {
                T[r*9+c]->clear();  //vider le champ
                p.possible[r][c]=CalculatePossibleValues(r,c); // calculer les candidats

                // si niveau "Easy", afficher le candidats comme infos bulles,avec une durée de 2s
                if( x == 1)
                {
                    T[r*9+c]->setToolTip(QString::fromStdString(p.possible[r][c]));
                    T[r*9+c]->setToolTipDuration(2000);
                }

                // si le champ n'accept pas la saisie , l'activer pour qu'il le fait
                if( !T[r*9+c]->isEnabled() )    T[r*9+c]->setEnabled(true);

            }

            // case remplie
            else
            {
                T[r*9+c]->setToolTip("");       //pas d'info bulles
                T[r*9+c]->setText(s);           // mettre la valeur dans la case

                //si le champ est activé , le desactiver
                if( T[r*9+c]->isEnabled())
                      T[r*9+c]->setEnabled(false);
            } 

            // reconnecter le signal pour detecter les valeurs saisies
                connect(T[r*9+c],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));
        }

    }
}

//------------------------------------ Open : pour ouvrir la fenetre du choix de niveau

void Interface::Open()
{
    w->show();
}


//------------------------------------ getLevel : pour affecter à x la bonne valeur selon le bouton radio coché

void Interface::getLevel()
{

    ResetPossible(); // vidage la grille des candidats

    x=0;
    if (L1->isChecked())// si le premier niveau est coché

    {
        x=1;
            //afficher le niveau
        lev->setText("Level : Easy");
    }
    else
    {
        if(L2->isChecked())
        {
            x=2;

            lev->setText("Level : Medium");

        }
        else
        {
            if(L3->isChecked())
            {
                x=3;
                lev->setText("Level : hard");
            }
            else
            {
                if(L4->isChecked())
                {
                    x=4;
                    lev->setText("Level : Evil");
                }

            }
        }

    }

       if(x==0) // si aucun niveau n'est selectionné ,afficher un message
          QMessageBox::warning(w,"Level","Please select a level");

       //sinon
       else if (x==1 || x==2 || x==3 || x==4)

        {
           // toutes les cases n'ont pas d'info bulles
           for(int i=0;i<81;i++)
               T[i]->setToolTip("");

           // afficher ces boutons
            save->setParent(this);
            save->show();
            Solve->setParent(this);
            Solve->show();
            restart->setParent(this);
            restart->show();
            redob->setParent(this);
            redob->show();
            undob->setParent(this);
            undob->show();

            // arreter le chrono ,pour le regler
            disconnect(timer,SIGNAL(timeout()),this,SLOT(settime()));
            elaps->setText("Elapsed Time:  00:00");
            sec=0;
            timer->start(1000);

            // une fois cliquer sur start, la fenetre du choix de niveau disprait
            connect(start,SIGNAL(clicked()),w,SLOT(close()));

            // generer une grille convenable
            initialiser(x);
            //l'afficher sur l'interface
            set();

            // le chrono commence
            connect(timer,SIGNAL(timeout()),this,SLOT(settime()));


        }
}


///-------------------------------- au cas où la grille n'a pas de solution , cette fonction est appelée

void Interface::openNoSolution()
{
    // afficher un message et question, avec reponse Yes ou No
    int rep=QMessageBox::warning(this,"Error","You've made this Puzzle Unsolvable ! \n\n Would you like to Solve the Original one ?\n",QMessageBox::Yes,QMessageBox::No);

    // si le joueur clique sur Yes
    if(rep == QMessageBox::Yes)

    {
        // la grille complete est affichée
        resetGrid();
        set();
    }

    // si le joueur clique sur No
    else if (rep == QMessageBox::No)
    {
        // grille vidée
         clearGrid();
    }
}

///------------------------------------- Slot recevant la position de la case recemment changée et la valeur dedans et appel la fonction de test


void Interface::onValueChanged(int p,int v)
{

        TestTheEnteringValue(p,v);
}

///---------------------------------------------------------- UndoMove :


void Interface::UndoMove()
{
    int id,value;
    int r,c;

    // appel à la fonction booleenne qui change la valeur de id et value par celles du dernier coup dans la pile,
    //si elle les a changées, elle retourne true , sinn false
    if (UndoM(id,value))
    {
        cout << "UNDO" << endl;

        /// extraire la ligne et la colonne de l'id =position
        r = id/9;
        c = id%9;

        cout << r << c << endl;

        // deconnecter pour changer la case (r,c) sans emettre de signal
        disconnect(T[id],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));

        // si le dernier coup effectué est le vidage
        if (value == 0 )
        {
            T[id]->clear(); // on supprime le contenu actuel de cette case
            T[id]->setStyleSheet(T[id]->color); //on applique le style par defaut
        }

        // sinon si le dernier coup effectué est une valeur non nulle
        else
         {   T[id]->setText(intToQstr(value)); // afficher cette valeur dans la case de la grille d'interface

            // si cette valeur n'est pas un candidat de la case , afficher la case en rouge
            if(CalculatePossibleValues(id/9,id%9).find(to_string(value)) == string::npos)
                T[id]->setStyleSheet(QLatin1String("background-color: rgb(236, 0, 0);\n" "font: 75 16pt \"Arial\";"));
        }

        // reconnecter le signal
        connect(T[id],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));

        //mettre cette valeur dans  la grille du jeu
        SetCell(r,c,value);
    }
}

//------------------------------------ RedoMove : meme principe que UndoMove

void Interface::RedoMove()
{
    int id, value;
    int r,c;

    if (RedoM(id, value))
    {
        cout << "REDO" << endl;

        r = id/9;
        c = id%9;

        disconnect(T[id],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));

        if (value == 0 )
        {
            T[id]->clear();
            T[id]->setStyleSheet(T[id]->color);
        }
        else
        {
            T[id]->setText(intToQstr(value));
            if(CalculatePossibleValues(id/9,id%9).find(to_string(value)) == string::npos)
                T[id]->setStyleSheet(QLatin1String("background-color: rgb(236, 0, 0);\n" "font: 75 16pt \"Arial\";"));

        }

        connect(T[id],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));

        SetCell(r,c,value);
    }
}
//-------------------------------------------------------------Simple Setting for Continue

void Interface::simpleSet(MatrixOfInt& g ,MatrixOfString& p)

{
    //evaluer la valeur sauvegardés dans le fichier et qui a ete mise dans x pour afficher le niveau de la grille sauvegardée
    if( x == 1 ) lev->setText("Level : Easy");
    else if( x == 2 ) lev->setText("Level : Medium");
    else if( x == 3 ) lev->setText("Level : Hard");
    else if( x == 4 ) lev->setText("Level : Evil");


    for (int r=0; r<9; r++)
    {
       for (int c=0; c<9; c++)
       {
           // extraire la valeur de case de grille du jeu
           QString s=intToQstr(g.grid[r][c]);
           // appliquer le style par defaut
           T[r*9+c]->setStyleSheet(T[r*9+c]->color);

           // deconnecter le signal pour effectuer les changement sans emmettre de signal
           disconnect(T[r*9+c],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));

           // si la case est vide (valeur = 0)
           if((s.toStdString()).compare("0") == 0)
           {
               // champ vide
                T[r*9+c]->setText("");

                // si le niveau est "Easy" , afficher les candidats sous forme d'infos bulles
                if( x == 1 )
                {
                    T[r*9+c]->setToolTip(QString::fromStdString(p.possible[r][c]));
                    T[r*9+c]->setToolTipDuration(2000);
                }

            }
           // si la valeur n'est pas nulles 
           else
           {
               // si le champ est activé , c'est que cette valeur a ete saisie par le joueur
               if(T[r*9+c]->isEnabled())
                {
                   T[r*9+c]->setText(s); // afficher la valeur

                   // si le niveau est "Easy" , afficher les les candidats sous forme d'infos bulles
                    if( x == 1 )
                    {
                        T[r*9+c]->setToolTip(QString::fromStdString(p.possible[r][c]));
                        T[r*9+c]->setToolTipDuration(2000);
                    }

                    // si la valeur ne figure dans la liste des candidats , la case deviendrait rouge
                   if (p.possible[r][c].find(s.toStdString()) == string::npos)
                    {
                       T[r*9+c]->setStyleSheet(QLatin1String("background-color: rgb(236, 0, 0);\n" "font: 75 16pt \"Arial\";"));
                    }
                }
               // sinon c'est une valeur donnée pr défaut
              else
                {
                    T[r*9+c]->setToolTip("");
                    T[r*9+c]->setText(s);
                }
           }
           // reconnecter le signal
            connect(T[r*9+c],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));
        }
    }

    // arreter le chrono , le regler et le relancer
    disconnect(timer,SIGNAL(timeout()),this,SLOT(settime()));
    elaps->setText("Elapsed Time:  00:00");
    sec=0;
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(settime()));
}


//------------------------------------ Interface's Destructor


Interface::~Interface()
{
    for(int i=0;i<81;i++)
        delete T[i];

    delete lev;
    delete start;
    delete L1;
    delete L2;
    delete L3;
    delete L4;
    delete vbox;
    delete GB;
    delete VL;
    delete bg;
    delete w;
    delete timer;
    delete time;
    delete elaps;
    delete Generate;
    delete undob;
    delete redob;
    delete restart;
    delete back;
    delete save;
    delete Solve;
    delete GL;
    delete GLW;
}

