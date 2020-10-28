# Example usage:

```
# sudo ./main
Manufacturer: C*Core
Issuer: XXXX
Label: UKEY
SerialNumber: 3446534887A40B31AA550000AA550000
original data:
0x5f 0xe9 0x7c 0xcd 0x58 0xfe 0xd7 0xab 0x41 0xf7 0x1e 0xfb 0xfd 0xe7 0xe1 0x46 
encrypted data:
0x56 0xda 0x23 0xe2 0x5f 0xa7 0xcd 0x82 0x5d 0x51 0xc2 0x20 0xf5 0x98 0x09 0x0b 
decrypted data:
0x5f 0xe9 0x7c 0xcd 0x58 0xfe 0xd7 0xab 0x41 0xf7 0x1e 0xfb 0xfd 0xe7 0xe1 0x46 
sym enc and dec succ!
```

# Example usage of GmSSL SKF tool:
```
# sudo gmssl skf -lib ./libskf.so -vendor wisec -listdevs
  Device 0 : sdb
# DEV="sdb"
# sudo gmssl skf -lib ./libskf.so -dev $DEV -devinfo
Device sdb :
  Device State     : Present
  Version          : 1.1
  Manufacturer     : C*Core
  Issuer           : XXXX
  Label            : UKEY
  Serial Number    : 33:34:34:36:35:33:34:38:38:37:41:34:30:42:33:31:41:41:35:35:30:30:30:30:41:41:35:35:30:30:30:30:01
  Firmware Version : 1.0
  Ciphers          : sm1-ecb,sm1-cbc,cbcmac-sm1,ssf33-ecb,ssf33-cbc,cbcmac-ssf33,sms4-ecb,sms4-cbc,cbcmac-sms4
  Public Keys      : rsa,rsaEncryption,sm2sign,sm2exchange,sm2encrypt
  Digests          : sm3,sha1,sha256
  Auth Cipher      : sm1-ecb
  Total Sapce      : 122880
  Free Space       : 106560
  MAX ECC Input    : 1920
  MAX Cipher Input : 16256
```
