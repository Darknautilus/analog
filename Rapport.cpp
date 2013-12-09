//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============


#include <list>
#include <cctype>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "Rapport.h"
#include "Config.h"

using namespace std;

// Foncteur servant à mettre une chaîne de caractères en majuscules
struct stoupper
{ 
	char operator()(char c) const 
	{
		return toupper(static_cast<unsigned char>(c));
	} 
};

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

	if(DEBUG)
	{
		cout << endl << "Traitement de :" << endl << *ligne << endl;
	}

	// Vérification paramètre temporel
	if (creneauMin >= 0 && ligne->creneau != creneauMin)
	{
		return false;
	}

	/*
	 * Si l'utilisateur a choisi d'exclure certains fichiers, ce bloc sert à parcourir la liste des extensions à exclure (cf Config.h)
	 * Ce parcours n'est pas sensible à la casse.
	 */	
	if(exclusionFichiers)
	{
		int dotPos = ligne->cible.rfind('.');
		if(dotPos != string::npos)
		{
			string extension = ligne->cible.substr(dotPos,ligne->cible.length()-dotPos);
			transform(extension.begin(),extension.end(),extension.begin(),stoupper()); // Met la chaîne en majuscules
			for( list<string>::const_iterator it = TYPES_EXCLUS.begin() ; it != TYPES_EXCLUS.end() ; ++it )
			{
				if(it->compare(extension) == 0)
				{
					return false;
				}
			}
		}
	}

	/*
	 * Ajout des noeuds (cibles + referers) dans le set
	 */
	noeud noeudLocalc;
	noeud noeudLocalr;
	noeudLocalc.texte = ligne->cible;
	noeudLocalc.estLocal = true; // Une cible est toujours locale
	string strLocale = ligne->referer.substr(0,LOCALHOST.length());    
	if(strLocale.compare(LOCALHOST) == 0)
	{
		noeudLocalr.estLocal = true;
		noeudLocalr.texte = ligne->referer.substr(LOCALHOST.length(), (ligne->referer.length()-LOCALHOST.length()));
	}
	else
	{
		noeudLocalr.texte = ligne->referer;
		noeudLocalr.estLocal = false;
	}

	bool cibleExiste = false;
	bool refererExiste = false;

	if(noeudLocalr.texte.empty())
	{
		refererExiste = true; // Sert à éviter l'insertion dans le set et le parcours des boucles ci-après
	}

	/*
	 * On parcours les noeuds jusqu'à la fin du set ou bien jusqu'à ce que le referer et la cible aient été trouvés
	 * On peut noter que les conditions dans les if sont optimisées pour n'exécuter les comparaisons de chaînes qu'en dernier recours.
	 */	
	for (set<noeud>::const_iterator it = noeuds.begin(); it != noeuds.end() && (!cibleExiste || !refererExiste) ; ++it)
	{
		if (!cibleExiste && it->estLocal && it->texte.compare(noeudLocalc.texte) == 0)
		{
			if(DEBUG)
			{
				cout << "Cible existante : " << *it << endl;
			}
			cibleExiste = true;
			noeudLocalc.id = it->id;
			if(consultations.count(it->id) > 0)
			{
				(consultations.at(it->id))++;
			}
		}
		if (!refererExiste && it->texte.compare(noeudLocalr.texte) == 0)
		{
			refererExiste = true;
			noeudLocalr.id = it->id;
			if(DEBUG)
			{
				cout << "Referer existant : " << *it << endl;
			}
		}
	}

	if( !cibleExiste )
	{
		noeudLocalc.id = dernierId++;
		pair<set<noeud>::iterator, bool> cibleInserted = noeuds.insert(noeudLocalc);
		if(DEBUG)
		{
			cout << "Cible " << noeudLocalc;
			if(!cibleInserted.second)
			{
				cout << " non";
			}
			cout << " ajoutée" << endl;
		}
		consultations.insert(pair<int,int>(noeudLocalc.id ,1));
	}
	if( !refererExiste )
	{
		noeudLocalr.id = dernierId++;
		pair<set<noeud>::iterator, bool> refInserted = noeuds.insert(noeudLocalr);
		if(DEBUG)
		{
			cout << "Referer " << noeudLocalr;
			if(!refInserted.second)
			{
				cout << " non";
			}
			cout << " ajouté" << endl;
		}
	}

	// S'il n'y a pas de referer, il ne peut y avoir de relation
	if(!noeudLocalr.texte.empty())
	{
		/*
		 * Ajout des relations dans le set
		 */
		relation relLoc;
		relLoc.dest = noeudLocalc.id;
		relLoc.src = noeudLocalr.id;
		relLoc.nbTot = 1;
		bool relationExiste = false;

		int oldTot;
		set<relation>::iterator it; 
		for (it = relations.begin() ; it != relations.end() && !relationExiste ; ++it)
		{
			if(*it == relLoc)
			{
				relationExiste = true;
				oldTot = it->nbTot;
				if(DEBUG)
				{
					cout << "Relation existante : " << *it << endl;
				}
			}
		}
		if(relationExiste)
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
			if(DEBUG)
			{
				cout << "Relation " << relLoc << " ajoutée" << endl;
			}
		}  
	}

	return true;
}

