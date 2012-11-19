view: documento.pdf
	evince documento.pdf

documento.pdf: documento.dvi
	dvipdf documento.dvi
documento.dvi: documento.tex capitulo1.tex
	latex documento.tex

clean:
	rm documento.pdf
	rm documento.dvi
	rm *.log
	rm *.aux
