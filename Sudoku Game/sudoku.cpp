#include "interface.h"
#include "startwind.h"


using namespace std;


string grille = "000000000000000000000000000000000000000000000000000000000000000000000000000000000"; // Empty Grid


// -------------------------------- Variables Globales

MatrixOfInt grid;                   //grille du jeu
MatrixOfString possible;            // grille des candidats
stack<string> Moves, RedoMoves;     // piles pour stocker les coups effectués
int totalScore;                     // score indiquant l'effort mis en palce pour résoudre un puzzle
ofstream Logs("Logs.txt");          // fichier pour suivre les activités des algorithmes
bool BruteForceStop;                // pour tester si on doit arreter le brute force ou non
string grid_backup;                 // grille générée complète
string grid_backup_dug;             // grille générée creusée
int CanBeDugg[9][9];                // grille indiquant les zones à creusées dans la grille générée complete

stack<MatrixOfInt> gridStack;       // pile de matrice pour sauvegarder la grille du jeu à un etat donné
stack<MatrixOfString> possibleStack; // pile de matrice pour sauvegarder la grille des candidats à un etat donné



// --------------------------------- string to int

int StringToInt(string s)
{
    char a=s[0];
    if (a == 0)
            return 0;
    else
        return a-48;
}
// ---------------------------------------- int to Qstring

QString intToQstr(int n)
{
    QString s = QString::fromStdString(to_string(n));
    return s;
}

// --------------------------------- charToInt


int charToInt(char c)
{
    return c-48;
}

// ---------------------------------------Randomize : donne une valeur aleatoire entre a et b
int Randomize(int a,int b)
{
    return (rand()%(b-a)+a);
}
// --------------------------------- setCell : pour mettre la valeur reçue dans les coordonneés reçues

void SetCell(int row, int col, int val)
{
    //---save the value in the array

    grid.grid[row][col] = val;

    Logs << "Cell [ " << row+1 << " , " << col+1 << " ] confirmed with value " << val << endl;



        //---if erasing a cell, you need to reset the possible values for all cells

    if (val == 0)
       for(int r=0; r<9; r++)
       {
           for(int c=0; c<9; c++)
           {
               if(grid.grid[r][c] == 0)
               {
                   possible.possible[r][c]="";
               }
            }
        }

    //  recalculate possible values
    for (int i=0; i<9; i++)
        for (int j=0; j<9; j++)
        {
            if (grid.grid[i][j] == 0)
            {
                possible.possible[i][j]=CalculatePossibleValues(i,j);
            }
        }
}

// --------------------------------- TestTheEnteringValue : tester la valeur saisi par le joueur

void Interface::TestTheEnteringValue(int id,int v)
{
    int r = id/9;

    int c = id%9;

    // extraire l'ancienne valeur de la cellule , au cas où on veut annuler/ refaire le coup
    int oldValue = grid.grid[r][c];
    Moves.push(to_string(r)+to_string(c) + to_string (oldValue));

    // la mettre dans la grille
    SetCell(r,c,v);


    // tester si ce coup est le dernier,avec lequel la grille serait résolue
    if (IsPuzzleSolved())
    {
        timer->stop();
        int rep = QMessageBox::information(this,"Winner","Congratulations!\n You Solved it !",QMessageBox::Ok);

        if(rep == QMessageBox::Ok)
             clearGrid();
    }


    else
    {
        // on regle les infos bulles de grille ,on les affiche si le niveur est "Easy" (x=1)
        for (int i=0; i<9; i++)
             for (int j=0; j<9; j++)
                 if (grid.grid[i][j] == 0)
                 {
                     T[i*9+j]->setToolTip("");
                        if(x == 1)
                     {
                        T[i*9+j]->setToolTip(QString::fromStdString(possible.possible[i][j]));
                     }
                  }

        // si a valeur est 0 (= case vide) on restaure la couleur originale
        if (v == 0)

        {
            T[id]->setStyleSheet(T[id]->color);
        }

        // sinon
        else
        {
            // si la valeur saisie est interdite , on affiche a case en rouge
            if (possible.possible[r][c].find(to_string(v)) == string::npos)
            {
                T[id]->setStyleSheet(QLatin1String("background-color: rgb(236, 0, 0);\n" "font: 75 16pt \"Arial\";"));
            }
        }
     }
}


// --------------------------------- initialiser : mettre la grille de teype string dans la matrice du jeu

void initialise(int grid[9][9], string gril)
{
    for (int r=0; r<9; r++)
        for (int c=0; c<9; c++)
            grid[r][c] = charToInt(gril[r*9+c]);

}


// --------------------------------- resetStacks : vider les piles des coups

void resetStacks()
{
    while(!Moves.empty()) Moves.pop();
    while(!RedoMoves.empty()) RedoMoves.pop();
}


// --------------------------------- GetPuzzle : générer un puzzle en fonction du niveau indiqué par level(= Interface::x ) passé par parametre

string GetPuzzle(int level)
{

    string result=grille;
    totalScore = 0;         //score =0 au début
    ResetBruteForce();      // restaurer les parametres du brute force à leur etat initial

    //generer un puzzle avec du brute force à partir d'une grille vide
    if (!SolvePuzzle())
    {
        SolvePuzzleByBruteForce();
    }

    //backup de la grille sous forme string
    grid_backup = ConvertGridToStr(grid.grid);

    // creuser selon le niveau selectionné
    if (level == 1)
    {
        return easy();
    }

    else if (level == 2)
    {
        return medium();
    }

    else if (level == 3)
    {
        return hard();
    }

    else if (level == 4)
    {
        return Evil();
    }

    return result;
}

// --------------------------------- initialiser : se charge pour obtenir une grille du niveau x passé par parametre

void initialiser(int x)
{

    string result;

    // vider les piles et la grille possible
    resetStacks();
    ResetPossible();


    cout<<"grille originale vide:"<<grille<<endl;
    grid_backup=grille;

    // remplir la grille avec des 0
    initialise(grid.grid, grille);

    // obtenir la grille creusée et prête à etre mise dans la grille du jeu
    result = GetPuzzle(x);

    cout<<"grid backup :"<<grid_backup<<endl;
    cout << "grille creusee :" << grid_backup_dug << endl;

    initialise(grid.grid, result);
}


// -----------------------------------------set : prepare les case
void Interface::set()
{
    // activer les cases pour accepter la saisie et conncter le signal de changement de valeur
    for(int i=0;i<81;i++)
    {
        T[i]->setEnabled(true);
        connect(T[i],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));
    }

    // affichage de la grille du jeu sur l'interface
    setGrid(grid,possible);
}

// --------------------------------- ResetBruteForce : // restaurer les parametres du brute force à leur etat initial


void ResetBruteForce()
{
    BruteForceStop = false;
    while(!gridStack.empty()) gridStack.pop();
    while(!possibleStack.empty()) possibleStack.pop();
}

// --------------------------------- ResetPossible : vider la grille possible


void ResetPossible()
{
    for (int r=0; r<9; r++)
        for (int c=0; c<9; c++)
            possible.possible[r][c] = "";

}

// --------------------------------- isPuzzleSolved : tester si le puzzle est résoulue

