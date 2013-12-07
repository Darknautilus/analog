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
	noeuds.clear();
	relations.clear();
}

bool Rapport::ajouterLigne(const LigneLog *ligne)
{
	//Vérification paramètre temporel
	//if (creneauMin < 0)
	//{
	//	return false;
	//}

	////Vérification paramètre d'extension
	//string extension = "";
	//string lcible = ligne->cible;
	//string lreferer = ligne->referer;
	//int strlencib = lcible.length();
	//for (int i=strlencib-1; i>=0; i--)
	//{
	//	if(lcible.at(i)!='.')
	//	{
	//		extension += lcible.at(i);
	//	}
	//	else
	//	{
	//		i=-1;
	//	}
	//}
	
	int dotPos = ligne->cible.rfind('.');
	string extension = ligne->cible.substr(dotPos,ligne->cible.length()-dotPos);
	for( list<string>::const_iterator it = TYPES_EXCLUS.begin() ; it != TYPES_EXCLUS.end() ; ++it )
	{
		if(it->compare(extension) == 0)
		{
			return false;
		}
	}
	

	//list <string> lextension {extension};
	//list <string> motsCommuns;
	//set_intersection(TYPES_EXCLUS.begin(), TYPES_EXCLUS.end(), lextension.begin(), lextension.end(), motsCommuns.begin());
	//if (!motsCommuns.empty())
	//{
	//	return false;
	//}
  
	//Changements dans la structure noeuds
	noeud noeudLocalc;
	noeud noeudLocalr;
	noeudLocalc.texte = ligne->cible;
	noeudLocalc.estLocal = true;
	//noeudLocalc.id = dernierId++;
	//noeudLocalr.id = dernierId++;
	string strLocale = ligne->referer.substr(0,LOCALHOST.length());    
	if(strLocale.compare(LOCALHOST) == 0)
	{
		noeudLocalr.estLocal = true;
		noeudLocalr.texte = ligne->referer.substr(32, (ligne->referer.length()-31)); // Que signifie ce 31 ??
	}
	else
	{
		noeudLocalr.texte = ligne->referer;
	}

	bool cibleExiste = false;
	bool refererExiste = false;
	int cibleId;
	int refId;
	//set<noeud>::iterator itCibEx;
  //set<noeud>::iterator itRefEx;
	
	for (set<noeud>::iterator it = noeuds.begin(); it != noeuds.end() && (!cibleExiste || !refererExiste) ; ++it)
	{
		if (!cibleExiste && it->estLocal && it->texte.compare(ligne->cible) == 0)
		{
			cibleExiste = true;
			//itCibEx = it;
			cibleId = it->id;
			(consultations.at(it->id))++;
		}
		if (!refererExiste && it->texte.compare(ligne->referer) == 0)
		{
			refererExiste = true;
			//itRefEx = it;
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

	//if (cibleExiste)
	//{
	//	noeudLocalc.nbCons++;
	//	noeuds.insert(noeudLocalc);
	//}
	//else if (refererExiste) 
	//{
	//	noeuds.insert(noeudLocalr); 
	//}
	//else if (!cibleExiste)
	//{
	//	itCibEx->nbCons++;
	//}

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
	ofstream outPutFile;
	if(!nomRapport.empty())
	{
		outPutFile.open(nomRapport);
	}
	
	if (creneauMin>=0)
	{
		cout<<"Attention, seules les logs comprises entre "<< creneauMin << " et " << creneauMin+1 << "h seront prises en compte" << endl;
	}

	for(set<noeud>::iterator it=noeuds.begin();it!=noeuds.end(); ++it)
	{
		if(it->nbCons >= nbHitsMin)
		{	
			
			cout<<(*it).texte<<" ("<<(*it).nbCons<<"hits)"<<endl;

			if (!nomRapport.empty())
			{
				outPutFile << "node" << it->id << "[label=\"" << it->texte << "\"];" << endl;
			}
		}
	}
	
	for(set<relation>::iterator it=relations.begin(); it!=relations.end(); ++it)
	{
		if ((it->src->nbCons >= nbHitsMin) && (it->dest->nbCons >= nbHitsMin))
		{
			outPutFile << "node" << it->src->id << "-> node" << it->dest->id << "[label=\"" << it->nbTot << "\"];" << endl;  
		}
	}
	
	if(!nomRapport.empty())
	{
		outPutFile.close();
	}	

	return true;
}

void Rapport::afficherTopDocs() const
{
}
