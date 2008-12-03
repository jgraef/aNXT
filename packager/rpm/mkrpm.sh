#!/bin/sh
# set -x
#
#    Copyright (C) 2008  J. "MUFTI" Scheurich (IITS Universitaet Stuttgart)
#
#    automatically create rpm package for
#    aNXT - a NXt Toolkit
#    Libraries and tools for LEGO Mindstorms NXT robots
#    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

RPM_SRC_LIST="rpm RPM redhat packages"

unset MKRPM_SRC
for i in $RPM_SRC_LIST ; do 
   if test -d /usr/src/$i/SOURCES ; then
      MKRPM_SRC=/usr/src/$i/SOURCES
   fi
done
if test "$MKRPM_SRC" = "" ; then
   echo -n non of typical rpm source directories 1>&2
   for i in $RPM_SRC_LIST ; do 
       echo -n " " /usr/src/$i/SOURCES 1>&2 
   done
   echo " " found 1>&2 
   echo extend variable RPM_SRC_LIST in $0 for your system 1>&2
   exit 1
fi

UNAME_A=`uname -a`

export RPM_BUILD_ROOT='$RPM_BUILD_ROOT'

VERSION=`../../bin/anxt-config --version`
(
   cd ../../.. && 
   rm -rf /tmp/ANXT-$VERSION && \
   cp -r aNXT /tmp/ANXT-$VERSION && \
   gawk -v version=$VERSION '{gsub("/usr/local","$$RPM_BUILD_ROOT/usr");print $0}' aNXT/Makefile.config > /tmp/ANXT-$VERSION/Makefile.config
   cd /tmp/ANXT-$VERSION && 
   make realclean 
) && \
(cd /tmp && tar -cpf - ANXT-$VERSION) | \
gzip -c > $MKRPM_SRC/ANXT-$VERSION.tar.gz

rm -f /tmp/anxt-$VERSION.spec

cat > /tmp/anxt-$VERSION.spec << EOT 
Summary: a toolkit for LEGO Mindstorms NXT robots
Name: ANXT
Version: $VERSION
Release: 1
#Copyright: GPL
License: GPL
Group: Applications/Multimedia
Source: http://linguistix.iits.uni-stuttgart.de/mediawiki/images/0/0e/ANXT-$VERSION.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

%description
aNXT - a NXt Toolkit
 Libraries and tools for LEGO Mindstorms NXT robots
 aNXT implement most of the LEGO(R) Mindstorms(R) NXT direct commands 
 and commands for the LEGO(R) Mindstorms(R) NXT communication protocol
 in C as a library. This commands allow to steer a NXT brick or transfer 
 data to a NXT brick via USB or bluetooth.
 Additionally, there are commandline tools for up/downloading files, 
 starting/stopping .rxe programs, get values from sensors and steer motors.
%prep
%setup -q

%build
make RPM_OPT_FLAGS="$RPM_OPT_FLAGS" 

%install
rm -rf '$RPM_BUILD_ROOT'
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/lib
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man8
mkdir -p $RPM_BUILD_ROOT/usr/share/applications
mkdir -p $RPM_BUILD_ROOT/usr/share/icons/Bluecurve/48x48/apps
mkdir -p $RPM_BUILD_ROOT/usr/share/pixmaps/
EOT
(
cd /tmp/ANXT-$VERSION && make -n installfiles >> /tmp/anxt-$VERSION.spec
)
cat >> /tmp/anxt-$VERSION.spec << EOT 
install -m 644 desktop/nxt_pilot.desktop $RPM_BUILD_ROOT/usr/share/applications/nxt_pilot.desktop
install -m 644 desktop/nxt_pilot.png $RPM_BUILD_ROOT/usr/share/icons/Bluecurve/48x48/apps/nxt_pilot.png
install -m 644 desktop/nxt_pilot.png $RPM_BUILD_ROOT/usr/share/pixmaps/

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root)
%doc README doc

EOT

(
   cd ../.. &&
   for i in bin/* ; do
      echo /usr/$i >> /tmp/anxt-$VERSION.spec
   done
   for i in lib/* ; do
      echo /usr/$i >> /tmp/anxt-$VERSION.spec
   done
   find include -name '*.h' -print | awk '{print "/usr/" $0}' >> /tmp/anxt-$VERSION.spec
   cd doc
   for i in man/man[18]/* ; do
      echo /usr/$i.gz >> /tmp/anxt-$VERSION.spec
   done
)
echo /usr/share/applications/nxt_pilot.desktop >> /tmp/anxt-$VERSION.spec
echo /usr/share/icons/Bluecurve/48x48/apps/nxt_pilot.png >> /tmp/anxt-$VERSION.spec
echo /usr/share/pixmaps/nxt_pilot.png >> /tmp/anxt-$VERSION.spec

if rpm -ba /tmp/anxt-$VERSION.spec ; then
   echo > /dev/null
else
   rpmbuild -ba /tmp/anxt-$VERSION.spec
fi
