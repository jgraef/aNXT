#!/bin/sh
# set -x

VER=`sh ../../bin/anxt-config --version`
VERSION=anxt-$VER
VERSION_ORIG=$VERSION.orig
VERSION_DEBIAN=anxt_$VER

#../../batch/findcopyrights.sh > copyright

rm -rf /tmp/$VERSION /tmp/$VERSION_ORIG

(cd ../../.. && cp -r aNXT /tmp/$VERSION) &&
(cd /tmp/$VERSION && make realclean) &&
(cd ../../.. && cp -r aNXT /tmp/$VERSION_ORIG) &&
(cd /tmp/$VERSION_ORIG && make realclean) &&
(cd .. && tar -cf - debian) | (cd /tmp/$VERSION && tar -xf -) &&
cp control /tmp/$VERSION/debian/control

chmod a+x /tmp/$VERSION/debian/mkinstalldirs
chmod a+x /tmp/$VERSION/debian/postinst
chmod a+x /tmp/$VERSION/debian/prerm
chmod a+x /tmp/$VERSION/debian/rules

cat << EOT > /tmp/$VERSION/debian/changelog
anxt ($VER-1) experimental; urgency=low

  * Version $VER debian package
    For details see the README file of the source package

EOT

DATE=`date -R`
echo ' -- J. "MUFTI" Scheurich <mufti@csv.ica.uni-stuttgart.de>  '$DATE >> /tmp/$VERSION/debian/changelog
echo >> /tmp/$VERSION/debian/changelog

cat changelog >> /tmp/$VERSION/debian/changelog

#cat Makefile.debian_extension >> /tmp/$VERSION/Makefile

gawk -v version=$VERSION '{gsub("/usr/local",version "/debian/tmp/usr");print $0}' ../../Makefile.config > /tmp/$VERSION/Makefile.config

#dpkg-buildpackage -us -uc -sa -rfakeroot
# us = unsigned uc= unsigned changes
(
   cd /tmp/$VERSION && 
   dpkg-buildpackage -us -uc -rfakeroot
) &&
cp /tmp/$VERSION/debian/changelog . &&
rm -rf /tmp/$VERSION &&
echo use \"dpkg -i `echo /tmp/$VERSION_DEBIAN-1_i386.deb`\" to install

