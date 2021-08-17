#!/bin/bash
rm -rf release
mkdir -p release

cp -rf score-addon-stdIO *.{hpp,cpp,txt,json} LICENSE release/

mv release score-addon-stdIO
7z a score-addon-stdIO.zip score-addon-stdIO
