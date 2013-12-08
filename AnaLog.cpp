//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============

#include <iostream>
#include <cstring>
#include <cctype>
#include <string>
#include <stdexcept>
#include <map>

#include "Config.h"
#include "LecteurLigneFichier.h"
#include "LigneLog.h"
#include "Rapport.h"

using namespace std;
using namespace LecteurFichier;



paramCode isParam(string &p)
{
	for(paramMap::const_iterator it = parameters.cbegin(); it != parameters.cend(); ++it)
	{
		if(it->second.compare(p) == 0)
		{
			return it->first;
		}
	}
	return paramCode::NULLP;
}

int main(int argc, const char **argv)
{
	int creneauMin = -1;	
	int nbHits = 1;
	bool fileExclusion = false;
	string logFile = "";
	string dotFile = "";



	// ================================= DEBUT analyse paramètres ================================================

	if(argc < 2)
	{
		printError(generalError::MISSING_ARGUMENT, " nom du fichier log");
		exit(EXIT_FAILURE);
	}

	string currParameter;
	paramCode currParamCode;
	paramCode prevParamCode = paramCode::NULLP;
	for( int i=1 ; i<argc ; i++ )
	{
		currParameter = argv[i];
		currParamCode = isParam(currParameter);
		if(prevParamCode != paramCode::NULLP && currParamCode != paramCode::NULLP)
		{
			printError(generalError::BAD_PARAMETER_VALUE, '\''+parameters.at(prevParamCode)+'\'');
			exit(EXIT_FAILURE);
		}
		else if( prevParamCode == paramCode::PARAM_DOTFILE )
		{
			dotFile = currParameter;
		}
		else if( prevParamCode == paramCode::PARAM_HITNB )
		{
			try
			{
				nbHits = stoi(currParameter);
			}
			catch(const invalid_argument& iae)
			{
				printError(generalError::BAD_PARAMETER_VALUE, '\''+parameters.at(prevParamCode)+'\'');
				exit(EXIT_FAILURE);
			}
			catch(const out_of_range &ofre)
			{
				printError(generalError::BAD_PARAMETER_VALUE, '\''+parameters.at(prevParamCode)+'\'');
				exit(EXIT_FAILURE);
			}
			if(nbHits < 1)
			{
				printError(generalError::BAD_PARAMETER_VALUE, '\''+parameters.at(prevParamCode)+"' (doit etre >=1)");
				exit(EXIT_FAILURE);
			}
		}
		else if( prevParamCode == paramCode::PARAM_HMIN )
		{
			try
			{
				creneauMin = stoi(currParameter);
			}
			catch(const invalid_argument& iae)
			{
				printError(generalError::BAD_PARAMETER_VALUE, '\''+parameters.at(prevParamCode)+'\'');
				exit(EXIT_FAILURE);
			}
			catch(const out_of_range &ofre)
			{
				printError(generalError::BAD_PARAMETER_VALUE, '\''+parameters.at(prevParamCode)+'\'');
				exit(EXIT_FAILURE);
			}
			if(creneauMin < 0 || creneauMin > 23)
			{
				printError(generalError::BAD_PARAMETER_VALUE, '\''+parameters.at(prevParamCode)+"' (doit être >= 0 et <= 23)");
				exit(EXIT_FAILURE);
			}
		}
		else if(prevParamCode == paramCode::NULLP && currParamCode == paramCode::NULLP)
		{
			logFile = currParameter;
		}

		if( currParamCode == paramCode::PARAM_EXCLFILES )
		{
			fileExclusion = true;
			prevParamCode = paramCode::NULLP;
		}
		else
		{
			prevParamCode = currParamCode;
		}
	}
	if(prevParamCode != paramCode::NULLP)
	{
		printError(generalError::BAD_PARAMETER_VALUE, '\''+parameters.at(prevParamCode)+'\'');
		exit(EXIT_FAILURE);
	}

	// ================================= FIN analyse paramètres ================================================

	// Tentative d'ouverture du fichier et controle d'erreur	
	LecteurLigneFichier<LigneLog> *lecteur;
	try
	{
		lecteur = new LecteurLigneFichier<LigneLog>(logFile);
	}
	catch(ErrcodeLecteurLigneFichier ellf)
	{
		printError(ellf," '"+logFile+"' ");	
		exit(EXIT_FAILURE);	
	}

	// Initialisation du rapport
	Rapport rapport(dotFile,nbHits,creneauMin,fileExclusion,0);
	
	// Si le fichier rapport existe déjà, on vérifie que l'utilisateur veut bien l'écraser
	if(rapport.fichierSortie() && rapport.dotFileExiste())
	{
		cout << "Le fichier '" << dotFile << "' existe déjà, voulez-vous l'écraser ? [o/N]" << endl;
		string rep;
		while(true)
		{
			getline(cin,rep);
			if(rep.empty() || (rep.length() == 1 && (toupper(rep[0]) == 'O' || toupper(rep[0]) == 'N')))
			{
				break;
			}
		}
		if(rep.empty() || toupper(rep[0]) == 'N')
		{
			printError(EXISTING_OUTPUT_FILE, " '"+dotFile+"' ");
			exit(EXIT_FAILURE);
		}
	}

	rapport.afficherContraintes();
	
	LigneLog *ligne;
	while((ligne = lecteur->ligneSuivante()) != NULL || !lecteur->endOfFile())
	{
		if(lecteur->valid())
		{
			if(!rapport.ajouterLigne(ligne))
			{
				cout << "Attention, ligne " << lecteur->position() << " valide mais non traitée" << endl;
			}
			delete ligne;
		}
	}

	if(rapport.fichierSortie())
	{
		rapport.genererRapport();
	}
	else
	{
		rapport.afficherTopDocs(NB_TOP_DOCS);
	}

	delete lecteur;

	printError(generalError::OK);
	return EXIT_SUCCESS;
}
