/* 
 * itfMain.c --
 *
 *	This file contains a generic main program for Tk-based applications.
 *	It can be used as-is for many applications, just by supplying a
 *	different appInitProc procedure for each specific application.
 *	Or, it can be used as a template for creating new main programs
 *	for Tk applications.
 *
 * Copyright (c) 1990-1994 The Regents of the University of California.
 * Copyright (c) 1994-1996 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tkMain.c 1.150 96/09/05 18:42:25
 */

#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <tcl.h>
#ifndef DISABLE_TK
#include <tk.h>
#endif
#ifdef NO_STDLIB_H
#   include "../compat/stdlib.h"
#else
#   include <stdlib.h>
#endif

#ifndef DISABLE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

/* to avoid to include itf.h */
extern void itfDestroyAllObjects (void);


/*
 * Declarations for various library procedures and variables (don't want
 * to include tkInt.h or tkPort.h here, because people might copy this
 * file out of the Tk source directory to make their own modified versions).
 * Note: don't declare "exit" here even though a declaration is really
 * needed, because it will conflict with a declaration elsewhere on
 * some systems.
 */

extern int		isatty _ANSI_ARGS_((int fd));
/* extern int		read _ANSI_ARGS_((int fd, char *buf, size_t size)); */
extern char *		strrchr _ANSI_ARGS_((CONST char *string, int c));

#ifndef DISABLE_READLINE
void                    Tk_DoNothing _ANSI_ARGS_((ClientData clientData, int mask));
#ifndef DARWIN
static int              Tcl_rl_DoOneEvent ();
#endif
#endif

/*
 * Global variables used by the main program:
 */

static Tcl_Interp *interp;	/* Interpreter for this application. */
static Tcl_DString command;	/* Used to assemble lines of terminal input
				 * into Tcl commands. */
static Tcl_DString line;	/* Used to read the next line from the
                                 * terminal input. */
static int tty;			/* Non-zero means standard input is a
				 * terminal-like device.  Zero means it's
				 * a file. */

/*
 * Forward declarations for procedures defined later in this file.
 */

static void		Prompt _ANSI_ARGS_((Tcl_Interp *interp, int partial));
#ifndef DISABLE_READLINE
static void		StdinProc _ANSI_ARGS_((ClientData clientData,
			    int mask));
#endif

/*
 *----------------------------------------------------------------------
 *
 * Itf_Main --
 *
 *	Main program for Wish and most other Tk-based applications.
 *
 * Results:
 *	None. This procedure never returns (it exits the process when
 *	it's done.
 *
 * Side effects:
 *	This procedure initializes the Tk world and then starts
 *	interpreting commands;  almost anything could happen, depending
 *	on the script being interpreted.
 *
 *----------------------------------------------------------------------
 */

