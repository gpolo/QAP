ROOT = artigo
LATEX = latex
EXTRA = dvipdf -o$(ROOT).pdf $(ROOT).dvi

all:
	$(LATEX) $(ROOT)
	bibtex $(ROOT)
	bibtex $(ROOT)
	$(LATEX) $(ROOT)
	$(LATEX) $(ROOT)
	$(LATEX) $(ROOT)
	$(EXTRA)

clean:
	rm -rf *~ *.aux *.dvi *.bbl *.blg *.log *.pdf *.lot *.lof *.toc
