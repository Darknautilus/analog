//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============


#ifndef RAPPORT_H_
#define RAPPORT_H_

#include <set>
#include <string>

#include "LigneLog.h"
#include "LecteurLigneFichier.h"

using namespace std;

class LigneLog;

struct noeud
{
	int id;
	string texte;
	bool estLocal;
	
	bool operator == (const noeud &n) const
	{
		return (this->id == n.id);
	}

	bool operator < (const noeud &n) const
	{
		if((estLocal && !(n.estLocal)) || (estLocal == n.estLocal && id < n.id))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	friend ostream & operator << (ostream &stream, const noeud &n)
	{
		stream << "[" << n.id << "," << n.texte << "," << n.estLocal << "]";
		return stream;
	}

};

struct relation
{
  int src;
  int dest;
  int nbTot;
	
	bool operator == (const relation &r) const
	{
		return (src == r.src && dest == r.dest);
	}

  bool operator < (const relation &r) const
  {
    if (src < r.src || (src == r.src && nbTot < r.nbTot))
    {
      return true;
    }
    else
    {
      return false;
    }
  }

	friend ostream & operator << (ostream & stream, const relation &r)
	{
		stream << "[" << r.src << " -> " << r.dest << "," << r.nbTot << "]";
		return stream;
	}

};


class Rapport
{
  public:
    Rapport(string& _nomRapport, int _nbHitsMin, int _creneauMin, bool _exclusionFichiers, int dernierId);
    virtual ~Rapport();

    bool ajouterLigne(const LigneLog *);
    bool genererRapport() const;
    void afficherTopDocs(const int) const;
		void afficherContraintes() const;
		bool dotFileExiste() const;
		bool fichierSortie() const;

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
