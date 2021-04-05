#!/bin/bash
path_to_configs=$HOME/.melon
[ ! -d $path_to_configs ] && mkdir $path_to_configs
current_path=`pwd`
cd $path_to_configs
echo "doe" | saslpasswd2 -f $path_to_configs/sasldb2 -c -p john
echo "pwcheck_method: auxprop"                > localserver.conf
echo "auxprop_plugin: sasldb"                >> localserver.conf
echo "sasldb_path: $path_to_configs/sasldb2" >> localserver.conf
cd $current_path
