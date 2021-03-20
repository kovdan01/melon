#!/bin/bash
[ ! -d ~/.melon ] && mkdir ~/.melon
current_path=`pwd`
touch ~/.melon/sasldb2
cd ~/.melon/
echo "doe" | saslpasswd2 -f sasldb2 -c john
echo -n "pwcheck_method: auxprop
sasldb_path: " > localserver.conf
echo -n $HOME >> localserver.conf
echo -n "/.melon/sasldb2" >> localserver.conf
cd $current_path