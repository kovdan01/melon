# Auth Service Configuration guide

Auth service uses standard SASL configuration system. Server name is set to `localserver`, so the confifuration file is to be called `localserver.conf`.  The location for the file is the same directiory as `libsasl` library `lib/` directory. That's right, you should put it alongside the `.so`'s.

### Configuration example
SASL configurations should work a wide array of possible options for password storage and checking (see "Useful links" section), but the current service was tested with this minimal configuration: 
```
pwcheck_methor: auxprop
sasldb_path: /etc/sasldb2

`sasldb` database option is configured with `saslpasswd2` and `sasldblistusers2` utilities 

```
### Useful links
- [SASL options guide](https://blog.sys4.de/cyrus-sasl-libsasl-man-page-en.html)
- [some more options](https://stuff.mit.edu/afs/sipb/project/imaptools/src/cyrus-sasl-2.1.0/doc/options.html)