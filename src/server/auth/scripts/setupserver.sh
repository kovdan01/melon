#!/bin/bash
path_to_configs=$HOME/.melon
[ ! -d $path_to_configs ] && mkdir $path_to_configs
current_path=`pwd`
cd $path_to_configs
touch sasldb2
echo "doe" | saslpasswd2 -f $path_to_configs/sasldb2 -c -p john
echo -n "pwcheck_method: auxprop
sasldb_path: " > localserver.conf
echo -n $path_to_configs/sasldb2 >> localserver.conf
cd $current_path
