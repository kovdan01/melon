#!/bin/bash
path_to_configs=$HOME/.melon
[ ! -d $path_to_configs ] && mkdir $path_to_configs
current_path=`pwd`
cd $path_to_configs
echo "doe" | saslpasswd2 -c -p -a melon -f $path_to_configs/sasldb2 john
echo "pwcheck_method: auxprop"                > melon.conf
echo "auxprop_plugin: sasldb"                >> melon.conf
echo "sasldb_path: $path_to_configs/sasldb2" >> melon.conf
cd $current_path
