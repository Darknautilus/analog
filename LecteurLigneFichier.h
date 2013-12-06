//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============


#ifndef LECTEURLIGNEFICHIER_H_
#define LECTEURLIGNEFICHIER_H_

#include <fstream>
#include <string>
#include <map>

#include <iostream>

namespace LecteurFichier
{
	using namespace std;
	
	class LigneFichier
	{
		protected:
			LigneFichier() {}
			virtual bool estValide() const = 0;
			virtual ~LigneFichier() {}
	};

	/*
	 * class LecteurLigneFichier
	 *
	 * Cette classe générique a pour but de fournir une interface simple pour la lecture d'un fichier texte ligne par ligne.
	 * C'est pourquoi elle est réutilisable, la seule contrainte étant de l'utiliser avec une spécialisation de la classe abstraite LigneFichier dont la déclaration est disponible ci-dessus.
	*/
	enum ErrcodeLecteurLigneFichier
	{
		ERR_OUVERTURE
	};
	typedef map<ErrcodeLecteurLigneFichier,string> ErrLecteurLigneFichier;
	const ErrLecteurLigneFichier LecteurLigneFichierErrors =
	{
		{ERR_OUVERTURE,"Erreur a l'ouverture du fichier :"}
	};
	inline void printError(ErrcodeLecteurLigneFichier ecode,string message = "")
	{
		cout << LecteurLigneFichierErrors.at(ecode) << " " << message << " (";
		cerr << ecode;
		cout << ")" << endl;
	}

	template <typename T>
	class LecteurLigneFichier
	{
		public:
			LecteurLigneFichier(string &nomFic);
			virtual ~LecteurLigneFichier();

			T *ligneSuivante();
			bool endOfFile() const;
			bool valid() const;

		private:
			ifstream ficDesc;
			bool eof;
			bool validline;
	};

	template <typename T>
	LecteurLigneFichier<T>::LecteurLigneFichier(string &nomFic) :
			ficDesc(nomFic.c_str()), eof(false)
	{
		if(!ficDesc.is_open())
		{
			throw ErrcodeLecteurLigneFichier::ERR_OUVERTURE;
		}
	}

	template <typename T>
	LecteurLigneFichier<T>::~LecteurLigneFichier()
	{
	}

	template <typename T>
	bool LecteurLigneFichier<T>::endOfFile() const
	{
		return eof;
	}

	template <typename T>
	bool LecteurLigneFichier<T>::valid() const
	{
		return validline;
	}

	template <typename T>
	T *LecteurLigneFichier<T>::ligneSuivante()
	{
		string ligne;
		getline(ficDesc, ligne);
		if(ficDesc.eof() || ficDesc.fail() || ficDesc.bad())
		{
			if( ficDesc.eof() )
			{
				eof = true;
			}
			validline = false;
			return NULL;
		}
		else
		{
			T *objet = new T(ligne);
			if(objet->estValide())
			{
				validline = true;
				return objet;
			}
			else
			{
				delete objet;
				validline = false;
				return NULL;
			}
		}
	}

}

#endif /* LECTEURLIGNEFICHIER_H_ */
