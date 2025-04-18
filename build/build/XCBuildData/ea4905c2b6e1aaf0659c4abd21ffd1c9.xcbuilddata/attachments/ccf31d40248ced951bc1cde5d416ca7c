#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/bss/Code/projects/max-externals/build/source/bss/helloworld
  cp /Users/bss/Code/projects/max-externals/source/max-sdk-base/script/PkgInfo /Users/bss/Code/projects/max-externals/source/bss/helloworld/../../../externals/helloworld.mxo/Contents/PkgInfo
  cd /Users/bss/Code/projects/max-externals/build/source/bss/helloworld
  codesign -s - -f --deep /Users/bss/Code/projects/max-externals/externals/helloworld.mxo 2>/dev/null
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/bss/Code/projects/max-externals/build/source/bss/helloworld
  cp /Users/bss/Code/projects/max-externals/source/max-sdk-base/script/PkgInfo /Users/bss/Code/projects/max-externals/source/bss/helloworld/../../../externals/helloworld.mxo/Contents/PkgInfo
  cd /Users/bss/Code/projects/max-externals/build/source/bss/helloworld
  codesign -s - -f --deep /Users/bss/Code/projects/max-externals/externals/helloworld.mxo 2>/dev/null
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/bss/Code/projects/max-externals/build/source/bss/helloworld
  cp /Users/bss/Code/projects/max-externals/source/max-sdk-base/script/PkgInfo /Users/bss/Code/projects/max-externals/source/bss/helloworld/../../../externals/helloworld.mxo/Contents/PkgInfo
  cd /Users/bss/Code/projects/max-externals/build/source/bss/helloworld
  codesign -s - -f --deep /Users/bss/Code/projects/max-externals/externals/MinSizeRel/helloworld.mxo 2>/dev/null
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/bss/Code/projects/max-externals/build/source/bss/helloworld
  cp /Users/bss/Code/projects/max-externals/source/max-sdk-base/script/PkgInfo /Users/bss/Code/projects/max-externals/source/bss/helloworld/../../../externals/helloworld.mxo/Contents/PkgInfo
  cd /Users/bss/Code/projects/max-externals/build/source/bss/helloworld
  codesign -s - -f --deep /Users/bss/Code/projects/max-externals/externals/RelWithDebInfo/helloworld.mxo 2>/dev/null
fi

