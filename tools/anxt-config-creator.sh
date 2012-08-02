#!/bin/sh
# Copyright (C) 1999, 2002, 2003 Free Software Foundation, Inc.
#                           2008 J. "MUFTI" Scheurich
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This file is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# based on gpg-error-config by g10 Code GmbH

selfdir=`dirname $0`
dir=$selfdir
if test "X_$dir" = "X_."; then
    dir=$PWD
fi
if test $# -gt 1; then
    echo usage: $0 [prefix] 1>&2
    exit 1
fi
if test $# -eq 1; then
    dir=$1    
fi

cat << EOS
#!/bin/sh
# Copyright (C) 1999, 2002, 2003 Free Software Foundation, Inc.
#                           2008 J. "MUFTI" Scheurich
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This file is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# based on gpg-error-config by g10 Code GmbH

EOS

echo version=\"`awk '/CONFIG_VERSION/ {split($0,part,"= ");print part[2]}' $selfdir/../Makefile.config`\"
echo prefix=$dir
echo libs=\"`awk '/CONFIG_LIBS/ {split($0,part,"= ");print part[2]}' $selfdir/../Makefile.config`\"
echo staticlibs=\"`awk '/CONFIG_STATIC_LIBS/ {split($0,part,"= ");print part[2]}' $selfdir/../Makefile.config`\"
echo cflags=\"`awk '/CONFIG_CFLAGS/ {split($0,part,"= ");print part[2]}' $selfdir/../Makefile.config`\"
cat << EOS
exec_prefix=\${prefix}/bin
includedir=\${prefix}/include
libdir=\${prefix}/lib

output=""

usage()
{
    cat <<EOF
Usage: anxt-config [OPTIONS]
Options:
	[--prefix]
	[--exec-prefix]
	[--version]
	[--libs]
        [--static-libs]
	[--cflags]
EOF
    exit \$1
}

if test \$# -eq 0; then
    usage 1 1>&2
fi

while test \$# -gt 0; do
    case "\$1" in
	-*=*)
	    optarg=`echo "\$1" | sed 's/[-_a-zA-Z0-9]*=//'`
	    ;;
	*)
	    optarg=
	    ;;
    esac

    case \$1 in
        --prefix)
	    output="\$output \$prefix"
	    ;;
        --exec-prefix)
	    output="\$output \$exec_prefix"
	    ;;
        --version)
            output=\$version
	    ;;
        --cflags)
	    if test "x\$includedir" != "x/usr/include" -a "x\$includedir" != "x/include"; then
		output="\$output -I\$includedir"
	    fi
	    output="\$output \$cflags"
	    ;;
	--libs)
	    if test "x\$libdir" != "x/usr/lib" -a "x\$libdir" != "x/lib"; then
		output="\$output -L\$libdir"
	    fi
	    output="\$output \$libs"
	    ;;
	--static-libs)
	    if test "x\$libdir" != "x/usr/lib" -a "x\$libdir" != "x/lib"; then
		output="\$output -L\$libdir"
	    fi
	    output="\$output \$staticlibs"
	    ;;
	*)
            usage 1 1>&2
	    ;;
    esac
    shift
done

echo \$output
EOS
