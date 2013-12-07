//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============


#include <list>
#include <string>
#include <algorithm>
#include "Rapport.h"
#include "Config.h"
#include <iostream>
#include <fstream>

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

bool Rapport::fichierSortie() const
{
	return !nomRapport.empty();
}

bool Rapport::dotFileExiste() const
{
	ifstream tf(nomRapport.c_str());
	return tf;
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
	relLoc.dest = noeudLocalc.id;
	relLoc.src = noeudLocalr.id;
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
	ofstream outPutFile;
	if(!nomRapport.empty())
	{
		outPutFile.open(nomRapport);
	}
	
	afficherTopDocs(consultations.size());

	for(set<noeud>::iterator it=noeuds.begin();it!=noeuds.end(); ++it)
	{
		if(consultations.at(it->id) >= nbHitsMin)
		{	
			if (!nomRapport.empty())
			{
				outPutFile << "node" << it->id << "[label=\"" << it->texte << "\"];" << endl;
			}
		}
	}
	
	for(set<relation>::iterator it=relations.begin(); it!=relations.end(); ++it)
	{
		if (!nomRapport.empty() && (consultations.at(it->src) >= nbHitsMin) && (consultations.at(it->dest) >= nbHitsMin))
		{
			outPutFile << "node" << it->src << "-> node" << it->dest << "[label=\"" << it->nbTot << "\"];" << endl;  
		}
	}
	
	if(!nomRapport.empty())
	{
		outPutFile.close();
	}	

	return true;
}

void Rapport::afficherTopDocs(const int nbDocs) const
{
	afficherContraintes();

	multimap<int,int> sortedNbCons;
	for(map<int,int>::const_iterator it = consultations.begin(); it != consultations.end(); ++it)
	{
		sortedNbCons.insert(pair<int,int>(it->second, it->first));
	}
	
	int i = 0;
	for(map<int,int>::iterator it = sortedNbCons.begin(); it != sortedNbCons.end() && i < nbDocs; ++it)
	{
		for(set<noeud>::iterator itset = noeuds.begin(); itset != noeuds.end() && itset->estLocal; ++itset)
		{
			if(itset->id == it->second)
			{
				cout << itset->texte << " (" << it->first << " hits)" << endl;
				break;
			}
		}
		i++;
	}

}

void Rapport::afficherContraintes() const
{
	if(exclusionFichiers)
	{
		cout << "Les fichiers de types suivants n'ont pas été pris en compte : ";
		for(list<string>::const_iterator it = TYPES_EXCLUS.begin(); it != TYPES_EXCLUS.end(); ++it)
		{
			if(it != TYPES_EXCLUS.begin())
			{
				cout << ", ";
			}
			cout << *it;
		}
		cout << endl;
	}
	if(nbHitsMin > 1)
	{
		cout << "Seuls les documents consultés " << nbHitsMin << " fois ou plus ont été comptabilisés" << endl;
	}
	if (creneauMin >= 0)
	{
		cout << "Seuls les documents consultés entre " << creneauMin << "h et " << creneauMin+1 << "h ont été comptabilisés" << endl;
	}

}
