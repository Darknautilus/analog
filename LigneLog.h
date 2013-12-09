//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============


#ifndef LIGNELOG_H_
#define LIGNELOG_H_

#include <iostream>
#include <string>
#include "LecteurLigneFichier.h"
#include "Rapport.h"

using namespace std;

class LigneLog : LecteurFichier::LigneFichier
{
  public:
    LigneLog(string&);
    virtual ~LigneLog();

    bool estValide () const;
		friend std::ostream& operator <<(std::ostream&, const LigneLog&);

		friend class Rapport;

  private:
		bool valide;
    string referer;
    string cible;
    int creneau;

};

#endif /* LIGNELOG_H */