bool Rapport::genererRapport() const
{
	ofstream outPutFile;
	// Teste si l'utilisateur avait bien demandé un fichier Dot
	if(!fichierSortie())
	{
		return false;
	}
	else
	{
		outPutFile.open(nomRapport.c_str());
		if(!outPutFile)
		{
			return false;
		}
	}
	
	afficherTopDocs(consultations.size());

	outPutFile << "digraph {" << endl;
	for(set<noeud>::iterator it=noeuds.begin(); it!=noeuds.end(); ++it)
	{
		bool estCons = (consultations.count(it->id) > 0);
		if(!estCons || consultations.at(it->id) >= nbHitsMin)
		{
			outPutFile << "node" << it->id << " [label=\"" << it->texte << "\"];" << endl;
		}
	}
	
	for(set<relation>::iterator it=relations.begin(); it!=relations.end(); ++it)
	{
		bool RestCons = (consultations.count(it->src) > 0);
		bool CestCons = (consultations.count(it->dest) > 0);
		if ((!RestCons || (consultations.at(it->src) >= nbHitsMin)) && (!CestCons || (consultations.at(it->dest) >= nbHitsMin)))
		{
			outPutFile << "node" << it->src << " -> node" << it->dest << " [label=\"" << it->nbTot << "\"];" << endl;  
		}
	}
	
	outPutFile << "}" << endl;
	outPutFile.close();
	cout << "Fichier Dot généré sous le nom de '" << nomRapport << "'" << endl;

	return true;
}

void Rapport::afficherTopDocs(const int nbDocs) const
{

	if(DEBUG)
	{
		cout << "====== Noeuds ======" << endl;
		for(set<noeud>::const_iterator it = noeuds.begin(); it != noeuds.end(); ++it)
		{
			cout << *it << endl;
		}


		cout << "====== Consultations ======" << endl;
		int i = 1;
		for(map<int,int>::const_iterator it = consultations.begin(); it != consultations.end(); ++it)
		{
			cout << i << " : " << it->first << "|" << it->second << endl;
			i++;
		}
	}

	// On inverse la map en triant par nombre de consultations
	multimap<int,int> sortedNbCons;
	for(map<int,int>::const_iterator it = consultations.begin(); it != consultations.end(); ++it)
	{
		if(it->second >= nbHitsMin)
		{
			sortedNbCons.insert(pair<int,int>(it->second, it->first));
		}
	}

	if(DEBUG)
	{
		cout << "====== Consultations (triées par nb de consultations)  ======" << endl;
		int i = 1;
		for(multimap<int,int>::const_iterator it = sortedNbCons.begin(); it != sortedNbCons.end(); ++it)
		{
			cout << i << " : " << it->first << "|" << it->second << endl;
			i++;
		}
		cout << endl << "========== End DEBUG infos ==========" << endl;
	}

	int nbDocsAff;
	if(nbHitsMin > 1)
	{
		nbDocsAff = sortedNbCons.size();
	}
	else
	{
		nbDocsAff = nbDocs;
	}
	cout << endl << "Affichage des " << nbDocsAff << " documents";
	if(nbDocsAff == NB_TOP_DOCS)
	{
		cout << " les plus consultés";
	}
	cout << " : " << endl << "------------------------" << endl;
	int i = 0;
	for(multimap<int,int>::reverse_iterator it = sortedNbCons.rbegin(); it != sortedNbCons.rend() && i < nbDocsAff; ++it)
	{
		for(set<noeud>::const_iterator itset = noeuds.begin(); itset != noeuds.end() && itset->estLocal; ++itset)
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
		cout << "Les fichiers de types suivants ne seront pas traités : ";
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
		cout << "Seuls les documents consultés " << nbHitsMin << " fois ou plus seront comptabilisés" << endl;
	}
	if (creneauMin >= 0)
	{
		cout << "Seuls les documents consultés entre " << creneauMin << "h et " << creneauMin+1 << "h seront comptabilisés" << endl;
	}

}