void
Itf_Main(argc, argv, appInitProc)
    int argc;				/* Number of arguments. */
    char **argv;			/* Array of argument strings. */
    Tcl_AppInitProc *appInitProc;	/* Application-specific initialization
					 * procedure to call after most
					 * initialization but before starting
					 * to execute commands. */
{
    char *args, *fileName;
    char buf[20];
    int code;
    size_t length;
    Tcl_Channel inChannel, outChannel, errChannel;

    Tcl_FindExecutable(argv[0]);
    interp = Tcl_CreateInterp();
#ifdef TCL_MEM_DEBUG
    Tcl_InitMemory(interp);
#endif

    /*
     * Parse command-line arguments.  A leading "-file" argument is
     * ignored (a historical relic from the distant past).  If the
     * next argument doesn't start with a "-" then strip it off and
     * use it as the name of a script file to process.
     */

    fileName = NULL;
    if (argc > 1) {
	length = strlen(argv[1]);
	if ((length >= 2) && (strncmp(argv[1], "-file", length) == 0)) {
	    argc--;
	    argv++;
	}
    }
    if ((argc > 1) && (argv[1][0] != '-')) {
	fileName = argv[1];
	argc--;
	argv++;
    }

    /*
     * Make command-line arguments available in the Tcl variables "argc"
     * and "argv".
     */

    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    sprintf(buf, "%d", argc-1);
    Tcl_SetVar(interp, "argc", buf, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", (fileName != NULL) ? fileName : argv[0],
	    TCL_GLOBAL_ONLY);

    /*
     * Set the "tcl_interactive" variable.
     */

    /*
     * For now, under Windows, we assume we are not running as a console mode
     * app, so we need to use the GUI console.  In order to enable this, we
     * always claim to be running on a tty.  This probably isn't the right
     * way to do it.
     */

    /* (fuegen) I've disabled this because you can define this in debug options
#ifdef __WIN32__
    tty = 1;
#else
    tty = isatty(0);
#endif
    */

    tty = isatty(0);

    Tcl_SetVar(interp, "tcl_interactive",
	    ((fileName == NULL) && tty) ? "1" : "0", TCL_GLOBAL_ONLY);

    /*
     * Invoke application-specific initialization.
     */

    if ((*appInitProc)(interp) != TCL_OK) {
	errChannel = Tcl_GetStdChannel(TCL_STDERR);
	if (errChannel) {
            Tcl_Write(errChannel,
		    "application-specific initialization failed: ", -1);
            //Tcl_Write(errChannel, interp->result, -1);
	    Tcl_Write (errChannel, Tcl_GetStringResult (interp), -1);
            Tcl_Write(errChannel, "\n", 1);
        }
    }

    /*
     * Invoke the script specified on the command line, if any.
     */

    if (fileName != NULL) {
	code = Tcl_EvalFile(interp, fileName);
	if (code != TCL_OK) {
	    goto error;
	}
	tty = 0;
    } else {

	/*
	 * Evaluate the .rc file, if one has been specified.
	 */

	Tcl_SourceRCFile(interp);

	/*
	 * Establish a channel handler for stdin.
	 */

	inChannel = Tcl_GetStdChannel(TCL_STDIN);
#ifndef DISABLE_READLINE
	/* stueker: I disabled this, because it lead to an endless loop for the Tcl_rl_DoOneEvent,
	   because it constantly throws FileEvents when doing copy and paste into the shell. */
	/*	
	if (inChannel) {
            Tcl_CreateChannelHandler(inChannel, TCL_READABLE, Tk_DoNothing,
                    (ClientData) inChannel);
        }
	*/
#else
        if (inChannel) {
            Tcl_CreateChannelHandler(inChannel, TCL_READABLE, StdinProc,
                    (ClientData) inChannel);
        }
        if (tty) {
            Prompt(interp, 0);
        }
#endif
    }

    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
    if (outChannel) {
	Tcl_Flush(outChannel);
    }
    Tcl_DStringInit(&command);
    Tcl_DStringInit(&line);
    Tcl_ResetResult(interp);

    /*
     * Loop infinitely, waiting for commands to execute.  When there
     * are no windows left, Tk_MainLoop returns and we exit.
     */

#ifndef DISABLE_READLINE
    if (tty){
        int count, gotPartial = 0;
        char *cmd, *input;
	input = NULL;

#ifndef DISABLE_TK
        Tk_DoOneEvent  (TK_ALL_EVENTS  | TK_DONT_WAIT);
#else
        Tcl_DoOneEvent (TCL_ALL_EVENTS | TCL_DONT_WAIT);
#endif
#ifndef DARWIN
        rl_event_hook = Tcl_rl_DoOneEvent;
#endif
        
        while (1) {
        prompt:
	  if (input) {   /* 'input' is allocated by malloc in the libreadline and thus needs to be freed. See libreadline documentation for details. */
	    free(input);
	    input = (char *)NULL;
	  }
	  input = readline (gotPartial ? "> " : "% ");

          if (input){
	    count = strlen (input);
	    add_history (input);
	    cmd = Tcl_DStringAppend (&command, input, count);

	    if (!Tcl_CommandComplete (cmd)){
	      cmd = Tcl_DStringAppend (&command, "\n",  1);
	      gotPartial = 1;
	      goto prompt;
	    }
	    gotPartial = 0;
	    code = Tcl_RecordAndEval (interp, cmd, 0);
	    Tcl_DStringFree (&command);
	    //if (*interp->result != 0) {
	    if (strlen (Tcl_GetStringResult (interp))) {
	      if ((code != TCL_OK) || (tty)){
		//printf ("%s\n", interp->result);
		printf ("%s\n", Tcl_GetStringResult (interp));
	      }
	    }
	  } else {
	    if (!gotPartial){
	      if (tty){
		Tcl_Eval (interp, "exit");
		exit (1);
	      } else {
#ifndef DISABLE_TK
		Tk_DeleteFileHandler (0);
#else
		Tcl_DeleteFileHandler (0);
#endif
	      }
	      return;
	    } 
	  }
        }
    }
#endif

#ifndef DISABLE_TK
    Tk_MainLoop();
#else
    Tcl_DoOneEvent (TCL_ALL_EVENTS | TCL_DONT_WAIT);
#endif
    Tcl_DeleteInterp(interp);
    Tcl_Exit(0);

error:
    /*
     * The following statement guarantees that the errorInfo
     * variable is set properly.
     */

    Tcl_AddErrorInfo(interp, "");
    errChannel = Tcl_GetStdChannel(TCL_STDERR);
    if (errChannel) {
        Tcl_Write(errChannel, Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY),
		-1);
        Tcl_Write(errChannel, "\n", 1);
	Tcl_Flush(errChannel);
    }
    itfDestroyAllObjects ();
    /* Tcl_DeleteInterp(interp); */
    abort();
}

/*
 *----------------------------------------------------------------------
 *
 * StdinProc --
 *
 *	This procedure is invoked by the event dispatcher whenever
 *	standard input becomes readable.  It grabs the next line of
 *	input characters, adds them to a command being assembled, and
 *	executes the command if it's complete.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Could be almost arbitrary, depending on the command that's
 *	typed.
 *
 *----------------------------------------------------------------------
 */
#ifndef DISABLE_READLINE
    /* ARGSUSED */
