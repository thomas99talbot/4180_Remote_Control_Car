#!/bin/sh
echo starting raspivid
sudo raspivid -o - -t 30000  -w 800 -h 600 --nopreview -fps 12 | cvlc -vvv stream:///dev/stdin --sout '#rtp{sdp=rtsp://:8080/}' :demux=h264
