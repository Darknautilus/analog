//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============


#ifndef RAPPORT_H_
#define RAPPORT_H_

#include <set>
#include <time.h>
#include <string>
#include <list>

#include "LigneLog.h"
#include "LecteurLigneFichier.h"

using namespace std;

class LigneLog;

struct noeud
{
	int id;
	string texte;
	bool estLocal;
	int nbCons;

	bool operator == (const noeud &n) const
	{
		return (this->id == n.id);
	}

	bool operator < (const noeud &n) const
	{
		if((estLocal && !(n.estLocal)) || (estLocal == n.estLocal && nbCons > n.nbCons))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

};

struct relation
{
  noeud *src;
  noeud *dest;
  int nbTot;
	
	bool operator == (const relation &r) const
	{
		return (*(this->src) == *(r.src) && *(this->dest) == *(r.dest));
	}

  bool operator < (const relation &r) const
  {
    if ((src->id < r.src->id) || (src->id == r.src->id && nbTot < r.nbTot))
    {
      return true;
    }
    else
    {
      return false;
    }
  }

};

const string LOCALHOST = "http://intranet-if.insa-lyon.fr";
const list<string> TYPES_EXCLUS {".jpg",".png",".gif",".bmp",".js",".css"};
const int NB_TOP_DOCS = 10;

class Rapport
{
  public:
    Rapport(string& _nomRapport, int _nbHitsMin, int _creneauMin, bool _exclusionFichiers, int dernierId);
    virtual ~Rapport();

    bool ajouterLigne(const LigneLog *ligne);
    bool genererRapport() const;
    void afficherTopDocs() const;

  private:
    string nomRapport;
    int nbHitsMin;
    int creneauMin;
    bool exclusionFichiers;
    int dernierId;
    
    set<noeud> noeuds;
    map<int,int> consultations;
		set<relation> relations;
		
};

#endif /* RAPPORT_H_ */