bool IsPuzzleSolved()
{
    string pattern; // conient la chaine "123456789" et pour chaque ligne,colonne et bloc ,on supprime les valeurs existantes de pattern
    int r,c;

    //By Rows
    for (r = 0; r<9 ; r++) {
        pattern = "123456789";
        for (c = 0 ; c<9 ; c++) {
            if (pattern.find(to_string(grid.grid[r][c])) != string::npos)  // string::npos => Not found
                pattern.erase(pattern.find(to_string(grid.grid[r][c])),1);
        }
        if (pattern.length() > 0) return false;
    }

    //By Columns
    for (c = 0; c<9 ; c++) {
        pattern = "123456789";
        for (r = 0 ; r<9 ; r++) {
            if (pattern.find(to_string(grid.grid[r][c])) != string::npos)
                pattern.erase(pattern.find(to_string(grid.grid[r][c])),1);
        }
        if (pattern.length() > 0) return false;
    }


    //By MiniGrid
    for  (r = 0 ; r < 9 ; r+=3){
        pattern = "123456789";
        for  (c = 0 ; c < 9 ; c+=3){
            for (int rr = 0; rr<3 ; rr++) {
                for (int cc = 0; cc<3 ; cc++) {
                    if (pattern.find(to_string(grid.grid[r+rr][c+cc])) != string::npos)
                        pattern.erase(pattern.find(to_string(grid.grid[r+rr][c+cc])),1);
                }
            }
        }
        if (pattern.length() > 0) return false;
    }
    return true;
}

// --------------------------------- AfficherPossible

void afficherPossible(){
    cout << endl << "Possible:" << endl;
    for (int r=0; r<9; r++) {
        if (r%3 == 0) {
            cout<<"  ";
            for (int i=0; i<107; i++) cout << "-";
            cout<<endl;
        }
        for (int c=0; c<9; c++) {
            if (c%3 == 0) cout <<" | ";
            cout << setw(9) << possible.possible[r][c];
            if (c%3 == 0 || c%3 == 1) cout <<" . ";
        }
        cout<<" |"<<endl;
    }
    cout<<"  ";
    for (int i=0; i<107; i++) cout << "-";
    cout<<endl;
}

// --------------------------------- AfficherGrid


void afficherGrid()
{
    //    cout << endl << "Grid:" << endl;
    for (int r=0; r<9; r++) {
        if (r%3 == 0) {
            cout<<" ";
            for (int i=0; i<23; i++) cout << "-";
            cout<<endl;
        }

        for (int c=0; c<9; c++) {
            if (c%3 == 0) cout <<"| ";
            if (grid.grid[r][c]) cout<<grid.grid[r][c]<< " ";
            else cout<<"  ";
        }
        cout<<"|"<<endl;
    }
    cout<<" ";
    for (int i=0; i<23; i++) cout << "-";
    cout<<endl;
}

// --------------------------------- CalculatePossibleValues : calculer les valeurs possible des cases vides


string CalculatePossibleValues(int row, int col)
{
    int startR, startC;
    string str, x;
    if (possible.possible[row][col].length() == 0) str = "123456789";
    else str = possible.possible[row][col];

    //par Colonne
    for (int r=0; r<9 ; r++){
        if (grid.grid[r][col] != 0) {
            x = to_string(grid.grid[r][col]);
            if (str.find(x) != string::npos)
                str.erase(str.find(x),1);
        }
    }

    //par Ligne
    for (int c=0; c<9 ; c++){
        if (grid.grid[row][c] != 0) {
            x = to_string(grid.grid[row][c]);
            if (str.find(x) != string::npos)
                str.erase(str.find(x),1);
        }
    }

    //Par Bloc
    startR = row - (row % 3);
    startC = col - (col % 3);
    for (int r = startR; r<startR + 3; r++) {
        for (int c = startC; c<startC + 3; c++) {
            if (grid.grid[r][c] != 0) {
                x = to_string(grid.grid[r][c]);
                if (str.find(x) != string::npos)
                    str.erase(str.find(x),1);
            }
        }
    }
    return str;
}


// --------------------------------- Column, Row and Minigrid Elimination
/////////////////////////////////////////////////////////////////////////////////
//  Cette technique de résolution est logique et basique est repose sur la confirmation des
//  cases qui ont un seul candidat.
//  le score de cette technique est 1
/////////////////////////////////////////////////////////////////////////////////


