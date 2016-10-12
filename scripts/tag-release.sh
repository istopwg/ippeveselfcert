#!/bin/sh
#
# Tag a release and push it.
#

# Make sure we are running in the right directory...
if test ! -f scripts/tag-release.sh; then
        echo "Run this script from the top-level source directory, e.g.:"
        echo ""
        echo "    scripts/tag-release.sh $*"
        echo ""
        exit 1
fi

# Get the version number...
if test $# = 0; then
        echo "Usage: scripts/tag-release.sh version"
        exit 1
fi

version="$1"

# See if we have local changes (other than this script...)
if (git status | grep -v tag-release.sh | grep -q modified:); then
        echo Local changes remain:
        git status | grep -v tag-release.sh | grep modified:
        exit 1
fi

# Tag and push...
git tag -m "Tag $version" v$version
git push origin v$version
