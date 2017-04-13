#!/bin/sh

# a very hacky script to build and push to gh-pages
# designed to be run from master (or any branch no the gh-pages branch)
#
# This will publish the html docs to the github.io url:
# http://chrisbarker-noaa.github.io/libgd
# this needs to be updated for the "proper" repo

PARENTDIR=../../../
REPONAME=libgd.gh_pages/
GHPAGESDIR=$PARENTDIR$REPONAME

# make sure the gh pages repo is there
if [ ! -d "$GHPAGESDIR" ]; then
	# if it's not there yet, create a clone
    pushd $PARENTDIR
    ## this needs to be changed to the main repo
    #  or, pulled from  .git configuration somehow
    #  or can jsut make the repo by hand first.
    git clone https://github.com/ChrisBarker-NOAA/libgd.git $REPONAME
    popd
fi

# now put it in the right branch
pushd $GHPAGESDIR
git checkout gh-pages
popd

# make the docs
./run_docs.sh

# copy to other repo (on the gh-pages branch)
cp -R html/ $GHPAGESDIR

# commit and push teh updated docs to the gh_pages branch inteh other repo
pushd $GHPAGESDIR
git add * # in case there are new files added
git commit -a -m "updating online docs"
# pull any changes that someone else pushed
#      with "ours" merge so there won't be conflicts
git pull -s ours --no-edit

# push it up to gitHub
git push