bool CRME()
{
    bool changes = false;
    Logs << "\n----------------------------- CRME -----------------------------" << endl;
    for (int row=0; row<9; row++){
        for (int col=0; col<9; col++){
            if (grid.grid[row][col] == 0){
                possible.possible[row][col] = CalculatePossibleValues(row, col);

                if (possible.possible[row][col].length() == 1){
                    //---that means a number is confirmed--
                    SetCell(row, col, charToInt(possible.possible[row][col][0]));
                    // accumuler le score
                    totalScore += 1;
                    // cette fonction a effectué un changement
                    changes = true;
                }

                // pas de candidats, pas de solution ,lancer une exception
                else if (possible.possible[row][col].length() == 0)
                {

                    Logs << "Exception !!"<< endl;
                    Logs << row << " " << col << endl;
                    cout << "Exception CRME !" << endl;
                    throw(0);

                    // Throw exception

                }
            }
        }
    }
    return changes;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//  La technique suivante est le Lone Ranger, une technique logique et intermediaire,
//  consiste à la recherche d'un candidat parmi plusieurs candidats d'une
//  case et qui apparait un seule fois dans la ligne, colonne ou bloc.
//  il aide la CRME si celle ci se bloque.
//  son score est 2
//
////////////////////////////////////////////////////////////////////////////////////////

// --------------------------------- LookForLoneRangersInMinigrids


bool LookForLoneRangersInMinigrids()
{
    bool changes=false;
    bool NextMiniGrid;   // passer ou non au prochain bloc ?
    int occurrence,rPos = 0,cPos = 0;

    Logs << "\n-------------------- Lone Rangers Minigrids --------------------" << endl;

    //--check for each number from 1 to 9
    for(int n=1; n<=9; n++)
    {
        //---check the 9 minigrid
        for(int r=0; r<9; r+=3)
        {
            for(int c=0; c<9; c+=3)
            {
                NextMiniGrid=false;

                //---check within the minigrid
                occurrence=0;
                for (int rr=0; rr<=2; rr++)
                {
                    for (int cc=0; cc<=2; cc++)
                    {

                        if (grid.grid[r+rr][c+cc] == 0 && possible.possible[r+rr][c+cc].find(to_string(n)) != string::npos)
                        {
                            occurrence++;
                            cPos = c+cc;
                            rPos = r+rr;

                            // occurrence plus que 1 ==> n'est pas un Lone Ranger
                            if(occurrence > 1)
                            {
                                NextMiniGrid = true;
                                break;
                            }
                        }
                    }
                    // goto to the next Minigrid
                    if (NextMiniGrid) break;
                }

                if (!NextMiniGrid && occurrence == 1)
                {
                    //---that means number is confirmed
                    SetCell(rPos, cPos, n);
                    changes = true;
                    totalScore += 2;
                }
            }
        }
    }
    return changes;
}


// --------------------------------- LookForLoneRangersInRows

bool LookForLoneRangersInRows()
{
    bool changes=false;
    int occurrence,cPos = 0,rPos = 0;
    Logs << "\n----------------------- Lone Rangers Rows ----------------------" << endl;

    for(int r=0; r<9; r++)
    {
        for(int n=1; n<=9; n++)
        {
            occurrence=0;
            for(int c=0; c<9; c++)
            {
                if (grid.grid[r][c]==0 && possible.possible[r][c].find(to_string(n)) != string::npos)
                {
                    occurrence++;
                    //--if multiple occurrences, not a lone ranger anymore
                    if(occurrence>1) break;
                    cPos=c;
                    rPos=r;
                }
            }

            if (occurrence==1)
            {
                //---number is confirmed---
                SetCell(rPos,cPos,n);
                totalScore += 2;
                changes = true;
            }
        }
    }

    return changes;
}

// --------------------------------- LookForLoneRangersInColumns


bool LookForLoneRangersInColumns()
{
    bool changes=false;
    int occurrence,cPos=0,rPos=0;
    Logs << "\n--------------------- Lone Rangers Columns ---------------------" << endl;
    //'---check by column----

    for(int c=0; c<9; c++)
    {
        for(int n=1; n<=9; n++)
        {
            occurrence=0;
            for(int r=0; r<9; r++)
            {
                if (grid.grid[r][c]==0 && possible.possible[r][c].find(to_string(n)) != string::npos)
                {
                    occurrence++;
                    //--if multiple occurrences, not a lone ranger anymore
                    if(occurrence>1) break;

                    cPos=c;
                    rPos=r;
                }
            }

            if (occurrence==1)
            {
                //---number is confirmed---
                SetCell(rPos,cPos,n);
                totalScore += 2;
                changes = true;
            }
        }
    }
    return changes;
}


/////////////////////////////////////////////////////////////////////////////////
//
//  La technique suivante est le Twins, une technique logique et avancée,
//  consiste à la recherche de 2 candidats parmi plusieurs candidats d'une
//  case et qui apparaissent 2 fois dans la ligne, colonne ou bloc.
//  il aide le Lone Ranger si celui ci se bloque.
//  son score est 3
//
/////////////////////////////////////////////////////////////////////////////////


// --------------------------------- LookForTwinsInMinigrids

bool LookForTwinsInMinigrids(){
    bool changes = false;
    Logs << "\n----------------------- Twins Minigrids ------------------------" << endl;
    int r, rr, rrr, c, cc, ccc, startR, startC;
    string original_possible;

    //---look for twins in each cell
    for (r=0; r<9; r++) {
        for (c=0; c<9; c++) {
            //---if two possible values, check for twins
            if (grid.grid[r][c] == 0 && possible.possible[r][c].length() == 2) {
                //---scan by the minigrid that the current cell is in
                startR = r - (r % 3);
                startC = c - (c % 3);
                for (rr = startR ; rr < startR + 3 ; rr++) {
                    for (cc = startC ; cc < startC + 3 ; cc++) {
                        //---for cells other than the pair of twins
                        if ((cc != c || rr != r) && (possible.possible[rr][cc] == possible.possible[r][c])) {

                            //---twins found---
                            Logs << "Found : [ " << r+1 << " , " << c+1 <<" ]"  << " = " << setw(3) << possible.possible[r][c]<< "  ||  [ " << rr+1 << " , " << cc+1 << " ]"  << " = " << setw(3) << possible.possible[rr][cc] << endl;

                            //---remove the twins from all the other possible values in the minigrid
                            for (rrr = startR; rrr < startR + 3; rrr++) {
                                for (ccc = startC; ccc < startC + 3; ccc++) {
                                    //---only check for empty cells
                                    if (grid.grid[rrr][ccc] == 0 && !( (rrr == r && ccc == c) || (rrr == rr && ccc == cc))) {

                                        //---save a copy of the original possible values (twins)
                                        original_possible = possible.possible[rrr][ccc];

                                        //---remove first twin number from possible values
                                        if (possible.possible[rrr][ccc].find(possible.possible[r][c][0]) != string::npos)
                                            possible.possible[rrr][ccc].erase(possible.possible[rrr][ccc].find(possible.possible[r][c][0]),1);

                                        //---remove second twin number from possible values
                                        if (possible.possible[rrr][ccc].find(possible.possible[r][c][1]) != string::npos)
                                            possible.possible[rrr][ccc].erase(possible.possible[rrr][ccc].find(possible.possible[r][c][1]),1);

//                                       ---if the possible values are  modified, then set the changes
//                                         variable to True to indicate that the possible values of cells in the
//                                         minigrid have been modified
                                        if (original_possible != possible.possible[rrr][ccc]) {
                                            changes = true;
                                        }

//                                        ---if left with 1 possible value for  the current cell, cell is confirmed
                                        if (possible.possible[rrr][ccc].length()==1) {
                                            SetCell(rrr, ccc, charToInt(possible.possible[rrr][ccc][0]));
                                            totalScore += 3;
                                        }
                                        //if possible value reduces to empty string, then the user has placed a
                                        // move that results in the puzzle being not solvable
                                        else if (possible.possible[rrr][ccc].length() == 0){
                                            Logs << "Exeption !!"<< endl;
                                            Logs << rrr << " " << ccc << endl;
                                            cout << "Exeption TwinsInMinigrids !" << endl;
                                            throw(0);
                                            // Throw exception
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return changes;
}

// --------------------------------- LookForTwinsInRows

bool LookForTwinsInRows(){
    bool changes = false;
    int r, c, cc, ccc;
    string original_possible;
    Logs << "\n-------------------------- Twins Rows --------------------------" << endl;

    for (r=0; r<9; r++) {
        for (c=0; c<9; c++) {
            if (grid.grid[r][c] == 0 && possible.possible[r][c].length() == 2) {
                for (cc = 0 ; cc < 9 ; cc++) {
                    if (cc != c  && (possible.possible[r][cc] == possible.possible[r][c]) ) {
                        Logs << "Found : [ " << r+1 << " , " << c+1 <<" ]"  << " = " << setw(3) << possible.possible[r][c]<< "  ||  [ " << r+1 << " , " << cc +1<< " ]"  << " = " << setw(3) << possible.possible[r][cc] << endl;
                        for (ccc = 0; ccc < 9 ; ccc++) {
                            if (grid.grid[r][ccc] == 0 && ccc != c && ccc != cc) {
                                original_possible = possible.possible[r][ccc];

                                if (possible.possible[r][ccc].find(possible.possible[r][c][0]) != string::npos)
                                    possible.possible[r][ccc].erase(possible.possible[r][ccc].find(possible.possible[r][c][0]),1);

                                if (possible.possible[r][ccc].find(possible.possible[r][c][1]) != string::npos)
                                    possible.possible[r][ccc].erase(possible.possible[r][ccc].find(possible.possible[r][c][1]),1);


                                if (original_possible != possible.possible[r][ccc]) {
                                    changes = true;
                                }

                                if (possible.possible[r][ccc].length()==1) {
                                    SetCell(r, ccc, charToInt(possible.possible[r][ccc][0]));
                                    totalScore += 3;
                                }

                                else if (possible.possible[r][ccc].length()==0) {
                                    Logs << "Exeption !!"<< endl;
                                    Logs << r << " " << ccc << endl;
                                    cout << "Exeption TwinsInRows !" << endl;
                                    throw(0);
                                    // Throw exception
                                }
                            }
                        }
                    }

                }
            }
        }
    }
    return changes;

}

// --------------------------------- LookForTwinsInColumns


bool LookForTwinsInColumns(){
    int r, c, rr, rrr;
    string original_possible;
    bool changes = false;
    Logs << "\n------------------------- Twins Columns ------------------------" << endl;
    for (r=0; r<9; r++) {
        for (c=0; c<9; c++) {
            if (grid.grid[r][c] == 0 && possible.possible[r][c].length() == 2) {
                for (rr = 0 ; rr < 9 ; rr++) {
                    if (rr != r  && (possible.possible[rr][c] == possible.possible[r][c] )) {
                        Logs << "Found : [ " << r+1 << " , " << c +1<<" ]"  << " = " << setw(3) << possible.possible[r][c]<< "  ||  [ " << rr +1<< " , " << c+1 << " ]"  << " = " << setw(3) << possible.possible[rr][c] << endl;
                        for (rrr = 0; rrr < 9 ; rrr++) {
                            if (grid.grid[rrr][c] == 0 && rrr != r && rrr != rr) {
                                original_possible = possible.possible[rrr][c];

                                if (possible.possible[rrr][c].find(possible.possible[r][c][0]) != string::npos)
                                    possible.possible[rrr][c].erase(possible.possible[rrr][c].find(possible.possible[r][c][0]),1);

                                if (possible.possible[rrr][c].find(possible.possible[r][c][1]) != string::npos)
                                    possible.possible[rrr][c].erase(possible.possible[rrr][c].find(possible.possible[r][c][1]),1);


                                if (original_possible != possible.possible[rrr][c]) {
                                    changes = true;
                                }

                                if (possible.possible[rrr][c].length()==1) {
                                    SetCell(rrr, c, charToInt(possible.possible[rrr][c][0]));
                                    totalScore += 3;
                                }

                                else if (possible.possible[rrr][c].length()==0) {
                                    Logs << "Exeption !!"<< endl;
                                    Logs << rrr << " " << c << endl;
                                    cout << "Exeption TwinsInColumns !" << endl;
                                    throw(0);
                                    // Throw exception
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return changes;
}


/////////////////////////////////////////////////////////////////////////////////
//
//  La technique suivante est le Triplets, une technique logique et avancée,
//  similaire à celle de Twins, mais celle ci consiste à la recherche de
//  3 candidats parmi plusieurs candidats d'une
//  case et qui apparaissent 3 fois dans la ligne, colonne ou bloc.
//  il aide la technique de Twins si celle ci se bloque.
//  son score est 4
//
/////////////////////////////////////////////////////////////////////////////////


// --------------------------------- LookForTripletsInMinigrids


bool LookForTripletsInMinigrids(){
    int r, rr, rrr, c, cc, ccc, startR, startC;
    string original_possible, TripletsLocation;
    bool changes = false;
    Logs << "\n----------------------- Triplets Minigrids ---------------------" << endl << endl;

    //---check each cell
    for (r=0; r<9; r++) {
        for (c=0; c<9; c++) {
            TripletsLocation = "";
//            --- three possible values; check for triplets
            if (grid.grid[r][c] == 0 && possible.possible[r][c].length() == 3) {
//              ---first potential triplet found
                TripletsLocation += to_string(r) + to_string(c);

//                ---scan by minigrid
                startR = r - (r % 3);
                startC = c - (c % 3);
                for (rr = startR ; rr < startR + 3 ; rr++) {
                    for (cc = startC ; cc < startC + 3 ; cc++) {

                        if ((cc != c || rr != r) && (possible.possible[rr][cc] == possible.possible[r][c] || (possible.possible[rr][cc].length() == 2 && possible.possible[r][c].find(possible.possible[rr][cc][0]) != string::npos && possible.possible[r][c].find(possible.possible[rr][cc][1]) != string::npos) )) {
                            //---save the coordinates of the triplets
                            TripletsLocation += to_string(rr) + to_string(cc);
                        }

                    }
                }

//                --found 3 cells as triplets; remove all from the other cells

                if (TripletsLocation.length()==6){

//                    --triplets found
                    int x1,x2,x3,y1,y2,y3;
                    x1 = charToInt(TripletsLocation[0]);   y1 = charToInt(TripletsLocation[1]); // coordonnes du 1er Triplet
                    x2 = charToInt(TripletsLocation[2]);   y2 = charToInt(TripletsLocation[3]); // coordonnes du 2ieme Triplet
                    x3 = charToInt(TripletsLocation[4]);   y3 = charToInt(TripletsLocation[5]); // coordonnes du 3ieme Triplet

                    Logs << "Found : [ " << x1+1 << " , " << y1+1 <<" ]"  << " = " << setw(3) << possible.possible[x1][y1]<< "  ||  [ " << x2+1 << " , " << y2+1 << " ]"  << " = " << setw(3) << possible.possible[x2][y2]<< "  ||  [ " << x3+1 << " , " << y3+1 << " ]" << " = " << setw(3) << possible.possible[x3][y3] << endl;
                    //---remove each cell's possible values containing the triplet
                    for (rrr = startR; rrr < startR + 3; rrr++) {
                        for (ccc = startC; ccc < startC + 3; ccc++) {
//                            ---look for the cell that is not part of the 3 cells found
                            if (grid.grid[rrr][ccc] == 0 && !( (rrr == x1 && ccc == y1) || (rrr == x2 && ccc == y2) || (rrr == x3 && ccc== y3))) {
                                //---save the original possible values
                                original_possible = possible.possible[rrr][ccc];

                                //---remove first triplet number from possible values
                                if (possible.possible[rrr][ccc].find(possible.possible[r][c][0]) != string::npos)
                                    possible.possible[rrr][ccc].erase(possible.possible[rrr][ccc].find(possible.possible[r][c][0]),1);

//                                ---remove second triplet number from possible values
                                if (possible.possible[rrr][ccc].find(possible.possible[r][c][1]) != string::npos)
                                    possible.possible[rrr][ccc].erase(possible.possible[rrr][ccc].find(possible.possible[r][c][1]),1);
//                                ---remove third triplet number from possible values
                                if (possible.possible[rrr][ccc].find(possible.possible[r][c][2]) != string::npos){
                                    possible.possible[rrr][ccc].erase(possible.possible[rrr][ccc].find(possible.possible[r][c][2]),1);
                                }

//                                ---if the possible values are modified, then set the changes variable to True to indicate
//                                 that the possible values of cells in the minigrid have been modified

                                if (original_possible != possible.possible[rrr][ccc]) {
                                    changes = true;
                                }

//                                ---if left with 1 possible value for the current cell, cell is confirmed
                                if (possible.possible[rrr][ccc].length()==1) {
                                    SetCell(rrr, ccc, charToInt(possible.possible[rrr][ccc][0]));
                                    totalScore += 4;
                                }

//                                ---if possible value reduces to empty string,then the user has placed a move that results
//                                 in the puzzle being not solvable
                                else if (possible.possible[rrr][ccc].length() == 0){
                                    Logs << "Exeption !!"<< endl;
                                    Logs << rrr << " " << ccc << endl;
                                    cout << "Exeption TripletsInMinigrids !" << endl;
                                    throw(0);
                                    // Throw exception
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return changes;
}



// --------------------------------- LookForTripletsInRows


bool LookForTripletsInRows(){
    int r, c, cc, ccc;
    string original_possible, TripletsLocation;
    bool changes = false;
    Logs << "\n------------------------- Triplets Rows ------------------------" << endl;

    for (r=0; r<9; r++) {
        for (c=0; c<9; c++) {
            TripletsLocation = "";
            if (grid.grid[r][c] == 0 && possible.possible[r][c].length() == 3) {
                TripletsLocation += to_string(c);
                for (cc = 0 ; cc < 9 ; cc++) {

                    if (cc != c  && (possible.possible[r][cc] == possible.possible[r][c] || (possible.possible[r][cc].length() == 2 && possible.possible[r][c].find(possible.possible[r][cc][0]) != string::npos && possible.possible[r][c].find(possible.possible[r][cc][1]) != string::npos) )) {

                        TripletsLocation += to_string(cc);
                    }

                }
            }

            if (TripletsLocation.length()==3){
                int y1,y2,y3;
                y1 = charToInt(TripletsLocation[0]);
                y2 = charToInt(TripletsLocation[1]);
                y3 = charToInt(TripletsLocation[2]);

                Logs << "Found : [ " << r+1 << " , " << y1+1 <<" ]"  << " = " << setw(3) << possible.possible[r][y1]<< "  ||  [ " << r+1 << " , " << y2+1 << " ]"  << " = " << setw(3) << possible.possible[r][y2]<< "  ||  [ " << r+1 << " , " << y3+1 << " ]" << " = " << setw(3) << possible.possible[r][y3] << endl;

                for (ccc = 0; ccc < 9 ; ccc++) {
                    if (grid.grid[r][ccc] == 0 && ccc != charToInt(TripletsLocation[0]) && ccc != charToInt(TripletsLocation[1]) && ccc!= charToInt(TripletsLocation[2])) {
                        original_possible = possible.possible[r][ccc];

                        if (possible.possible[r][ccc].find(possible.possible[r][c][0]) != string::npos)
                            possible.possible[r][ccc].erase(possible.possible[r][ccc].find(possible.possible[r][c][0]),1);

                        if (possible.possible[r][ccc].find(possible.possible[r][c][1]) != string::npos)
                            possible.possible[r][ccc].erase(possible.possible[r][ccc].find(possible.possible[r][c][1]),1);

                        if (possible.possible[r][ccc].find(possible.possible[r][c][2]) != string::npos){
                            possible.possible[r][ccc].erase(possible.possible[r][ccc].find(possible.possible[r][c][2]),1);
                        }

                        if (original_possible != possible.possible[r][ccc]) {
                            changes = true;
                        }

                        if (possible.possible[r][ccc].length()==1) {
                            SetCell(r, ccc, charToInt(possible.possible[r][ccc][0]));
                            totalScore += 4;
                        }

                        else if (possible.possible[r][ccc].length()==0) {
                            Logs << "Exeption !!"<< endl;
                            Logs << r << " " << ccc << endl;
                            cout << "Exeption TripletsInRows !" << endl;
                            throw(0);
                            // Throw exception
                        }
                    }
                }
            }
        }
    }
    return changes;
}

// --------------------------------- LookForTripletsInColumns



bool LookForTripletsInColumns(){
    int r, c, rr, rrr;
    string original_possible, TripletsLocation;
    bool changes = false;
    Logs << "\n----------------------- Triplets Columns -----------------------" << endl;

    for (r=0; r<9; r++) {
        for (c=0; c<9; c++) {
            TripletsLocation = "";
            if (grid.grid[r][c] == 0 && possible.possible[r][c].length() == 3) {
//                cout << "found first triplet " << r << " " << c << endl;
                TripletsLocation += to_string(r);
                for (rr = 0 ; rr < 9 ; rr++) {
                    if (rr != r  && (possible.possible[rr][c] == possible.possible[r][c] || (possible.possible[rr][c].length() == 2 && possible.possible[r][c].find(possible.possible[rr][c][0]) != string::npos && possible.possible[r][c].find(possible.possible[rr][c][1]) != string::npos) )) {
                        TripletsLocation += to_string(rr);
                    }

                }
            }

            if (TripletsLocation.length()==3){
//                cout << "found triplet " << endl;

                int x1,x2,x3;
                x1 = charToInt(TripletsLocation[0]);
                x2 = charToInt(TripletsLocation[1]);
                x3 = charToInt(TripletsLocation[2]);

                Logs << "Found : [ " << x1+1 << " , " << c+1 <<" ]"  << " = " << setw(3) << possible.possible[x1][c]<< "  ||  [ " << x2+1 << " , " << c+1 << " ]"  << " = " << setw(3) << possible.possible[x2][c]<< "  ||  [ " << x3+1 << " , " << c +1<< " ]" << " = " << setw(3) << possible.possible[x3][c] << endl;

                for (rrr = 0; rrr < 9 ; rrr++) {
                    if (grid.grid[rrr][c] == 0 && rrr != charToInt(TripletsLocation[0]) && rrr != charToInt(TripletsLocation[1]) && rrr != charToInt(TripletsLocation[2])) {
                        original_possible = possible.possible[rrr][c];

                        if (possible.possible[rrr][c].find(possible.possible[r][c][0]) != string::npos)
                            possible.possible[rrr][c].erase(possible.possible[rrr][c].find(possible.possible[r][c][0]),1);

                        if (possible.possible[rrr][c].find(possible.possible[r][c][1]) != string::npos)
                            possible.possible[rrr][c].erase(possible.possible[rrr][c].find(possible.possible[r][c][1]),1);

                        if (possible.possible[rrr][c].find(possible.possible[r][c][2]) != string::npos){
                            possible.possible[rrr][c].erase(possible.possible[rrr][c].find(possible.possible[r][c][2]),1);
                        }

                        if (original_possible != possible.possible[rrr][c]) {
                            changes = true;
                        }
                        if (possible.possible[rrr][c].length()==1) {
                            SetCell(rrr, c, charToInt(possible.possible[rrr][c][0]));
                            totalScore += 4;
                        }

                        else if (possible.possible[rrr][c].length()==0) {
                            Logs << "Exception !!"<< endl;
                            Logs << rrr << " " << c << endl;
                            cout << "Exception TripletsInColumns !" << endl;
                            throw(0);
                            // Throw exception
                        }

                    }
                }
            }

        }
    }
    return changes;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//
                          //////////////////
//                        //              //
//                        //    NOTE      //
//                        //              //
                          //////////////////

//  Toutes Les techniques precedentes sont des techniques Logique qui résouent
//  une grille sans aucune supposition ou randomisation .
//
//  Le grille qui est résolue avec une ou plusieurs de ces techniques
//  est une grille qui a UNE UNIQUE SOLUTION.
//
////////////////////////////////////////////////////////////////////////////////////////


// --------------------------------- SolvePuzzle : résoudre le puzzle

bool SolvePuzzle()
{
    bool changes ;
    bool ExitLoop = false ;
    do //---Look for Triplets in Columns
    {
        do //---Look for Triplets in Rows
        {
            do //---Look for Triplets in Minigrids
            {
                do //---Look for Twins in Columns
                {
                    do //---Look for Twins in Rows
                    {
                        do //---Look for Twins in Minigrids
                        {
                            do //---Look for Lone Ranger in Columns
                            {
                                do //---Look for Lone Ranger in Rows
                                {
                                    do //---Look for Lone Ranger in' Minigrids
                                    {
                                        do //---Perform Col/Row and  Minigrid Elimination
                                        {
                                            changes = CRME();
                                            if (IsPuzzleSolved() )
                                            {
                                                ExitLoop = true ;
                                                break;
                                            }

                                        }
                                        while (changes);

                                        if (ExitLoop) break;
                                        //---Look for Lone Ranger in Minigrids
                                        changes = LookForLoneRangersInMinigrids();
                                        if (IsPuzzleSolved() )
                                        {
                                            ExitLoop = true ;
                                            break ;
                                        }
                                    }
                                    while (changes);

                                    if (ExitLoop) break;
                                    //---Look for Lone Ranger in Rows
                                    changes = LookForLoneRangersInRows() ;
                                    if (IsPuzzleSolved() )
                                    {
                                        ExitLoop = true;
                                        break ;
                                    }
                                }
                                while (changes);

                                if (ExitLoop) break;
                                //---Look for Lone Ranger in Columns
                                changes = LookForLoneRangersInColumns();
                                if (IsPuzzleSolved() )
                                {
                                    ExitLoop = true;
                                    break ;
                                }
                            }
                            while (changes);

                            if (ExitLoop) break;
                            //---Look for Twins in Minigrids;
                            changes = LookForTwinsInMinigrids();
                            if (IsPuzzleSolved())
                            {
                                ExitLoop = true;
                                break ;
                            }
                        }
                        while (changes);

                        if (ExitLoop) break;
                        //---Look for Twins in Rows
                        changes = LookForTwinsInRows();
                        if (IsPuzzleSolved() )
                        {
                            ExitLoop = true;
                            break ;
                        }
                    }
                    while (changes);

                    if (ExitLoop) break;
                    //---Look for Twins in Columns
                    changes = LookForTwinsInColumns();
                    if (IsPuzzleSolved() )

                    {
                        ExitLoop = true;
                        break ;
                    }
                }
                while (changes);

                if (ExitLoop) break;
                //---Look for Triplets in Minigrids
                changes = LookForTripletsInMinigrids();
                if (IsPuzzleSolved() )
                {
                    ExitLoop = true;
                    break ;
                }
            }
            while (changes);

            if (ExitLoop) break;
            //---Look for Triplets in Rows
            changes = LookForTripletsInRows();
            if (IsPuzzleSolved() )
            {
                ExitLoop = true;
                break ;
            }
        }
        while (changes);

        if (ExitLoop) break;
        //---Look for Triplets in Columns
        changes = LookForTripletsInColumns();
        if (IsPuzzleSolved() )
        {
            ExitLoop = true ;
            break ;
        }
    }
    while (changes);

    if (IsPuzzleSolved())
        return true ;

    else
        return false ;

}


// --------------------------------- RandomizeThePossibleValues :
//                       elle permute de maniere aleatoire la chaine réçue en parametre


void RandomizeThePossibleValues(string &str)
{
    unsigned int i, j;
    char temp;
    int t = str.length();
    srand(time(NULL));
    for (i=0; i<str.length(); i++) {
        j = Randomize(0,t-1);
        //Swap Chars
        temp = str[j];
        str[j] = str[i];
        str[i] = temp;
    }
}


// --------------------------------- FindCellWithFewestPossibleValues
//                            trouve la cellule avec le moindre nombre de candidats

void FindCellWithFewestPossibleValues(int &row, int &col){

    unsigned long min = 100;
    for (int r=0; r<9; r++) {
        for (int c=0; c<9; c++) {
            if (grid.grid[r][c] == 0 && possible.possible[r][c].length() < min) {
                min = possible.possible[r][c].length();
                row = r;
                col = c;
            }
        }
    }
}

// --------------------------------- SolvePuzzleByBruteForce

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// cette fonction et récursive ,elle tente de résoudre un puzzle de Sudoku en sélectionnant
//  systématiquement une valeur possible d'une cellule , puis en appliquant tous les autres
// techniques pour résoudre le puzzle . elle appelle elle-même jusqu'à ce que le puzzle soit résolu
// , ou, si la sélection d'une valeur particulière pour une cellule provoque le puzzle soit insoluble
// , elle revient en arrière (backtrack) par restauration de l'état de la grille
//  en utilisant les deux objets Stack , ActualStackand PossibleStack .
//
//  Puisque cette technique fait recours à la randomisation , toute grille qui neccesite
//  l'intervetion de la Brute Force  est une Grille à plusieurs Solutions.
//
//  score de la brute force = 5
//  score du backtrack = 5
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


void SolvePuzzleByBruteForce()
{
    srand(time(NULL));
    int r,c,length;
    Logs << "\n///////////////////// SolvePuzzleByBruteForce \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << endl;

    totalScore+=5;
    //---find out which cell has the smallest number of possible values
    FindCellWithFewestPossibleValues(r,c);


    //---push the actual and possible stacks into the stack
    gridStack.push(grid);
    possibleStack.push(possible);

//    ---select one value and try
    length = possible.possible[r][c].length();

    for (int i=0; i<length; i++)
    {
        Logs << "Brute force SetCell" << endl;
        RandomizeThePossibleValues( possible.possible[r][c] );
        SetCell(r,c,charToInt(possible.possible[r][c][i]));

        try{
            if (SolvePuzzle())
            {
//                ---if the puzzle is solved, the recursion can stop now
                BruteForceStop = true;
                return;
            }
            else
            {
//                ---no problem with current selection, proceed with next cell
                SolvePuzzleByBruteForce();
                if (BruteForceStop) return;
            }
        }
        catch(int exception)
        {
            Logs << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Backtack or No Solution!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
            totalScore+=5;


            if (exception == 0)
            {
                //Backtrack
                if (!gridStack.empty())
                {
                    grid = gridStack.top();
                    gridStack.pop();
                }

                //else ==> No solution

                //Backtrack
                if (!possibleStack.empty())
                {
                    possible = possibleStack.top();
                    possibleStack.pop();
                }
                 //else ==> No solution

            }
        }
    }
}

// -------------------------------- SolveP : SLot to receive the Solve button's click and Solve the puzzle

void Interface::SolveP()
{
    ResetBruteForce();
    resetStacks();
    undob->hide();
    redob->hide();
    save->hide();
    Solve->hide();
    timer->stop();

    // try in case the brute force couldn't hand an exception
    try
    {
        //si la grille n'est pas résolue avec les techniques logiques passer au brute force
        if(!SolvePuzzle())
        {

           SolvePuzzleByBruteForce();
        }
    }
    //  si le brute force n'a pas pu traiter les exceptions lancées, donc pas de solution
    //   (causée par un mouvement erronée saisie par le joueur)
    catch (int exc)
    {
        if(exc == 0)
        {
      //      no solution
            openNoSolution();

         }
    }

    // there's a solution , l'afficher
    setGrid(grid,possible);
}

// -----------------------------------------ClearGrid : vider la grille
void Interface::clearGrid()
{
    grid_backup="000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    grid_backup_dug="000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    x=0;

    restart->hide();
    Solve->hide();
    undob->hide();
    redob->hide();
    save->hide();
    timer->stop();
    elaps->setText("");
    resetStacks();
    ResetPossible();
    lev->setText("");

    for(int i=0;i<81;i++)
    {

        disconnect(T[i],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));

        T[i]->clear();
        T[i]->setToolTip("");
        T[i]->setEnabled(false);
        T[i]->setStyleSheet(T[i]->color);

        connect(T[i],SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));
    }

    initialise(grid.grid,grille);

}



// --------------------------------- ConvertGridToStr : de matrice à un string


string ConvertGridToStr(int m[9][9]){
    string str="";
    for (int r=0; r<9; r++) {
        for (int c=0; c<9; c++) {
            str += to_string(m[r][c]);
        }
    }
    return str;
}

// --------------------------------- lowerBound


bool lowerBound(int r, int c, int lowerB){
    int counter=0;
    //Check row
    for (int i=0; i<9; i++) {
        if (grid.grid[i][c] != 0 && i!=r) counter++;
    }
    if (counter < lowerB) return true;

    counter = 0;

    //Check Column
    for (int i=0; i<9; i++) {
        if (grid.grid[r][i] != 0 && i!=c) counter++;
    }
    if (counter < lowerB) return true;

    return false;
}

// --------------------------------- Digg



bool Digg(int r, int c,int lowerB){
    if (CanBeDugg[r][c] && grid.grid[r][c]!=0 && !lowerBound(r,c,lowerB)) {
        if (!lowerBound(r,c,lowerB)) {
            grid.grid[r][c] = 0;
            return true;
        }
        else{
            CanBeDugg[r][c] = 0;
            return false;
        }
    }

    else return false;

}


// ------------------------------------Generate Easy
string GenerateEasy()
{

    string resultat="";   //contient la grille finale retournée
    int x,zero=0,compteur=0,nbr_case_A_creuser;
    int violate[9][9]={0}; //contient les case qui ne peuvent pas etre creusee afin de verifier le lower bound


   nbr_case_A_creuser=Randomize(40,46); //le nombre des case à creuser
    do{
        for(int i=0;i<9;i++){
           for(int j=0;j<9;j++){
                      compteur++;
                      if (grid.grid[i][j] && violate[i][j]==0){
                          if(compteur<200){         //au-dessous de 200 iteration on peut choisir aleatoirement si on creuse ou pas la case
                             x=Randomize(0,1);
                             if(!x){         //afin d'interdir le choix de non creusée pour 4 ou 7 itération successif
                                zero++;
                                if (zero> Randomize(4,7)){ zero=0;x=1;}
                             }
                           }
                     else       x=1;
                     if(x){
                     if(!lowerBound(i,j,4)){// verifier la condtion de lowerbound
                         grid.grid[i][j]=0;
                         nbr_case_A_creuser--;
                         }
                     else       violate[i][j]=1;
                     }
                  }
                  if(!nbr_case_A_creuser || compteur>300 ) break ; //arreter si on arrive a creuse le nbr determinée ou si en depasse 300 itération
                  }
              if(!nbr_case_A_creuser || compteur>300) break ;
              }
          }while(nbr_case_A_creuser && compteur<300 );


   resultat=ConvertGridToStr(grid.grid);
   return resultat;

}


// ----------------------------------------Easy
string easy()
{

    string resultat="";
    bool generer=false;

    do{
         ResetPossible();   // afin d'initialiser les structures utilisée par les fonction de resolutions
         initialise(grid.grid,grid_backup); //transformer la chaine grid.backup a une grille de type int "grid.grid"
         resultat=GenerateEasy();//la fonction retourn une grille de niveau eazy
         initialise(grid.grid,resultat);

         totalScore = 0;
         if( SolvePuzzle() && totalScore<=48 && IsPuzzleSolved() )//verifier si la grille retournée respecte les intervales de score
                                                                //et si il a une seule solution
              generer=true;


      }while(!generer); // repeter jusqu'a trouvée une grille qui respect les condtions posee

        grid_backup_dug=resultat;
        return resultat;
}


// --------------------------------- medium

string GenerateMedium()
{
        // ToBeDugg: nombre de cellule à creuser
        // aller: contient la valeur vrais si on parcours de la gauche vers la droite
        // et faix si l'inverse
        // result: vas contenir la grille genere sous forme de string
        int ToBeDugg, limit, dugg, x;
        bool aller;
        const int lowerB = 3;
        string resultat;

        // reinitialisation de la matrice CanBeDugg par des 1 par tous
        for (int i =0; i<9; i++)
        {
            for (int j=0; j<9; j++)
            {
                CanBeDugg[i][j] = 1;
            }
        }

        limit = 0;
        dugg = 0;

        // on choisit un nombre de cellule à creuser
        ToBeDugg = Randomize(46,49);

        // on démarre par creuser une cellule aléatoire de la première ligne
        grid.grid[0][Randomize(0,8)]  =  0;
        int j = 0;

        // Boucle principale
        do{

            aller = true;
            for (int r=0; r<9; r++)
            {
                // parcours de la gauche vers la droite
                if (aller) {
                    for (int c=j; c<9; c+=2)
                    {
                        // on prend une valeur aleatoire entre 1 et 10
                        x = Randomize(1,10);
                        // si on n'a pas depasse la limite des cellules parcourus mais pas creuser
                        if (limit < 15)
                        {
                            // si la valeur x choisit est inferieur a 5 donc on vas creuser la cellule
                            if(x<5)
                            {
                                // on varifie si on peut la creuser (critère Lower Bound)
                                // si le critere est verrifie on creuse et on incremente le nombre
                                // de cellules creuser
                                if (Digg(r,c,lowerB))
                                {
                                    limit = 0;
                                    dugg++;
                                }

                                // si le nombre de cellule creuser = nombre de cellule à creuser on sort
                                if (dugg==ToBeDugg) goto fin;

                            }
                            // sinon on vas pas la creuser et on increment le compteur limit
                            else limit++ ;
                        }

                        // on a depasser la limite des cellules parcourus mais pas creuser
                        // donc on vas creuser la cellule meme si x n'est pas inferieur a 5
                        // et que le critere Lower Bound n'est pas verrifie
                        else{
                            if (grid.grid[r][c] != 0)
                            {
                                grid.grid[r][c] = 0;
                                dugg++;
                                limit = 0;
                                if (dugg==ToBeDugg) goto fin;
                            }
                        }
                    }
                    aller = false;
                    continue;
                }

                // Le meme principe ici mais dans le sens inverse (parcours de la droite vers la gauche)
                else {
                    for (int c=7+j; c>0; c-=2)
                    {
                        x = Randomize(1,10);
                        if (limit < 15)
                        {
                            if(x<5){
                                if (Digg(r,c,lowerB))
                                {
                                    limit = 0;
                                    dugg++;
                                }

                                if (dugg==ToBeDugg) goto fin;

                            }
                            else limit++ ;
                        }
                        else
                        {
                            if (grid.grid[r][c] != 0)
                            {
                                grid.grid[r][c] = 0;
                                dugg++;
                                limit = 0;
                                if (dugg==ToBeDugg) goto fin;
                            }
                        }
                    }
                    aller = true;
                    continue;
                }
            }

            if (j==0) j=1;
            else j=0;
        }while(dugg<ToBeDugg);

    fin:
        // on transforme la matrice grid.grid on string
        resultat = ConvertGridToStr(grid.grid);

       return resultat;
}

// ----------------------------------------------------------Medium
string medium()
{
    // unique: vrai si la grille a une solution unique, faux sinon
    string result;
    bool unique;


    do{
        // reinitialiser la matrice possible
        ResetPossible();
        // initialiser la matrice grid.grid par grid_backup
        initialise(grid.grid, grid_backup);
        // genere une grille medium
        result = GenerateMedium();

        // resoudre la grille
        initialise(grid.grid, result);
        totalScore = 0;
        SolvePuzzle();

        // si la grille a une solution unique cad qu'on a pu la resoudre on utilisant
        // seulement les techniques logiques alors unique <- vrai
        if (IsPuzzleSolved()) unique = true;

        else unique = false;

        // on doiz egalement verrifie le score de grille pour juge sa difficulte
    } while(!unique || !(totalScore>=49 && totalScore<=59));

    grid_backup_dug=result;
    return result;
}


// -------------------------------GenerateHard

string GenerateHard()
{

    int givens[100];
    int x,nbr_case,size=0,r,c,permuter,s,compteur=0,borne;
    string res="";


    //copie les indices des givens de grid.grid  dans la table givens
        for(int i=0;i<9;i++){
              for(int j=0;j<9;j++)
                       if(grid.grid[i][j]!=0){
                           givens[size]=i*10+j;
                           size++;
                       }
               }

        borne=Randomize(53,56);
        nbr_case=81-size;
        s=size;

        //melanger les cases du tableau 'givens'
        for(int i=0;i<s;i++){
                        x=Randomize(0,size-1);
                        permuter=givens[i];
                        givens[i]=givens[x];
                        givens[x]=permuter;
                        }

        do{
           for(int i=0;i<s;i++){

                          r=givens[i]/10;
                          c=givens[i]%10;
                          res=ConvertGridToStr(grid.grid);

                          if(!lowerBound(r,c,2)){
                                grid.grid[r][c]=0;
                                 //s'assurer que la grille generer a une seule solution
                                if(SolvePuzzle()){
                                           nbr_case--;
                                           initialise(grid.grid,res);
                                           grid.grid[r][c]=0;
                                           }
                                 else{
                                          initialise(grid.grid,res); //incremonter compteur si le creusage de telle case
                                                                      // genere une grille avec plusieur solution
                                          compteur++;
                                         }
                                          permuter=givens[i];
                                          givens[i]=givens[s-1];
                                          givens[s-1]=permuter;
                                          s--;
                                          ResetPossible();
                                 }
                         if(nbr_case>=borne || compteur>6) break;
                         }
                        s=size;

    }while(nbr_case<borne && compteur<6 );


       res=ConvertGridToStr(grid.grid);
       return res;
}

// ----------------------------------------Hard
string hard()
{

    string resultat="";
    bool generer=false;
    string gr;
    do{
         ResetPossible();
         gr=easy();  //A partir d'une grille easy on va generer une grille Hard
         initialise(grid.grid,gr);

         resultat=GenerateHard();
         initialise(grid.grid,resultat);

         totalScore = 0;
         if(SolvePuzzle() && totalScore>=60 && totalScore<=80)
         generer=true;


      }while(!generer);

      grid_backup_dug=resultat;
      return resultat;
}


// ------------------------------------------ GenerateEvil


string GenerateEvil()
{
    int i,j,k2,k3,compt=0;
    int DuggCells = Randomize(54,59);
     string res;
do{
  for(i=0;i<9;i++){
       if (i%2==0){
                   k2=rand() % 2 +1;
                  for (j=0;j<9;j+=k2){

                    if (grid.grid[i][j]!=0 && compt< DuggCells)

                      {grid.grid[i][j]=0;
                                 compt++;  }

                            }
                   }

      else {

                  k3=rand() % 2 +1;
                   for(int j=7;j>=0;j-=k3 ){

                      if (grid.grid[i][j]!=0 && compt< DuggCells){
                           grid.grid[i][j]=0;
                              compt++;  }
                   }
              }

      }

} while(compt< DuggCells);

     res=ConvertGridToStr(grid.grid);
  return res ;
}

// ------------------------------------------ Evil


string Evil()
{
   string result;
do{

        ResetPossible();
        initialise(grid.grid, grid_backup);
        result = GenerateEvil();
        initialise(grid.grid, result);
        totalScore = 0;
        if(!SolvePuzzle())
            SolvePuzzleByBruteForce();

    } while(!(totalScore>=110 && totalScore<=120));

    grid_backup_dug=result;
    return result;
}


// ------------------------------------------ResetGrid : la grille est restaurer à son etat complet

void resetGrid()
{
    initialise(grid.grid,grid_backup);
}

// ---------------------------------------------- SaveGrid : sauvegarder la grille creusée originalle et celle jouée et le niveau

void Interface::saveGrid()
{
    file.open("saved Grid.txt",ios::out);
    if(file)

    {
        file<<grid_backup_dug<<endl<<ConvertGridToStr(grid.grid)<<endl<<x;
    }

    file<<endl;

    file.close();
}

// ------------------------------------------------Restart : recommencer une partie
void Interface::Restart()
{
    //afficher ces boutons
    undob->show();
    redob->show();
    save->show();
    Solve->show();

    //arreter le chrono , le remettre à 0 ,et relancer
    disconnect(timer,SIGNAL(timeout()),this,SLOT(settime()));
    elaps->setText("Elapsed Time:  00:00");
    sec=0;
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(settime()));

    // remplir la grille par celle creusée precedemment sauvegradée dans la variable grid_backup_dug
    initialise(grid.grid,grid_backup_dug);

    // afficher la grille su l'interface
    set();
}

// ------------------------------------------------getGrid to continue : continuer une grille sauvegardée
void Interface::getGrid()
{
    x=0;
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

    ResetPossible();
    resetStacks();

    lev->setText("");

    //extraire les données du fichier
    file.open("saved Grid.txt",ios::in);
    string original,current;
         file>>original;
         file>>current;
         file>>x;

    file.close();

    // affecter à chaque variable la valeur extraite equivalente
    grid_backup_dug=original;


    // definir les cases qui acceptent la saisie
     for(int i=0;i<81;i++)
        {
            T[i]->setEnabled(false);
            if(charToInt(original[i]) == 0)
            {
                 T[i]->setEnabled(true);
            }
        }

    // résoudre la grille creusée originale extraite
    initialise(grid.grid,grid_backup_dug);
    if(!SolvePuzzle())
        SolvePuzzleByBruteForce();

    // mettre la solution est dans la grille grid_backup
    grid_backup=ConvertGridToStr(grid.grid);

    // encore mettre la grille creusée originale extraite dans la grille du jeu pour calculer les candidats des cases vides
    initialise(grid.grid,grid_backup_dug);

    for (int i=0; i<9; i++)
        for (int j=0; j<9; j++)
            if (grid.grid[i][j] == 0)
                possible.possible[i][j]=CalculatePossibleValues(i,j);


    // maintenant mettre dans la grille du jeu celle qui a ete jouée et sauvegardée
    initialise(grid.grid,current);

    // regler et afficher la grille sur l'interface
    simpleSet(grid,possible);

}


// --------------------------------- UndoM : pour annuler un coup

bool UndoM(int &id, int &value)
{
    string s;
    int r, c, val;

    //si la pile des coups déjà effectués n'est pas vide , extraire les données qui sont stockées sous la forme string :ligne+colonne+valeur
    if (!Moves.empty())
    {
        s = Moves.top();
        Moves.pop();

        r = charToInt(s[0]);
        c = charToInt(s[1]);
        val = charToInt(s[2]);

        cout << "undoM : " << r << c << val << endl;

        id =  r * 9 + c;
        value = val;

        // empiler dans la pile des coups à refaire
        RedoMoves.push(to_string(r) + to_string(c) + to_string(grid.grid[r][c]));
        return true;
    }
    else
        return false;
}


// --------------------------------- RedoM : pour refaire un coup


bool RedoM(int &id, int &value)
{
    string s;
    int r, c, val;
    //si la pile des coup des coups annulés n'est pas vide , extraire les données qui sont stockées sous la forme string :ligne+colonne+valeur
    if (!RedoMoves.empty())
    {
        s = RedoMoves.top();
        RedoMoves.pop();

        r = charToInt(s[0]);
        c = charToInt(s[1]);
        val = charToInt(s[2]);

        cout << "RedoM : " << r << c << val << endl;
        id =  r * 9 + c;
        value = val;

        // empiler dans la pile des coups à annuler
        Moves.push(to_string(r) + to_string(c) + to_string(grid.grid[r][c]));
        return true;
    }
    return false;
}