static void
StdinProc(clientData, mask)
    ClientData clientData;		/* Not used. */
    int mask;				/* Not used. */
{
    static int gotPartial = 0;
    char *cmd;
    int code, count;
    Tcl_Channel chan = (Tcl_Channel) clientData;

    count = Tcl_Gets(chan, &line);

    if (count < 0) {
	if (!gotPartial) {
	    if (tty) {
		Tcl_Exit(0);
	    } else {
		Tcl_DeleteChannelHandler(chan, StdinProc, (ClientData) chan);
	    }
	    return;
	} else {
	    count = 0;
	}
    }

    (void) Tcl_DStringAppend(&command, Tcl_DStringValue(&line), -1);
    cmd = Tcl_DStringAppend(&command, "\n", -1);
    Tcl_DStringFree(&line);
    
    if (!Tcl_CommandComplete(cmd)) {
        gotPartial = 1;
        goto prompt;
    }
    gotPartial = 0;

    /*
     * Disable the stdin channel handler while evaluating the command;
     * otherwise if the command re-enters the event loop we might
     * process commands from stdin before the current command is
     * finished.  Among other things, this will trash the text of the
     * command being evaluated.
     */

    Tcl_CreateChannelHandler(chan, 0, StdinProc, (ClientData) chan);
    code = Tcl_RecordAndEval(interp, cmd, TCL_EVAL_GLOBAL);
    Tcl_CreateChannelHandler(chan, TCL_READABLE, StdinProc,
	    (ClientData) chan);
    Tcl_DStringFree(&command);
    //if (*interp->result != 0) {
    if (strlen (Tcl_GetStringResult (interp))) {
	if ((code != TCL_OK) || (tty)) {
	    /*
	     * The statement below used to call "printf", but that resulted
	     * in core dumps under Solaris 2.3 if the result was very long.
             *
             * NOTE: This probably will not work under Windows either.
	     */

	    //puts(interp->result);
	  puts (Tcl_GetStringResult (interp));
	}
    }

    /*
     * Output a prompt.
     */

    prompt:
    if (tty) {
	Prompt(interp, gotPartial);
    }
    Tcl_ResetResult(interp);
}
#endif

/*
 *----------------------------------------------------------------------
 *
 * Prompt --
 *
 *	Issue a prompt on standard output, or invoke a script
 *	to issue the prompt.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A prompt gets output, and a Tcl script may be evaluated
 *	in interp.
 *
 *----------------------------------------------------------------------
 */

static void
Prompt(interp, partial)
    Tcl_Interp *interp;			/* Interpreter to use for prompting. */
    int partial;			/* Non-zero means there already
					 * exists a partial command, so use
					 * the secondary prompt. */
{
#ifdef USE_COMPAT_CONST
    CONST char *promptCmd;
#else
    char *promptCmd;
#endif
    int code;
    Tcl_Channel outChannel, errChannel;

    errChannel = Tcl_GetChannel(interp, "stderr", NULL);

    promptCmd = Tcl_GetVar(interp,
    	partial ? "tcl_prompt2" : "tcl_prompt1", TCL_GLOBAL_ONLY);
    if (promptCmd == NULL) {
defaultPrompt:
	if (!partial) {

            /*
             * We must check that outChannel is a real channel - it
             * is possible that someone has transferred stdout out of
             * this interpreter with "interp transfer".
             */

	    outChannel = Tcl_GetChannel(interp, "stdout", NULL);
            if (outChannel != (Tcl_Channel) NULL) {
                Tcl_Write(outChannel, "% ", 2);
            }
	}
    } else {
	code = Tcl_Eval(interp, (char*) promptCmd);
	if (code != TCL_OK) {
	    Tcl_AddErrorInfo(interp,
		    "\n    (script that generates prompt)");
            /*
             * We must check that errChannel is a real channel - it
             * is possible that someone has transferred stderr out of
             * this interpreter with "interp transfer".
             */
            
	    errChannel = Tcl_GetChannel(interp, "stderr", NULL);
            if (errChannel != (Tcl_Channel) NULL) {
	        //Tcl_Write(errChannel, interp->result, -1);
	        Tcl_Write (errChannel, Tcl_GetStringResult (interp), -1);
                Tcl_Write(errChannel, "\n", 1);
            }
	    goto defaultPrompt;
	}
    }
    outChannel = Tcl_GetChannel(interp, "stdout", NULL);
    if (outChannel != (Tcl_Channel) NULL) {
        Tcl_Flush(outChannel);
    }
}

#ifndef DISABLE_READLINE
#ifndef DARWIN
/*
 *----------------------------------------------------------------------
 *
 * Tcl_rl_DoOneEvent:
 *
 *      This routine is called from readline in the idle time.
 *      Calls Tcl's Tk_DoOneEvent.
 *----------------------------------------------------------------------
 */
static int Tcl_rl_DoOneEvent () {

#ifndef WINDOWS
  /* Process all current events, if any */
  while (Tcl_DoOneEvent (TCL_DONT_WAIT));
  /* The problem here is that Darwin has no mechanism like 
     int rl_set_keyboard_input_timeout (int u)
     and constantly seems to call Tcl_rl_DoOneEvent, sucking up CPU ... */
  usleep (10000);
#endif /* WINDOWS */

  return 0;
}
#endif
#endif
