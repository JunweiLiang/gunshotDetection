# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Makefile for Documentation
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze
#  Module  :  Makefile
#  Date    :  $Id: Makefile 3120 2010-02-14 05:22:07Z metze $
#
#  Remarks :  Needed to build the documentation for JRTk/ Ibis
#
#               You can generate documentation in several formats from
#               LaTeX sources:
#		0) dvi
#		1) ps
#		2) pdf
#		3) html
#
#		Some of the documentation (for C-modules and the tcl-lib)
#		is generated automatically from the executable. To make
#		sure the docu is up-to-date, execute the following
#		commands with your current executable ("janus" or "ibis"):
#
#	(i13pc152:/home/metze/janus/doc) ../src/Linux.gcc/janus lib2tex.tcl
#	(i13pc152:/home/metze/janus/doc) ../src/Linux.gcc/janus tcl2tex.tcl
#
#		Now you should be all set to re-create the documentation.
#		The above command produced new *.tex files for modules
#		and tcl-lib: make; make ps; make pdf; make html.
#
#		Have a look at the file "link-errors" which lists
#		unexistant links in the source code. These will show
#		up as "dead hyperlinks" in the pdf and html versions.
#
#		Creating the documentation requires the following
#		LaTeX-related packages to be installed (on SuSE 8.2):
#		te_etex, te_latex, te_pdf, tetex, latex2html, tex4ht
#
# ========================================================================
#
#  $Log$
#  Revision 1.2  2003/08/14 11:18:44  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.5  2003/08/07 12:58:35  metze
#  *** empty log message ***
#
#  Revision 1.1.2.4  2003/06/26 15:08:16  metze
#  Initial changes for V5.0 P013
#
#
# ========================================================================

L2H = latex2html -no_math -html_version 3.2,math -scalable_fonts -white

all:	clean
	./Extramake
	latex      janus-doku
	bibtex     janus-doku
	latex      janus-doku
	makeindex  janus-doku.idx
	latex      janus-doku
	#latex      janus-doku

html:	all
	$(L2H)     janus-doku
	echo       BODY { text-align: justify } >> janus-doku/janus-doku.css
	echo       P    { text-align: justify } >> janus-doku/janus-doku.css

ps:	all
	dvips      -f janus-doku | psbook | psnup -2 | pstops "2:0,1U(21cm,29.7cm)" > janus-book.ps

pdf:	clean
	./Extramake
	pdflatex   janus-doku
	bibtex	   janus-doku
	pdflatex   janus-doku
	makeindex  janus-doku.idx
	pdflatex   janus-doku
	pdflatex   janus-doku | grep reference | sort -u > link-errors
	# thumbpdf   janus-doku
	# pdflatex   janus-doku

clean:
	rm -rf *.aux *.idx *.ilg *.ind *.glo *.gls *.log *.toc *.out *.brf *.bbl *.blg */*.ilg */*.ind *~ */*~
