nombre = manual.de.usuario

view: ${nombre}.pdf
	evince $<

${nombre}.pdf: ${nombre}.dvi
	dvipdf $<
${nombre}.dvi: ${nombre}.tex
	latex $<
clean:	
	rm ${nombre}.aux
	rm ${nombre}.pdf
	rm ${nombre}.dvi
	rm ${nombre}.log
