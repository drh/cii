#!/bin/sh
# $Id$

set -f
echo Content-type: text/html
echo

cat <<\END
<HTML><HEAD><TITLE>What's Changed?</TITLE></HEAD>
<BODY>
<H1>What's Changed?</H1>
END
cd ../pkg
ver=`pwd`; ver=`basename $ver`
suffix=`echo $ver|tr -d .`
files=`find . -type f -newer /ftp/pub/packages/cii/cii$suffix.tar.gz -print | sed -e '/\.mk/d' -e 's/\.\///g'`
if [ -n "$files" ]; then
	cat <<End
<P>The files below have been updated since the $ver release.
To update your installation,
copy these files to the corresponding locations in your CII $ver directory
hierarchy and rebuild the CII library and sample programs.</P>
End
	echo '<TABLE BORDER="0" CELLPADDING="0">'
	for f in `ls -t $files`; do
		/usr/ucb/echo -n '<TR>'
		ls -l $f|nawk ' { printf "<TD>%s %s %s</TD><TD>&nbsp;</TD><TD><A HREF=\"../pkg/'$f'\">", $6, $7, $8 }'
		case $f in
		*.html)	nawk '/<title>/	{ i=match($0, /<title>.*<\/title>/)
			if (i>0) { printf "%s", substr($0, i+7, RLENGTH-15); exit }
			else printf "'`basename $f`'" }' $f
			;;
		*)	/usr/ucb/echo -n $f
		esac		 
		echo '</A></TD></TR>'
	done
	echo "</TABLE>"
else
	echo "<P>Nothing has changed since the $ver release</P>"
fi
echo "<HR><ADDRESS><SMALL>"`date`"</SMALL><ADDRESS>"
echo "</BODY></HTML>"
exit 0
