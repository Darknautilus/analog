//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <iostream>
#include <string>
#include <map>
#include <list>

using namespace std;

// ==================================== Constantes ======================================

const bool DEBUG = false;

// Adresse du serveur local (pour identifier les referers comme étant du serveur local)
const string LOCALHOST = "http://intranet-if.insa-lyon.fr";
// Liste des types exclus par l'option -x
const list<string> TYPES_EXCLUS {".JPG",".JPEG",".PNG",".GIF",".BMP",".JS",".CSS",".ICO"};
// Nombre de documents à afficher lors d'une exécution sans option
const int NB_TOP_DOCS = 10;

// ==================================== Options du programme ======================================

// Représente les options du programme
enum paramCode
{
	NULLP,
	PARAM_DOTFILE,
	PARAM_HITNB,
	PARAM_EXCLFILES,
	PARAM_HMIN,
	TEST_MODE
};

typedef map<paramCode,string> paramMap; 
const paramMap parameters =
{
	{PARAM_DOTFILE,"-g"},
	{PARAM_HITNB,"-l"},
	{PARAM_EXCLFILES,"-x"},
	{PARAM_HMIN,"-t"},
	{TEST_MODE,"--test"}
};

/*
 *	Détermine si une chaîne donnée est une option du programme
 *	E : un objet string
 *	R : le code de l'option correspondante ou NULLP sinon
 */
inline paramCode isParam(string &p)
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

// ==================================== Erreurs ======================================

enum generalError
{
	OK,
	MISSING_ARGUMENT,
	BAD_PARAMETER_VALUE,
	INPUT_FILE_OPENING_ERROR,
	EXISTING_OUTPUT_FILE
};

typedef map<generalError,string> generalErrorMap;
const generalErrorMap generalErrors =
{
	{OK,"Le programme s'est correctement déroulé"},
	{MISSING_ARGUMENT,"Argument obligatoire manquant :"},
	{BAD_PARAMETER_VALUE,"Valeur incorrecte pour le paramètre :"},
	{INPUT_FILE_OPENING_ERROR,"Le fichier n'a pas pu être ouvert :"},
	{EXISTING_OUTPUT_FILE,"Fichier de sortie déjà existant :"}
};

inline void printError(generalError ecode,string message = "")
{
	cout << generalErrors.at(ecode) << " " << message << " (";
	cerr << ecode;
	cout << ")" << endl;
}

#endif
