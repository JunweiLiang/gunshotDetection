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

#include <fstream>
#include "readHTKformat.h"
//#include "global.h"
//#include "input.h"
#include "computebelldelta.h"
#include "computebell_fe_preprocess.h"

#define MAX_FRAME   35000
#define MAX_DIM     256

using namespace std;

const long max_offset = 20;
const long half_offset = max_offset/2;

MFCCReader::MFCCReader() : 
m_NeedSwap(SWAP),
m_Dim(0),
m_T(0),
m_isXShared(false),
m_genDeltaCep(false),
m_genAccCep(false),
m_eNormalize(false),
m_cms(false),
m_energyPos(-1),
m_energyOffset(-1.0),
m_staticCepStartPos(-1)
{
   // reserve memory for efficiency
   m_FileList.reserve(MFCC_MAX_LIST_SIZE);

   //New2DArray(m_X,m_maxT=MAX_FRAME,m_maxDim=MAX_DIM);
   
   m_maxT = MAX_FRAME;
   m_maxDim = MAX_DIM;

   size[0] = m_maxT+max_offset;
   size[1] = m_maxDim+max_offset;

   offset[0] = half_offset;
   offset[1] = half_offset;

   m_X = New_2D_Array<float>(size, offset, 0.0);
}


MFCCReader::MFCCReader(bool swap) : 
m_NeedSwap(swap),
m_Dim(0),
m_T(0),
m_isXShared(false),
m_genDeltaCep(false),
m_genAccCep(false),
m_eNormalize(false),
m_cms(false),
m_energyPos(-1),
m_energyOffset(-1.0),
m_staticCepStartPos(-1)
{
   // reserve memory for efficiency
   m_FileList.reserve(MFCC_MAX_LIST_SIZE);
   //New2DArray(m_X,m_maxT=MAX_FRAME,m_maxDim=MAX_DIM);

   m_maxT = MAX_FRAME;
   m_maxDim = MAX_DIM;

   long size[2] = {m_maxT+max_offset, m_maxDim+max_offset};
   long offset[2] = {half_offset, half_offset};

   m_X = New_2D_Array<float>(size, offset, 0.0);
}

void MFCCReader::setShareX(float **X, int maxT, int maxD)
{
   // clear memory if it's a non-share and non-empty buffer
   //if (!m_isXShared && m_X) Delete2DArray(m_X,m_maxT,m_maxDim);

   if (!m_isXShared && m_X) Delete_2D_Array<float>(m_X, size, offset);

   // set shared!
   m_isXShared = true;

   // share buffer!
   m_X = X;
   m_maxT = maxT;
   m_maxDim = maxD;
}

long MFCCReader::getMaxT() const
{
   return m_maxT;
}

long MFCCReader::getMaxDim() const
{
   return m_maxDim;
}

MFCCReader::MFCCReader(const MFCCReader& r) {
   cout << "MFCCReader copy constructor is disabled!";
   exit(-1);
}


MFCCReader& MFCCReader::operator=(const MFCCReader& r) {
   cout << "MFCCReader assignment operator is disabled!";
   exit(-1);
}


MFCCReader::~MFCCReader() 
{
   //if (!m_isXShared) Delete2DArray(m_X,m_maxT,m_maxDim);

   if (!m_isXShared) Delete_2D_Array<float>(m_X, size, offset);
}


long MFCCReader::readFileList(const string& filelist) {
   ifstream is(filelist.c_str());
   static string   str;

   if (is.fail()) {
      is.close();
      cout << "Exception: file open error : "+filelist;
      exit(-1);
   }

   // clear record
   m_FileList.clear();
   while (is >> str) {
      m_FileList.push_back(str);
   }

   // clean up
   is.close();

   return m_FileList.size();
}

const MFCCReader::HTKHeaderType& MFCCReader::getHeader() const
{
   return m_Header;
}

void MFCCReader::setHeader(const HTKHeaderType& header)
{
   m_Header = header;
}


MFCCReader::HTKHeaderType MFCCReader::readHeader(const string& filename) const {
   ifstream is(filename.c_str(),ios::in|ios::binary);
   HTKHeaderType header;

   if (is.fail()) {
      is.close();
      cout << "Exception: file open error : "+filename;
      exit(-1);
   }

   // read htk header
   is.read((char *)&header,sizeof(m_Header));

   if (m_NeedSwap) {
      swapLong((char *) &header.nSamples);
      swapLong((char *) &header.sampPeriod);
      swapShort((char *) &header.sampSize);
      swapShort((char *) &header.paramKind);
   }

   is.close();

   return header;
}

void MFCCReader::setComputeDynamicCep(bool delta, bool acc)
{
   m_genDeltaCep = delta;
   m_genAccCep = acc;
}

