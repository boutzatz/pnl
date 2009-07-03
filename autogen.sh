#!/bin/sh
# Run this to generate all the initial makefiles, etc.

# This script has been modified to enable the use of a separate
# builddir

builddir=`pwd`
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.
DIE=0

(test -f $srcdir/configure.in) || {
    echo -n "**Error**: Directory "\'$srcdir\'" does not look like the"
    echo " top-level package directory"
    exit 1
}


(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`autoconf' installed."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
}

(grep "^AM_PROG_LIBTOOL" $srcdir/configure.in >/dev/null) && {
  (libtool --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "**Error**: You must have \`libtool' installed."
    echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
    DIE=1
  }
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`automake' installed."
  echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
  NO_AUTOMAKE=yes
}


# if no automake, don't bother testing for aclocal
test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: Missing \`aclocal'.  The version of \`automake'"
  echo "installed doesn't appear recent enough."
  echo "You can get automake from ftp://ftp.gnu.org/pub/gnu/" 
  DIE=1
}

if test "$DIE" -eq 1; then
  exit 1
fi

if test -z "$*"; then
  echo "**Warning**: I am going to run \`configure' with no arguments."
  echo "If you wish to pass any to it, please specify them on the"
  echo \'$0\'" command line."
  echo
fi


cd $srcdir
aclocalinclude="$ACLOCAL_FLAGS"
if grep "^AM_PROG_LIBTOOL" configure.in >/dev/null; then
	if test -z "$NO_LIBTOOLIZE" ; then 
	    echo "Running libtoolize..."
	    libtoolize --force --copy
	fi
fi
echo "Running aclocal  ..."
aclocal
## Do not run autoheader, otherwise config.h.in is polluted with
## some #define that interfere with Nsp.
echo "Running automake --gnu ..."
automake --add-missing --gnu --copy
echo "Running autoconf ..."
autoconf

conf_flags="--enable-maintainer-mode"
##conf_flags="--disable-optimize" 

cd $builddir
echo Running $srcdir/configure $conf_flags "$@" ...
$srcdir/configure $conf_flags "$@" \
  && echo Now type \`make\' to compile. || exit 1
