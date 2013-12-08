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

const bool DEBUG = false;

const string LOCALHOST = "http://intranet-if.insa-lyon.fr";
const list<string> TYPES_EXCLUS {".JPG",".JPEG",".PNG",".GIF",".BMP",".JS",".CSS",".ICO"};
const int NB_TOP_DOCS = 10;

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
