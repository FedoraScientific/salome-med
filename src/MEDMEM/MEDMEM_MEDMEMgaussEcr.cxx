// Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
//           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
// 
#include "MEDMEM_MEDMEMgaussEcr.hxx"
/*
 * En attendant une correction de la gestion du mode d'acc�s au fichier dans MEDfichier
 * on int�gre la correction ici.
 */

namespace med_2_3 {
  extern "C" {
# define ICI                    {\
                                        fflush(stdout);\
                                        fprintf(stderr, "%s [%d] : " , __FILE__ , __LINE__ ) ;\
                                        fflush(stderr) ;\
                                }

# define ISCRUTE(entier)        {\
                                        ICI ;\
                                        fprintf(stderr,"%s = %d\n",#entier,entier) ;\
                                        fflush(stderr) ;\
                                }

# define SSCRUTE(chaine)        {\
                                        ICI ;\
                                        fprintf(stderr,"%s = \"%s\"\n",#chaine,chaine) ;\
                                        fflush(stderr) ;\
                                }
# define MESSAGE(chaine)        {\
                                        ICI ;\
                                        fprintf(stderr,"%s\n",chaine) ;\
                                        fflush(stderr) ;\
                                }

    extern void _MEDmodeErreurVerrouiller(void);

    /*************************************************************************
     * COPYRIGHT (C) 1999 - 2003  EDF R&D
     * THIS LIBRARY IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR MODIFY
     * IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE
     * AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
     * EITHER VERSION 2.1 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
     *
     * THIS LIBRARY IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT
     * WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
     * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. SEE THE GNU
     * LESSER GENERAL PUBLIC LICENSE FOR MORE DETAILS.
     *
     * YOU SHOULD HAVE RECEIVED A COPY OF THE GNU LESSER GENERAL PUBLIC LICENSE
     * ALONG WITH THIS LIBRARY; IF NOT, WRITE TO THE FREE SOFTWARE FOUNDATION,
     * INC., 59 TEMPLE PLACE, SUITE 330, BOSTON, MA 02111-1307 USA
     *
     *************************************************************************/

    /*
     * - Nom de la fonction : MEDgaussEcr
     * - Description : It�rateur renvoyant (n�pdt,n�or), le nbre de point de GAUSS pour le type d'�l�ment,
     *                 et le  maillage par d�faut avec son eventuel lien � un autre fichier.
     * - Parametres :
     *     - fid      (IN) : ID du fichier HDF courant
     *     - type_geo (IN) : Le type g�om�trique de l'entit� concern� {MED_POINT,MED_SEG2 ......}
     *     - refcoo   (IN) : Les coordonn�es des noeuds de l'�l�ment de r�f�rence (tableau de taille(typegeo%100)*(typegeo/100) )
     *     - mode_coo (IN) : Choix du type d'entrelacement utilis� en m�moire pour refcoo et gscoo
     *                        { MED_FULL_INTERLACE(x1,y1,z1,x2,...)) , MED_NO_INTERLACE(x1,x2,y1,y2,z1,z2) }
     *     - ngauss   (IN) : Nombre de points de Gauss pour l'�l�ment de r�f�rence consid�r�
     *     - gscoo    (IN) : Les coordonn�es des points de Gauss pour l'�l�ment de r�f�rence (tableau de taille ngauss*type_geo/100)
     *     - wg       (IN) : Poids � appliquer aux points d'int�gration (tableau de taille ngauss)
     *     - locname (IN) : Nom � associer � la localisation (MED_TAILLe_NOM)

     REM :
     <locname> est � passer en param�tre de MEDchampEcrire.
    */

    med_err
    MEDMEMgaussEcr(med_idt fid, med_geometrie_element type_geo, med_float *refcoo, med_mode_switch mode_coo,
		   med_int ngauss, med_float *gscoo, med_float * wg, char * locname )
    {
      med_idt gid=0, chid=0;
      med_size dimd[1];
      med_err ret = -1;
      med_int typegeo = -1;
      char chemin[MED_TAILLE_GAUSS+1]="";

      /*
       * On inhibe le gestionnaire d'erreur HDF 5
       */
      _MEDmodeErreurVerrouiller();

      /*
       * Si le groupe "GAUSS" n'existe pas, on le cree
       */
      strncpy(chemin,MED_GAUSS,MED_TAILLE_GAUSS-1);
      chemin[MED_TAILLE_GAUSS-1] = '\0';
      if ((gid = _MEDdatagroupOuvrir(fid,chemin)) < 0)
	if ((gid = _MEDdatagroupCreer(fid,chemin)) < 0) {
	  MESSAGE("Impossible de creer le groupe MED_GAUSS : ");
	  SSCRUTE(chemin); goto ERROR;
	}

      /*
       * Si le groupe <locname> n'existe pas, on le cree
       * Sinon => erreur
       */
      if ((chid = _MEDdatagroupOuvrir(gid,locname)) >= 0) {
	if ( false )//MED_MODE_ACCES != MED_LECTURE_ECRITURE )
  {
	  MESSAGE("Le nom de localisation existe d�j� : ");
	  SSCRUTE(locname); goto ERROR;
	}
      } else
	if ((chid = _MEDdatagroupCreer(gid,locname)) < 0)
	  goto ERROR;

      /*
       * On stocke <ngauss> sous forme d'attribut
       */
      if (_MEDattrEntierEcrire(chid,MED_NOM_NBR,&ngauss) < 0) {
	MESSAGE("Erreur � l'�criture de l'attribut MED_NOM_NBR : ");
	ISCRUTE(ngauss);goto ERROR;
      };

      /*
       * On stocke <type_geo> sous forme d'attribut
       */
      typegeo = (med_int) type_geo;
      /* sizeof(enum) tjrs = sizeof(int) en C, or
	 sur machines 64 bits par d�faut med_int==long,
	 du coup sur  machines 64 bits _MEDattrEntierEcrire utilise
	 le type hdf NATIVE_LONG, ce qui pose un probl�me qd on passe
	 un enum.
      */
      if (_MEDattrEntierEcrire(chid,MED_NOM_GEO,&typegeo) < 0) {
	MESSAGE("Erreur � l'�criture de l'attribut MED_NOM_GEO : ");
	ISCRUTE(type_geo);goto ERROR;
      };


      /*
       * On stocke les coordonn�es de r�f�rence dans un dataset
       */

      dimd[0] = (type_geo%100)*(type_geo/100);
      if ( _MEDdatasetNumEcrire(chid,MED_NOM_COO,MED_FLOAT64,mode_coo,(type_geo/100),MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
				(unsigned char*) refcoo)  < 0 ) {
	MESSAGE("Impossible d'ecrire le dataset : ");SSCRUTE(MED_NOM_COO);
	ISCRUTE(dimd); goto ERROR;
      }

      /*
       * On stocke les points d'int�gration dans un dataset
       */

      dimd[0] = ngauss*(type_geo/100);
      if ( _MEDdatasetNumEcrire(chid,MED_NOM_GAU,MED_FLOAT64,mode_coo,(type_geo/100),MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
				(unsigned char*) gscoo)  < 0 ) {
	MESSAGE("Impossible d'ecrire le dataset : ");SSCRUTE(MED_NOM_GAU);
	ISCRUTE(dimd); goto ERROR;
      }

      /*
       * On stocke les poids dans un dataset
       */

      dimd[0] = ngauss;
      if ( _MEDdatasetNumEcrire(chid,MED_NOM_VAL,MED_FLOAT64,mode_coo,1,MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
				(unsigned char*) wg)  < 0 ) {
	MESSAGE("Impossible d'ecrire le dataset : ");SSCRUTE(MED_NOM_VAL);
	ISCRUTE(dimd); goto ERROR;
      }


      ret = 0;

    ERROR:

      /*
       * On ferme tout
       */

      if (chid>0)     if (_MEDdatagroupFermer(chid) < 0) {
	MESSAGE("Impossible de fermer le datagroup : ");
	ISCRUTE(chid); ret = -1;
      }

      if (gid>0)     if (_MEDdatagroupFermer(gid) < 0) {
	MESSAGE("Impossible de fermer le datagroup : ");
	ISCRUTE(gid); ret = -1;
      }

      return ret;
    }

#undef MESSAGE
#undef SSCRUTE
#undef ISCRUTE

  }
}