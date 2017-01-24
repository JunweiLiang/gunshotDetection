/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Transcription
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  Transcription.c
    Date    :  $Id: Transcription.c 2844 2008-11-25 12:00:00Z wolfel $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    Copyright (C) 1990-1994.   All rights reserved.

    This software is part of the JANUS Speech- to Speech Translation Project

    USAGE PERMITTED ONLY FOR MEMBERS OF THE JANUS PROJECT
    AT CARNEGIE MELLON UNIVERSITY OR AT UNIVERSITAET KARLSRUHE
    AND FOR THIRD PARTIES ONLY UNDER SEPARATE WRITTEN PERMISSION
    BY THE JANUS PROJECT

    It may be copied  only  to members of the JANUS project
    in accordance with the explicit permission to do so
    and  with the  inclusion  of  the  copyright  notices.

    This software  or  any  other duplicates thereof may
    not be copied or otherwise made available to any other person.

    No title to and ownership of the software is hereby transferred.

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#include "Transcription.h"

enum PARSER_STATES 
{
    STATE_UTTERANCE_START,
    STATE_UTTERANCE_SIGNATURE,
    STATE_UTTERANCE_NUMBER,
    STATE_START_TIME,
    STATE_TIME_SEPARATOR,
    STATE_END_TIME,
    STATE_INFO_SEPARATOR,
    STATE_EXPECTING_PHONEME_START,
    STATE_PHONEME,
    STATE_START_FRAME,
    STATE_END_FRAME,
    STATE_FORWARD,
    STATE_SKIP_LINE
};

const char*  STRING_UTTERANCE_SIGNATURE = "Utt_";
const char*  STRING_TIME_SEPARATOR      = "- ";
const char*  STRING_INFO_SEPARATOR = ": ";

static char**  acousticalUnitNames;
static int     nAcousticalUnits;

/*===================================================================================
/  createUtterance
===================================================================================*/
Utterance* createUtterance(int number, double timeStart, double timeEnd)
{
    Utterance* utterance = malloc(sizeof(Utterance));

    (*utterance).number         = number;
    (*utterance).timeStart      = timeStart;
    (*utterance).timeEnd        = timeEnd;
    (*utterance).phonemeList    = createSimpleList();

    return(utterance);
}

/*===================================================================================
/  getPhonemeNumberByName
===================================================================================*/
int getAUNumberByName(char* name)
{
    int i;

    if (name[0]==0) return(-1);
    for (i=0; i<nAcousticalUnits; i++) if (strcmp(acousticalUnitNames[i],name)==0) return(i);

    return(-1);
}

/*===================================================================================
/  createAcousticalUnit
===================================================================================*/
AcousticalUnit* createAcousticalUnit(char* name, int frameStart, int frameEnd)
{
    AcousticalUnit* phoneme = malloc(sizeof(AcousticalUnit));

    (*phoneme).number     = getAUNumberByName(name);
    (*phoneme).frameStart = frameStart;
    (*phoneme).frameEnd   = frameEnd;

    if ((*phoneme).number==-1) {
        printf("[ERROR] UNKNOWN PHONEME \"%s\"\n", name);
        exit(1);
    }

    return(phoneme);
}

/*===================================================================================
/  searchAcousticalUnitFrame
/
/  PARAMETERS
/    item1    :  current AcousticalUnit
/    item2    :  time to compare to AcousticalUnit's time
/  RETURNS
/     0       :  if given time (item2) is within AcousticalUnit's timeframe
/     1       :  if given time (item2) is after AcousticalUnit's timeframe
/    -1       :  if given time (item2) is before AcousticalUnit's timeframe
/    -2       :  ERROR 
===================================================================================*/
int searchAcousticalUnitFrame(void* item1, void* item2)
{
    AcousticalUnit* phoneme1 = (AcousticalUnit*) item1;
    int frame = (*(int*)item2);

    if (phoneme1==0) return(-2);
    if ((*phoneme1).frameEnd<frame) return(1);
    if (((*phoneme1).frameStart<=frame) && (frame<=(*phoneme1).frameEnd)) return(0);
    if ((*phoneme1).frameStart>frame) return(-1);

    return(-2);
}

/*===================================================================================
/  initializeParseInt
===================================================================================*/
int*    intNumber;
char    intTerminator;

void initializeParseInt(int* number, char terminator)
{
    (*number)       = 0;
    intNumber       = number;
    intTerminator   = terminator;
}

/*===================================================================================
/  parseInt
===================================================================================*/
int parseInt(char ch)
{
    if (ch==intTerminator) return(1);

    if ((ch>='0') && (ch<='9')) {
        (*intNumber) = (*intNumber)*10 + (int)(ch-'0');
        return(0);
    }

    return(2);
}

/*===================================================================================
/  initializeReadString
===================================================================================*/
char*       string = 0;
char*       stringTmp = 0;
const int   stringDynAllocSize = 1024;
int         stringLength = 0;
int         maxStringLength = 0;
char        stringTerminator;

void initializeReadString(char terminator)
{
    stringLength        = 0;
    stringTerminator    = terminator;
}

