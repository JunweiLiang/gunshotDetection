/*****************************************************************************
 *   Modification log: Pls insert comments here whenever you modify the file
 *****************************************************************************/
/*****
 *   Modifier:
 *   Date:
 *   Reason:
 ******/

/*****************************************************************************
 *   Author:   Wilson Tam
 *   Email:    cswilson@cs.ust.hk
 *
 *   Description:
 *   Usage:
 *****************************************************************************/

#ifndef READHTKFORMAT_H
#define READHTKFORMAT_H

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include "util_new.h"

using namespace std;

// HTK convention
enum _BaseParmKind {
   WAVEFORM,                                      /* Raw speech waveform (handled by HWave) */
   LPC,LPREFC,LPCEPSTRA,LPDELCEP,                 /* LP-based Coefficients */
   IREFC,                                         /* Ref Coef in 16 bit form */
   MFCC,                                          /* Mel-Freq Cepstra */
   FBANK,                                         /* Log Filter Bank */
   MELSPEC,                                       /* Mel-Freq Spectrum (Linear) */
   USER,                                          /* Arbitrary user specified data */
   DISCRETE,                                      /* Discrete VQ symbols (shorts) */
   ANON
};


// if macro is not defined, then set it to true by default
#ifndef SWAP
#define SWAP   false
#endif

#define MFCC_MAX_LIST_SIZE            20000

class MFCCReader;

class MFCCReader {
public:
   struct HTKHeaderType {
      int nSamples;
      int sampPeriod;
      short sampSize;
      short paramKind;
   };                                          // 12 bytes

private:
   vector<string>   m_FileList;    // bat feature filename
   bool             m_NeedSwap;    // swap parameter
   long             m_Dim;         // feature dim
   long             m_T;           // total frames in current utterance
   HTKHeaderType    m_Header;      // htk header
   float**          m_X;           // feature vectors
   long             m_maxT;        // Max # frame in buffer
   long             m_maxDim;      // Max # dimension in buffer
   bool             m_isXShared;   // to denote if X is shared
   bool             m_genDeltaCep; // flag to generate deltacep online
   bool             m_genAccCep;   // flag to generate Acccep online
   bool             m_eNormalize;  // flag to do E-Normalization
   bool             m_cms;         // flag to do Cepstral-Mean substraction
   int              m_energyPos;   // pos of energy term
   float            m_energyOffset; // offset value during E-Norm
   int              m_staticCepStartPos; // C1 pos

   long size[2];
   long offset[2];

   MFCCReader(const MFCCReader& r);
   MFCCReader& operator=(const MFCCReader& r);

public:
   MFCCReader();
   MFCCReader(bool swap);
   virtual ~MFCCReader();

   void set_num_frame(const long& T) { m_T = T; }
   void set_num_dimension(const long& D) { m_Dim = D; }

   inline float* operator[](const long& t) const {return m_X[t];}
   inline long getTotalUtterance() const {return long(m_FileList.size());}
   inline long getTotalFrames() const {return m_T;}
   inline long getTotalBands() const {return 1;}

   inline const string& getFilename(const long& id) const {return m_FileList[id];}
   inline const string& getFilename(const long& b, const long& id) const 
   { return this->getFilename(id); }

   inline long getSampPeriod() const {return m_Header.sampPeriod;}

   inline HTKHeaderType readHeader(const long& id) const {return this->readHeader(m_FileList[id]); }

   HTKHeaderType readHeader(const string& filename) const;

   long readFileList(const string& filelist);
   long readData(const string& filename);
   inline long readData(const long& id) {return this->readData(m_FileList[id]);}

   long writeData(const string& filename);
   inline long writeData(const long& id) {return this->writeData(m_FileList[id]);}

   void setShareX(float **X, int maxT, int maxD);
   long getMaxT() const;
   long getMaxDim() const;
   inline long dim() const {return m_Dim;}

   const HTKHeaderType& getHeader() const;
   void setHeader(const HTKHeaderType& header);

   inline void setSwap(const bool& swap) {m_NeedSwap = swap;}
   inline bool getSwap() const { return m_NeedSwap; }

   inline float** getX() const { return m_X; }

   // post-processing stuff
   void setComputeDynamicCep(bool delta, bool acc);
   void setENormalize(bool flag, int energy_pos, float energy_offset);
   void setCMS(bool flag, int static_cep_pos);

};

#endif
