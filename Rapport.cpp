//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============


#include <list>
#include <string>
#include <algorithm>
#include "Rapport.h"
#include "Config.h"

using namespace std;

Rapport::Rapport(string& _nomRapport, int _nbHitsMin = 1, int _creneauMin = -1, bool _exclusionFichiers = false, int _dernierId=0) :
    nomRapport(_nomRapport),
    nbHitsMin(_nbHitsMin),
    creneauMin(_creneauMin),
    exclusionFichiers(_exclusionFichiers),
    dernierId(_dernierId)
{
}

Rapport::~Rapport()
{
	consultations.clear();
	relations.clear();
	noeuds.clear();
}

bool Rapport::ajouterLigne(const LigneLog *ligne)
{
	// Vérification paramètre temporel
	if (creneauMin >= 0 && ligne->creneau != creneauMin)
	{
		return false;
	}
	
	if(exclusionFichiers)
	{
		int dotPos = ligne->cible.rfind('.');
		string extension = ligne->cible.substr(dotPos,ligne->cible.length()-dotPos);
		for( list<string>::const_iterator it = TYPES_EXCLUS.begin() ; it != TYPES_EXCLUS.end() ; ++it )
		{
			if(it->compare(extension) == 0)
			{
				return false;
			}
		}
	}

	//Changements dans la structure noeuds
	noeud noeudLocalc;
	noeud noeudLocalr;
	noeudLocalc.texte = ligne->cible;
	noeudLocalc.estLocal = true;
	string strLocale = ligne->referer.substr(0,LOCALHOST.length());    
	if(strLocale.compare(LOCALHOST) == 0)
	{
		noeudLocalr.estLocal = true;
		noeudLocalr.texte = ligne->referer.substr(LOCALHOST.length(), (ligne->referer.length()-LOCALHOST.length()));
	}
	else
	{
		noeudLocalr.texte = ligne->referer;
	}

	bool cibleExiste = false;
	bool refererExiste = false;
	int cibleId;
	int refId;
	
	for (set<noeud>::iterator it = noeuds.begin(); it != noeuds.end() && (!cibleExiste || !refererExiste) ; ++it)
	{
		if (!cibleExiste && it->estLocal && it->texte.compare(noeudLocalc.texte) == 0)
		{
			cibleExiste = true;
			cibleId = it->id;
			(consultations.at(it->id))++;
		}
		if (!refererExiste && it->texte.compare(noeudLocalr.texte) == 0)
		{
			refererExiste = true;
			refId = it->id;
		}
	}

	if( !cibleExiste )
	{
		cibleId = ++dernierId;
		noeudLocalc.id = cibleId;
		noeuds.insert(noeudLocalc);
		consultations.insert(pair<int,int>(cibleId ,1));
	}
	if( !refererExiste )
	{
		refId = ++dernierId; 
		noeudLocalr.id = refId;
		noeuds.insert(noeudLocalr);
	}

	//Changements dans la structure relations
	relation relLoc;
	relLoc.dest = &noeudLocalc;
	relLoc.src = &noeudLocalr;
	relLoc.nbTot = 1;
	bool relEx = false;

	int oldTot;
	set<relation>::iterator it; 
	for (it = relations.begin() ; it != relations.end() && !relEx ; ++it)
	{
		if(*it == relLoc)
		{
			relEx = true;
			oldTot = it->nbTot;
		}
	}
	if(relEx)
	{
		it--; // Annule la pré-incrémentation dans le for precedent.
		relations.erase(it);
		it++; // Optimisation de l'insertion en pointant l'élément qui sera APRES relLoc dans le set.
		relLoc.nbTot = ++oldTot;
		relations.insert(it,relLoc);	
	}
	else
	{
		relations.insert(relLoc);
	}  

	return true;
}

bool Rapport::genererRapport() const
{
	for(set<noeud>::iterator it=noeuds.begin();it!=noeuds.end(); ++it)
	{
		cout<<(*it).texte<<" ("<<(*it).nbCons<<"hits)"<<endl;
	}
	

	return true;
}

void Rapport::afficherTopDocs() const
{
}
