# Microsoft Developer Studio Project File - Name="janus" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=janus - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "janus.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "janus.mak" CFG="janus - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "janus - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "janus - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "janus - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "C:\Programme\Tcl\include" /I "..\..\..\base" /I "..\..\..\features" /I "..\..\..\hmm" /I "..\..\..\ibis" /I "..\..\..\itf" /I "..\..\..\main" /I "..\..\..\models" /I "..\..\..\net" /I "..\..\..\stc" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "WINDOWS" /D "DISABLE_READLINE" /D "IBIS" /D "USE_COMPAT_CONST" /FR /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 tcl84.lib tk84.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"C:\Programme\Tcl\lib"

!ELSEIF  "$(CFG)" == "janus - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Programme\Tcl\include" /I "..\..\..\base" /I "..\..\..\features" /I "..\..\..\hmm" /I "..\..\..\ibis" /I "..\..\..\itf" /I "..\..\..\main" /I "..\..\..\models" /I "..\..\..\net" /I "..\..\..\stc" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "WINDOWS" /D "DISABLE_READLINE" /D "IBIS" /D "USE_COMPAT_CONST" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 tcl84.lib tk84.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"C:\Programme\Tcl\lib"

!ENDIF 

# Begin Target

# Name "janus - Win32 Release"
# Name "janus - Win32 Debug"
# Begin Group "base"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\base\array.c
# End Source File
# Begin Source File

SOURCE=..\..\..\base\array.h
# End Source File
# Begin Source File

SOURCE=..\..\..\base\common.c
# End Source File
# Begin Source File

SOURCE=..\..\..\base\common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\base\dbase.c
# End Source File
# Begin Source File

SOURCE=..\..\..\base\dbase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\base\ffmat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\base\ffmat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\base\list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\base\list.h
# End Source File
# Begin Source File

SOURCE=..\..\..\base\mach_ind_io.c
# End Source File
# Begin Source File

SOURCE=..\..\..\base\mach_ind_io.h
# End Source File
# Begin Source File

SOURCE="..\..\..\base\matrix-io.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\base\matrix.c
# End Source File
# Begin Source File

SOURCE=..\..\..\base\matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\base\melscale.c
# End Source File
# End Group
# Begin Group "features"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\features\feature.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\feature.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureADC.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureADC.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureADCTables.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureAudio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureAudioItf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureDownsample.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureDownsample.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureFFT.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureFFT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureFFTItf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureFilter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureIOItf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureJanus.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureMapItf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureMethodItf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureModalities.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureModalities.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featureNRItf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featurePLP.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featurePLP.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\featurePLPItf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\formants.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\formants.h
# End Source File
# Begin Source File

SOURCE=..\..\..\features\lda.c
# End Source File
# Begin Source File

SOURCE=..\..\..\features\lda.h
# End Source File
# End Group
# Begin Group "hmm"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\hmm\hmm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\hmm\hmm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\hmm\path.c
# End Source File
# Begin Source File

SOURCE=..\..\..\hmm\path.h
# End Source File
# Begin Source File

SOURCE=..\..\..\hmm\textGraph.c
# End Source File
# Begin Source File

SOURCE=..\..\..\hmm\textGraph.h
# End Source File
# Begin Source File

SOURCE=..\..\..\hmm\viterbi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\hmm\vitGlat.c
# End Source File
# End Group
# Begin Group "ibis"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\ibis\bmem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\bmem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfgLexicon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfgParseTree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfgRead.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfgReadJSGF.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfgReadSOUP.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfgRuleStack.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\cfgTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\glat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\glat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\glatConsensus.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\glatGen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\lks.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\lks.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\lmla.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\lmla.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\metaLM.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\metaLM.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\ngramLM.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\ngramLM.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\phmm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\phmm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\phraseLM.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\phraseLM.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\scoreA.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\scoreA.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\sglobal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\slimits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\smem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\smem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\spass.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\spass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\strace.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\strace.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\stree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\stree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\svmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\svmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\svocab.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\svocab.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\xhmm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ibis\xhmm.h
# End Source File
# End Group
# Begin Group "itf"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\itf\error.c
# End Source File
# Begin Source File

SOURCE=..\..\..\itf\error.h
# End Source File
# Begin Source File

SOURCE=..\..\..\itf\itf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\itf\itf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\itf\itfMain.c
# End Source File
# Begin Source File

SOURCE=..\..\..\itf\itfWrapper.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\itf\itfWrapper.h
# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\main\janusInit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\main\janusMain.c
# End Source File
# End Group
# Begin Group "models"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\models\amodel.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\amodel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\bbi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\bbi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\cbmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\cbmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\codebook.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\codebook.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\context.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\context.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\covar.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\covar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\dictionary.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\dictionary.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\distrib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\distrib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\distribStream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\distribStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\duration.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\duration.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\lh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\lh.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\mladapt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\mladapt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\mladaptS.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\mladaptS.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\modalities.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\modalities.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\modelSet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\modelSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\phones.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\phones.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\ptree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\ptree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\questions.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\questions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\rewrite.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\rewrite.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\sample.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\sample.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\score.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\score.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\senones.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\senones.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\snStreamWeights.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\snStreamWeights.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\stream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\stream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\tags.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\tags.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\topo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\topo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\trans.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\trans.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\tree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\tree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\treeTransform.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\treeTransform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\models\word.c
# End Source File
# Begin Source File

SOURCE=..\..\..\models\word.h
# End Source File
# End Group
# Begin Group "net"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\net\classifier.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\net\classifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\net\ffnet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\net\ffnet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\net\gaussClass.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\net\gaussClass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\net\glimClass.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\net\glimClass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\net\hme.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\net\hme.h
# End Source File
# Begin Source File

SOURCE=..\..\..\net\hmeSet.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\net\hmeSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\net\hmeStream.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\net\hmeStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\net\mlpClass.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\net\mlpClass.h
# End Source File
# End Group
# Begin Group "stc"

# PROP Default_Filter ".c .h"
# Begin Source File

SOURCE=..\..\..\stc\cbn_assert.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbn_assert.h
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbn_common.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbn_common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbn_global.h
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbn_parmat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbn_parmat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbnew.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbnew.h
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbnewStream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\cbnewStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\matrix_ext.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stc\matrix_ext.h
# End Source File
# End Group
# End Target
# End Project
