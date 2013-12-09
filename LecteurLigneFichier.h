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

	/*
	 * Interface décrivant une ligne pouvant être traitée par LecteurLigneFichier
	 * La méthode estValide() doit être redéfinie dans les classes héritant de LigneFichier
	 */	
	class LigneFichier
	{
		protected:
			LigneFichier() {}
			virtual bool estValide() const = 0;
			virtual ~LigneFichier() {}
	};


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
	
	/*
	 * class LecteurLigneFichier
	 *
	 * Cette classe générique a pour but de fournir une interface simple pour la lecture d'un fichier texte ligne par ligne.
	 * C'est pourquoi elle est réutilisable, la seule contrainte étant de l'utiliser avec une spécialisation de la classe abstraite LigneFichier dont la déclaration est disponible ci-dessus.
	*/
	template <typename T> // T doit hériter de LigneFichier !
	class LecteurLigneFichier
	{
		public:
			/*
			 * Tente d'ouvrir le fichier dont le nom est donné en paramètres
			 * En cas d'erreur, soulève l'exception ErrcodeLecteurLigneFichier::ERR_OUVERTURE
			 */
			LecteurLigneFichier(string &);
			virtual ~LecteurLigneFichier();

			/*
			 * Lit la ligne suivante dans ficDesc et construit l'objet descripteur de ligne
			 * R : un pointeur vers l'objet T construit ou NULL en cas d'erreur
			 * Attention l'objet retourné doit être détruit manuellement !
			 */
			T *ligneSuivante();
			bool endOfFile() const;
			/*
			 * Indique si la dernière ligne lue est valide ou non
			 */
			bool valid() const;
			/*
			 * Donne le numéro de la ligne à laquelle se trouve le descripteur ficDesc
			 */
			int position() const;

		private:
			ifstream ficDesc; // Le descripteur de fichier
			bool eof; // Retourné par endOfFile()
			bool validline; // Retourné par valid()
			int lineNb; // Retourné par position()
	};

	template <typename T>
	LecteurLigneFichier<T>::LecteurLigneFichier(string &nomFic) :
			ficDesc(nomFic.c_str()), eof(false), lineNb(1)
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
	int LecteurLigneFichier<T>::position() const
	{
		return lineNb;
	}

	template <typename T>
	T *LecteurLigneFichier<T>::ligneSuivante()
	{
		string ligne;
		// Récupère la ligne suivante dans le descripteur de fichiers
		getline(ficDesc, ligne);
		lineNb++;
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
