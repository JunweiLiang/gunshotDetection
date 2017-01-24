/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: AR Module
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  ARModul.h
    Date    :  $Id: ARModul.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef AR_MODEL_YET_INCLUDED
    #define AR_MODEL_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mach_ind_io.h"
#include "Gaussian.h"
#include "matrix.h"

/*===================================================================================
/  struct <NoiseModel>
===================================================================================*/
struct ARModel
{
    /*-------------------------------------------------------------------------
    /  VARIABLES
    /------------------------------------------------------------------------*/
    int                 dim;
    int                 modelOrder;
    float               smoothing;
    float**             coeffMatrix;
    float**             coeffMatrixHistory;
    float**             D1;
    float**             D2;
    float**             D1temp;
    float**             D2temp;
    Gaussian*           errorDistr;
    DMatrix*		matD1;
    DMatrix*		matD2;
    DMatrix*		matD3;

    /*-------------------------------------------------------------------------
    /  INTERNAL VARIBALES
    /------------------------------------------------------------------------*/
    Gaussian*           workingErrorDistr;

    /*-------------------------------------------------------------------------
    /  FUNCTIONS
    /------------------------------------------------------------------------*/
    void (*sampleFromPD)(struct ARModel* arModel, float* N, float* predicted_ntp1, float nioInDB);
    void (*predict)(struct ARModel* arModel, float* N, float* predicted_ntp1, float nioInDB);
    void (*destruct)(struct ARModel* arModel);
} typedef ARModel;

/*===================================================================================
/  functions
===================================================================================*/
ARModel* createARModel(int dim, int modelOrder, Gaussian* errorDistr);

#endif
