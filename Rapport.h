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
	bool estLocal; // Indique si le noeud est un fichier du serveur analysé ou non
	
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
  int src; // Id du noeud source
  int dest; // Id du noeud destination
  int nbTot; // Nombre de relations entre les mêmes noeuds
	
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

		/*
		 * Traite la ligne en ajoutant ses infos dans les conteneurs noeuds, consultations et relations
		 * Attention, l'objet n'est pas supprimé et doit donc être supprimé par l'appelant
		 * R : Vrai si l'ajout s'est bien déroulé et faux sinon
		 */
    bool ajouterLigne(const LigneLog *);
		/*
		 * Génère le rapport de "nomRapport" avec les données actuelles
		 * R : Vrai si le fichier est généré et Faux sinon
		 */
    bool genererRapport() const;
		/*
		 * Affiche les n documents les plus consultés selon l'état actuel de consultations (avec n passé en paramètre)
		 */
    void afficherTopDocs(const int) const;
		void afficherContraintes() const;
		/*
		 * Retourne vrai si le fichier de sortie existe et faux sinon
		 */
		bool dotFileExiste() const;
		/*
		 * Retourne faux si nomRapport est à sa valeur par défaut (vide) et vrai sinon
		 */
		bool fichierSortie() const;

  private:
    string nomRapport; // Nom du fichier de sortie
    int nbHitsMin; // Nombre minimal de consultations nécessaire pour que le noeud soit traité
    int creneauMin; // Créneau du noeud nécessaire pour qu'il soit traité 
    bool exclusionFichiers; // Vaut vrai si l'utilisateur veut exclure certaines extensions (voir Config.h)
    int dernierId; // Id du dernier noeud ajouté
    
    set<noeud> noeuds;
    map<int,int> consultations; // Nombre de consultations pour chaque noeud (indiqué par son id)
		set<relation> relations;
		
};

#endif /* RAPPORT_H_ */
