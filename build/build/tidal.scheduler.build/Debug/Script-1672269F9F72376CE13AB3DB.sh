#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/bss/Code/projects/max-externals/build/source/bss/tidal.scheduler
  cp /Users/bss/Code/projects/max-externals/source/max-sdk-base/script/PkgInfo /Users/bss/Code/projects/max-externals/source/bss/tidal.scheduler/../../../externals/tidal.scheduler.mxo/Contents/PkgInfo
  cd /Users/bss/Code/projects/max-externals/build/source/bss/tidal.scheduler
  codesign -s - -f --deep /Users/bss/Code/projects/max-externals/externals/tidal.scheduler.mxo 2>/dev/null
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/bss/Code/projects/max-externals/build/source/bss/tidal.scheduler
  cp /Users/bss/Code/projects/max-externals/source/max-sdk-base/script/PkgInfo /Users/bss/Code/projects/max-externals/source/bss/tidal.scheduler/../../../externals/tidal.scheduler.mxo/Contents/PkgInfo
  cd /Users/bss/Code/projects/max-externals/build/source/bss/tidal.scheduler
  codesign -s - -f --deep /Users/bss/Code/projects/max-externals/externals/tidal.scheduler.mxo 2>/dev/null
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/bss/Code/projects/max-externals/build/source/bss/tidal.scheduler
  cp /Users/bss/Code/projects/max-externals/source/max-sdk-base/script/PkgInfo /Users/bss/Code/projects/max-externals/source/bss/tidal.scheduler/../../../externals/tidal.scheduler.mxo/Contents/PkgInfo
  cd /Users/bss/Code/projects/max-externals/build/source/bss/tidal.scheduler
  codesign -s - -f --deep /Users/bss/Code/projects/max-externals/externals/MinSizeRel/tidal.scheduler.mxo 2>/dev/null
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/bss/Code/projects/max-externals/build/source/bss/tidal.scheduler
  cp /Users/bss/Code/projects/max-externals/source/max-sdk-base/script/PkgInfo /Users/bss/Code/projects/max-externals/source/bss/tidal.scheduler/../../../externals/tidal.scheduler.mxo/Contents/PkgInfo
  cd /Users/bss/Code/projects/max-externals/build/source/bss/tidal.scheduler
  codesign -s - -f --deep /Users/bss/Code/projects/max-externals/externals/RelWithDebInfo/tidal.scheduler.mxo 2>/dev/null
fi

