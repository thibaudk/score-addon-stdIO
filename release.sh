#!/bin/bash
rm -rf release
mkdir -p release

cp -rf MyDevice *.{hpp,cpp,txt,json} LICENSE release/

mv release score-addon-my-device
7z a score-addon-my-device.zip score-addon-my-device
