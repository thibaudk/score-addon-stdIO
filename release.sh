#!/bin/bash
rm -rf release
mkdir -p release

cp -rf score-addon-unix-shell *.{hpp,cpp,txt,json} LICENSE release/

mv release score-addon-score-addon-unix-shell
7z a score-addon-score-addon-unix-shell.zip score-addon-score-addon-unix-shell
