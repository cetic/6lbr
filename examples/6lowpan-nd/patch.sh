
DIR=patch

cd ../../
mkdir $DIR

git diff contiki_master -- core/ --stdout > $DIR/core.diff
git diff contiki_master -- regression-tests/ *.yml --stdout > $DIR/regtest.diff

echo "-------- To apply patch --------"
echo "git apply $DIR/core.diff "
echo "git apply $DIR/regtest.diff "