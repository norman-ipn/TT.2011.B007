view: documento.pdf
	evince documento.pdf

documento.pdf: documento.dvi
	dvipdf documento.dvi
documento.dvi: documento.tex capitulo1.tex capitulo2.tex
	latex documento.tex

clean:
	rm *.dvi
	rm *.log
	rm *.aux
	rm *.pdf