long MFCCReader::readData(const string& filename) 
{
   ifstream is(filename.c_str(),ios::in|ios::binary);
   long cepDim = 0;

   if (is.fail()) {
      is.close();
      cout << "Exception: file open error : "+filename;
      exit(-1);
   }

      float fVal;
      long  t,d;

      // read htk header
      is.read((char *)&m_Header,sizeof(m_Header));

      if (m_NeedSwap) {
         swapLong((char *) &m_Header.nSamples);
         swapLong((char *) &m_Header.sampPeriod);
         swapShort((char *) &m_Header.sampSize);
         swapShort((char *) &m_Header.paramKind);
      }

      cepDim = m_Dim = m_Header.sampSize / sizeof(float);
      m_T   = m_Header.nSamples;


      // see if deltaCep and accCep are computed online. If so, the size of buffer may need to resize!!
      if (m_genDeltaCep) {
         if (m_genAccCep) {
	    m_Dim *= 3;
	 } else {
	    m_Dim *= 2;
	 }
      } 

      if (m_T > m_maxT || m_Dim > m_maxDim) {
	      cout << "feature buffer overflow!" << endl;
	      exit(-1);
      }
      /*
      // re-allocate memory for T
      if (m_T > m_maxT) {
         cout << "reallocate feature buffer duration from " << m_maxT << " to " << m_T<< endl;
         ResizeArray(m_X,m_T);
         for (t=m_maxT;t<m_T;t++) {
            New1DArray(m_X[t],m_maxDim);
         }
         m_maxT = m_T;
      }

      // re-allocate memory for dim
      if (m_Dim > m_maxDim) {
         cout << "reallocate feature buffer dim from " << m_maxDim << " to " << m_Dim << endl;
         m_maxDim = m_Dim;
         for (t=0;t<m_maxT;t++) {
            ResizeArray(m_X[t],m_maxDim);
         }
      }
      */

      // read in cepstrum
      for (t=0;t<m_T;t++) {
         for (d=0;d<cepDim;d++) {
            is.read((char *) &fVal,sizeof(float));
            if (m_NeedSwap) {
               swapLong((char *) &fVal);
            }
            m_X[t][d] = float(fVal);
         }
      }

      // E-Normalization
      if (m_eNormalize)
         ENormalize(m_X,m_T,m_energyPos,m_energyOffset);

      // CMS
      if (m_cms)
         CMS(m_X,m_T,m_staticCepStartPos,cepDim-1);

      // duplicate begin vector
      for (t = half_offset;t < 0; t++) {
         for (d=0;d<cepDim;d++) {
            m_X[t][d] = m_X[0][d];
         }
      }
   
      // duplicate end vector
      for (t = m_T;t < m_T+half_offset; t++) {
         for (d=0;d<cepDim;d++) {
            m_X[t][d] = m_X[m_T-1][d];
         }
      }

      // compute delta cep and acc cep if necessary
      if (m_genDeltaCep) {
         // generate delta (using bell lab config)
         ComputeBellDynamicFeature(m_X,m_T,0,cepDim,deltaW,DELTA_WIN_CONTEXT,false);
         if (m_genAccCep) {
            // generate acc (using bell lab config)
            ComputeBellDynamicFeature(m_X,m_T,cepDim,cepDim,accW,ACC_WIN_CONTEXT,true);
	 }
	 // HTK->bell feature
         //HTKCep39_2_BellCep39(m_X,m_T,m_Dim);
         Bell_Energy_Scaling(m_X,m_T,m_Dim);
      }


   is.close();

   return m_T;
}


long MFCCReader::writeData(const string& filename) {
   ofstream os(filename.c_str(),ios::out|ios::binary);

   if (os.fail()) {
      os.close();
      cout << "Exception: file open error : "+filename;
      exit(-1);
   }

      float fVal;

      m_Dim = m_Header.sampSize / sizeof(float);
      m_T   = m_Header.nSamples;

      cout << "# sample = " << m_Header.nSamples << " ";
      cout << "samp Period = " << m_Header.sampPeriod << " ";
      cout << "sampSize = " << m_Header.sampSize << " ";
      cout << "param_Kind = " << m_Header.paramKind << endl;

      if (m_NeedSwap) {
         swapLong((char *) &m_Header.nSamples);
         swapLong((char *) &m_Header.sampPeriod);
         swapShort((char *) &m_Header.sampSize);
         swapShort((char *) &m_Header.paramKind);
      }

      // read htk header
      os.write((char *)&m_Header,sizeof(m_Header));

      for (long t=0;t<m_T;t++) {
         for (long d=0;d<m_Dim;d++) {
            fVal = float(m_X[t][d]);
#ifdef READHTKFORMAT_DEBUG
	    cout << "writing T = " << t << " D = " << d << " val = " << fVal << endl;
#endif
            if (m_NeedSwap) {
               swapLong((char *) &fVal);
            }
            os.write((char *) &fVal,sizeof(float));
         }
      }

   os.close();

   return m_T;
}

void MFCCReader::setENormalize(bool flag, int energy_pos, float energy_offset)
{
   m_eNormalize = flag;
   m_energyPos  = energy_pos;
   m_energyOffset = energy_offset;
}

void MFCCReader::setCMS(bool flag, int static_cep_pos)
{
   m_cms = flag;
   m_staticCepStartPos = static_cep_pos;
}
