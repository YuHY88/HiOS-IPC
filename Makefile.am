## Process this file with automake to produce Makefile.in.

SUBDIRS = lib  @MPLS@ @AAA@ @CES@ @ROUTE@ @QOS@ @SYSLOG@ @FILEM@ @NTP@ @FTM@ @IFM@ @OSPFD@ @OSPF6D@  @RIPD@ \
         @VTYSH@ @pkgsrcdir@ @IPMC@\
         tests @DHCP@  @L2@ @DEVM@ @PING@ @SYSTEM@ @ISISD@ @BGPD@ @ALARM@ @CLOCK@ \
         @SDHMGT@ @SDHVCG@ @RMOND@ @STATIS@

DIST_SUBDIRS = lib ifm isisd ospfd ospf6d ripd \
	        vtysh pkgsrc tests \
	        mpls ces route qos ftm syslog isisd ntp dhcp \
	        l2 filem devm ping aaa system bgpd alarm clock ipmc \
	        sdhmgt sdhvcg rmond statis

EXTRA_DIST = aclocal.m4 SERVICES TODO REPORTING-BUGS INSTALL.quagga.txt \
	update-autotools \
	vtysh/Makefile.in vtysh/Makefile.am \
	tools/rrcheck.pl tools/rrlookup.pl tools/zc.pl \
	tools/zebra.el tools/multiple-bgpd.sh \
	fpm/fpm.h

if HAVE_LATEX

HACKING.pdf: HACKING.tex
	$(LATEXMK) -pdf $<

clean-local:
	-$(LATEXMK) -C HACKING.tex

endif

ACLOCAL_AMFLAGS = -I m4
