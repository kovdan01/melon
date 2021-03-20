mkdir ~/melon
touch ~/melon/sasldb2
cd ~/melon/
echo "doe" | saslpasswd2 -f sasldb2 -c john
touch localserver.conf
echo -n "pwcheck_method: auxprop
sasldb_path: " > localserver.conf
echo -n $HOME >> localserver.conf
echo -n "/melon/sasldb2" >> localserver.conf
