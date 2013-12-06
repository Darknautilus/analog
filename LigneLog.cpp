//================
// TP C++ 2 : AnaLog
// par B3208 : Aurélien Bertron et Alicia Parisse
// ===============

#include <iostream>
#include <stdlib.h>
#include <string>
#include "LigneLog.h"

using namespace std;

ostream& operator<<(ostream &stream, const LigneLog &ligne)
{
	stream << '(' << ligne.creneau << ')' << ligne.referer << " --> " << ligne.cible;
	return stream;	
}

LigneLog::LigneLog(string &ligne)
{
  string ldate = "";
  string lcible = "";
  string lreferer = "";
  int cptInfos = 0;
  int strlen = ligne.length();
  int i = 0;

  while(cptInfos < 9 && i < (strlen-1))
  {
    if (ligne.at(i+1) == ' ')
    {
			cptInfos++;
			i++;
      if (ligne.at(i+1) == '[')
      {
				i++;
        while (ligne.at(i) != ']')
        {
          i++;
          ldate += ligne.at(i);
        }
				i--;
      }
      else if (ligne.at(i+1) =='"')
      {
				i+=2;
        if(cptInfos == 4)
        {
          while (ligne.at(i)!='"')
          {
            lcible += ligne.at(i);
            i++;
          }
        }
        else if(cptInfos == 7)
        {
          while (ligne.at(i)!='"')
          {
            lreferer += ligne.at(i);
            i++;
          }
        }
				i--;
      }
    }
    i++;
  }
  
	if (i == strlen && cptInfos != 9)
  {
    valide = false;
  }
  else
  {
    creneau = atoi(ldate.substr(12,2).c_str());
    int firstSlash = lcible.find("/");
    cible = lcible.substr(firstSlash,lcible.rfind(" ")-firstSlash);
    if(lreferer == "-")
    {
      referer = "";
    }
    else
    {
      referer = lreferer;
    }
    valide = true;
  }
}

LigneLog::~LigneLog()
{
}

bool LigneLog::estValide() const
{
  return valide;
}
