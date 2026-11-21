texfile = VenegasA202273104
bibfile = Referencias.bib

documento: $(texfile).tex $(bibfile)
	pdflatex $(texfile).tex
	bibtex $(texfile)
	pdflatex $(texfile).tex
	pdflatex $(texfile).tex

clean:
	rm -f *~ $(texfile).aux $(texfile).bbl $(texfile).blg $(texfile).log $(texfile).out $(texfile).fls $(texfile).fdb_latexmk $(texfile).synctex.gz
