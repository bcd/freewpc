
HTDIR = /home/bcd/sw/freewpc/web

install:
	mkdir -p $(HTDIR)
	cp -p index.html faq.html $(HTDIR)

all : index.html faq.html

index.html : index.web mkweb.pl
	perl mkweb.pl < index.web > index.html

faq.html : faq.web mkweb.pl
	perl mkweb.pl < faq.web > faq.html

clean:
	rm -f index.html faq.html
