#ifndef INTERFACE_H
#define INTERFACE_H

#include <QTime>
#include <QDialog>
#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
#include <QTimer>

#include <iostream>
#include <string.h>
#include <iomanip>
#include <stack>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <math.h>

using namespace std;

namespace patch
{
    template < typename T > string to_string( const T& n )
        {
            ostringstream stm ;
            stm << n ;
            return stm.str() ;
        }
}

using namespace patch;

//------------------------------------ Structure
typedef struct MatrixOfInt
{
    int grid[9][9];
}MatrixOfInt;

typedef struct MatrixOfString
{
    string possible[9][9];
}MatrixOfString;


///////////////////////////////////////////
/// \brief The sudokuCell class
///

class sudokuCell :public QLineEdit
{
    Q_OBJECT

public :
    int id;
    QString color;
    sudokuCell(QWidget *p=0);


public slots:
    void declenche(QString);

signals:
    void valueChanged(int,int);
};


/////////////////////////////////////////////
/// \brief The Interface class
///
class Interface :public QWidget
{
    Q_OBJECT

public:

    sudokuCell *T[81];                              // the 81 cells to create the puzzle;
    int x;                                          // x holds the level's index ={1,2,3,4}
    fstream file;                                   // where the grid is saved as a string

    Interface();
    ~Interface();

    void set();                                     // a simple function to call setGrid                        // goto interface.cpp
    void simpleSet(MatrixOfInt&,MatrixOfString&);   // Simple setting for Continuing a saved puzzle             // goto sudoku.cpp
    void setGrid(MatrixOfInt&,MatrixOfString&);     // Setting for new puzzle                                   // goto sudoku.cpp
    void TestTheEnteringValue(int , int);           // Testing if the value entered by the user is Valid or Not // goto sudoku.cpp
    void openNoSolution();                    /* the user can make a move which makes the grid unsolvable in the long term, so this
                                                        slot propose what to do in this case*/   ///goto interface.cpp



public slots:
    void getGrid();                           /* to get the saved grid after clicking the button "Continue"*/     /// goto sudoku.cpp

    void saveGrid();                          /* to save the current played grid after clicking the button "Save"*/  /// goto sudoku.cpp

    void Open();                              /* open a window where's the level list to choose one and click start*/ ///goto interface.cpp

    void getLevel();                          /* launched by the click on start, it's to get the level and setting
                                                the grid with the appropriate level*/  /// goto interface.cpp

    void SolveP();                            /* the solve the grid */              /// goto sudoku.cpp

    void settime();                           /* to set the chronometer */          /// goto interface.cpp

    void onValueChanged(int , int);           /* the slot which get the position and value of the user's input  */  /// goto interface.cpp

    void UndoMove();                          /* to undo a move  */             /// goto interface.cpp

    void RedoMove();                          /* to redo a move */              /// goto interface.cpp

    void Restart();                           /* to reset the grid with the intial generated grid */   /// goto sudoku.cpp




    void clearGrid();                         /* to empty the grid */          /// goto sudiku.cpp

    void Menu();                              /* to get back to the Main Menu ( class startWind ) */   /// goto startWind.cpp

private:
/////////////////////////////////////// Window "Choose level" 's widgets

    QLabel *lev,*elaps,*bg;
    QPushButton *start;
    QVBoxLayout *vbox,*VL;
    QRadioButton *L1,*L2,*L3,*L4;
    QGroupBox *GB;
    QWidget *w;

//////////////////////////////////////// Grid Window 's widgets


    QPushButton *save,*restart,*back;
    QPushButton *redob,*undob;
    QPushButton *Generate,*Solve;
    int sec;
    QTime *time;
    QTimer *timer;

    QLabel * backgr;
    QWidget *GLW;
    QGridLayout *GL;

};


//------------------------------------- Fonctions du SUDOKU

int getValueFromGrid(int,int);
QString intToQstr(int );
int charToInt(char );
int StringToInt(string);

string GetPuzzle(int);

void initialiser(int);
void initialise(int grid[9][9], string);

void ResetBruteForce();
void ResetPossible();
void resetStacks();
void resetGrid();


void SetCell(int, int, int);
bool IsPuzzleSolved();

void afficherPossible();
void afficherGrid();

bool UndoM(int &, int &);
bool RedoM(int &, int &);

void ResetPossible();
string CalculatePossibleValues(int, int);

bool CRME();

bool LookForTripletsInMinigrids();
bool LookForTripletsInRows();
bool LookForTripletsInColumns();

bool LookForLoneRangersInMinigrids();
bool LookForLoneRangersInRows();
bool LookForLoneRangersInColumns();

bool LookForTwinsInMinigrids();
bool LookForTwinsInRows();
bool LookForTwinsInColumns();

bool SolvePuzzle();
void FindCellWithFewestPossibleValues(int&, int&);
void SolvePuzzleByBruteForce();

string ConvertGridToStr(int m[9][9]);

string GenerateEasy();
string easy();
string GenerateMedium();
string medium();
string GenerateHard();
string hard();
string GenerateEvil();
string Evil();


#endif // INTERFACE_H
