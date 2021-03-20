#!/bin/bash
path_to_configs=$HOME/.melon
[ ! -d $path_to_configs ] && mkdir $path_to_configs
current_path=`pwd`
cd $path_to_configs
touchsasldb2
echo "doe" | saslpasswd2 -f sasldb2 -c john
echo -n "pwcheck_method: auxprop
sasldb_path: " > localserver.conf
echo -n $HOME/.melon/sasldb2 >> localserver.conf
cd $current_path