/*===================================================================================
/  readString
===================================================================================*/
int readString(char ch)
{
    if (ch==stringTerminator) return(1);            /* end of string? */
    if (((ch>='A') && (ch<='Z')) || (ch=='+')) {    /* valid char? */
        if (stringLength>=maxStringLength) {        /* time to enlarge? */
            maxStringLength += stringDynAllocSize;
            stringTmp = malloc(maxStringLength);
            memcpy(stringTmp, string, stringLength);
            if (string!=0) free(string);
            string = stringTmp;
        }
        string[stringLength] = ch;                  /* store new char in string */
        stringLength++;
        return(0);
    }

    return(2);                                      /* ohoh mistake */
}

/*===================================================================================
/  obtainString
===================================================================================*/
char* obtainReadString()
{
    stringTmp = malloc(stringLength+1);
    memcpy(stringTmp, string, stringLength);
    stringTmp[stringLength] = 0;                   /* append terminating '0' */
    return(stringTmp);
}

/*===================================================================================
/  parseUtteranceAU (Acoustical Units from JANUS, not phonemes!)
===================================================================================*/
Utterance*      	utterance;
AcousticalUnit* 	phoneme;
int             	parseState;
int             	utteranceNumber;
double          	startTime;
double          	endTime;
char*           	phonemeString = 0;
int             	startFrame;
int             	endFrame;
int             	phonemeStartFrame;
int             	phonemeEndFrame;
double          	phonemeStartTime;
double          	phonemeEndTime;

Utterance* parseUtteranceJANUS(char* utteranceStr)
{
    int     uttPos;
    int     uttLen = strlen(utteranceStr);

    utterance = createUtterance(1, 0, 0);
    parseState = STATE_FORWARD;

    uttPos = 0;
    startFrame = -1; 

    while (uttPos<uttLen) {

        switch (parseState) {

            /*-----------------------------------------------------------------
            /  STATE_FORWARD
            /----------------------------------------------------------------*/
            case STATE_FORWARD:
                switch (utteranceStr[uttPos]) {
                    case ' ':
                        uttPos++;
                    break;
                    case '\r':
                    case '\n':
                        parseState = STATE_SKIP_LINE;
                    break;
                    case '{':
                        uttPos++;
                        initializeReadString(' ');
                        parseState = STATE_PHONEME;
                    break;
                    default:
                        printf("[PARSE ERROR] STATE_FORWARD>\n");
                        printf("    SKIPPING LINE !!!\n");
                        parseState = STATE_SKIP_LINE;
                    break;
                }
            break;

            /*-----------------------------------------------------------------
            /  STATE_PHONEME
            /----------------------------------------------------------------*/
            case STATE_PHONEME:
                switch (readString(utteranceStr[uttPos])) {
                    case 0:
                        uttPos++;
                    break;
                    case 1:
                        uttPos++;
                        phonemeString = obtainReadString();
                        initializeParseInt((int*)&phonemeStartFrame, ' ');
                        parseState = STATE_START_FRAME;
                    break;
                    case 2:
                        printf("[PARSE ERROR] STATE_PHONEME>\n");
                        printf("    SKIPPING LINE !!!\n");
                        parseState = STATE_SKIP_LINE;
                    break;
                }
            break;

            /*-----------------------------------------------------------------
            /  STATE_START_FRAME
            /----------------------------------------------------------------*/
            case STATE_START_FRAME:
                switch (parseInt(utteranceStr[uttPos])) {
                    case 0:
                        uttPos++;
                    break;
                    case 1:
                        uttPos++;
                        if (startFrame<0)
                            startFrame = phonemeStartFrame;
                        initializeParseInt((int*)&phonemeEndFrame, '}');
                        parseState = STATE_END_FRAME;
                    break;
                    case 2:
                        printf("[PARSE ERROR] STATE_START_FRAME>\n");
                        printf("    SKIPPING LINE !!!\n");
                        parseState = STATE_SKIP_LINE;
                    break;
                }
            break;

            /*-----------------------------------------------------------------
            /  STATE_START_FRAME
            /----------------------------------------------------------------*/
            case STATE_END_FRAME:
                switch (parseInt(utteranceStr[uttPos])) {
                    case 0:
                        uttPos++;
                    break;
                    case 1:
                        uttPos++;
                        endFrame = phonemeEndFrame;
                        if (utterance!=0) {
                            phoneme = createAcousticalUnit(phonemeString, phonemeStartFrame, phonemeEndFrame);
                            (*(*utterance).phonemeList).addNode((*utterance).phonemeList, createSimpleNode(phoneme));
                        }
                        parseState = STATE_FORWARD;
                    break;
                    case 2:
                        printf("[PARSE ERROR] STATE_END_FRAME>\n");
                        printf("    SKIPPING LINE !!!\n");
                        parseState = STATE_SKIP_LINE;
                    break;
                }
            break;

            /*-----------------------------------------------------------------
            /  STATE_SKIP_LINE
            /----------------------------------------------------------------*/
            case STATE_SKIP_LINE:
                uttPos++;
            break;

        }
    }

    return(utterance);

}

/*===================================================================================
/  parseUtteranceAU (Acoustical Units from JANUS, not phonemes!)
===================================================================================*/
void initTranscriptionJANUS(char** auNames, int nau)
{
    acousticalUnitNames = auNames;
    nAcousticalUnits = nau;
}
