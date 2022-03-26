#!/usr/bin/env bash
TYPE='rsa:2048'
KEY='ssl.key'
CSR='ssl.csr'
CONFIG='config.conf'
CERT='ssl.pem'

#GEN: key and csr
openssl req -newkey $TYPE -nodes -keyout $KEY -out $CSR -config $CONFIG -batch

#GEN: cert
openssl x509 -signkey $KEY -in $CSR -req -days 365 -out  $CERT
