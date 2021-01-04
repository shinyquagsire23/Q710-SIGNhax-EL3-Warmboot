#!/bin/bash
python3 genstuff.py
sudo python3 lglaf.py --rawshell --cr -c 'opentest'
sudo python3 lglaf.py --rawshell --cr -c 'signtest'
sudo python3 lglaf.py --rawshell --cr -c 'signtest-gen'
echo Write A
sudo python3 lglaf.py --rawshell --cr -c 'writetest-gen'
echo Write B
sudo python3 lglaf.py --rawshell --cr -c 'writetest-gen-2'
