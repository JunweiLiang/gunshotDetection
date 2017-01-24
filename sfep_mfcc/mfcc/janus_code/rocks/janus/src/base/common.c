/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: windows function mapping
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  windows.c
    Date    :  $Id: common.c 2894 2009-07-27 15:55:07Z metze $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

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

    $Log$
    Revision 4.2  2003/08/14 11:19:50  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.1.2.2.2.2  2003/07/02 17:09:23  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 4.1.2.2.2.1  2003/01/15 14:33:29  metze
    Added MostRecentlyReceivedSignal

    Revision 4.1.2.2  2001/01/25 10:31:36  fuegen
    moved include <fcntl.h> into WINDOWS defs

    Revision 4.1.2.1  2001/01/25 10:26:43  fuegen
    removed some errors on WINDOWS

    Revision 4.1  2001/01/15 09:49:55  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 1.1.2.1  2001/01/12 15:16:50  janus
    necessary changes for running janus under WINDOWS


   ======================================================================== */

#include "common.h"

int MostRecentlyReceivedSignal = 0;

/*
  Great source file - do we need it?
*/

