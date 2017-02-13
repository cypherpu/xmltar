#!/bin/bash

basepath=/home/dbetz/git/xmltar
targetpath=/home/dbetz/git/xmltar/src
tarfile=src
# base 16

echo 'xmltar no precompression, no postcompression'
rm -f $tarfile.id.16.id.xmltar
$basepath/xmltar -c -f $tarfile.id.16.id.xmltar --base16 $targetpath &> $tarfile.id.16.id.out

echo 'xmltar no precompression, postcompression'
rm -f $tarfile.id.16.gz.xmltar.gz
$basepath/xmltar -c -f $tarfile.id.16.gz.xmltar.gz --base16 --gzip $targetpath &> xmltar.id.16.gz.out

echo 'xmltar precompression, no postcompression'
rm -f $tarfile.gz.16.id.xmltar
$basepath/xmltar -c -f $tarfile.gz.16.id.xmltar --base16 --pre-gzip $targetpath &> xmltar.gz.16.id.out

echo 'xmltar precompression, postcompression'
rm -f $tarfile.gz.16.gz.xmltar.gz
$basepath/xmltar -c -f $tarfile.gz.16.gz.xmltar.gz --base16 --gzip --pre-gzip $targetpath &> xmltar.gz.16.gz.out

echo 'xmltar mv, no precompression, no postcompression'
rm -f $tarfile.id.16.id.mv.xmltar.[0-9][0-9]
$basepath/xmltar -c -f $tarfile.id.16.id.mv.xmltar.%02d --multi-volume --starting-volume=1 --tape-length=100000000 --base16 $targetpath &> xmltar.id.16.id.mv.out

echo 'xmltar mv, no precompression, postcompression'
rm -f $tarfile.id.16.gz.mv.xmltar.[0-9][0-9].gz
$basepath/xmltar -c -f $tarfile.id.16.gz.mv.xmltar.%02d.gz --multi-volume --starting-volume=1 --tape-length=100000000 --base16 --gzip $targetpath &> xmltar.id.16.gz.mv.out

echo 'xmltar mv, precompression, no postcompression'
rm -f $tarfile.gz.16.id.mv.xmltar.[0-9][0-9]
$basepath/xmltar -c -f $tarfile.gz.16.id.mv.xmltar.%02d --multi-volume --starting-volume=1 --tape-length=100000000 --base16 --pre-gzip $targetpath &> xmltar.gz.16.id.mv.out

echo 'xmltar mv, precompression, postcompression'
rm -f $tarfile.gz.16.gz.mv.xmltar.[0-9][0-9].gz
$basepath/xmltar -c -f $tarfile.gz.16.gz.mv.xmltar.%02d.gz --multi-volume --starting-volume=1 --tape-length=100000000 --base16 --gzip --pre-gzip $targetpath &> xmltar.gz.16.gz.mv.out

# base64

echo 'xmltar no precompression, no postcompression'
rm -f $tarfile.id.64.id.xmltar
$basepath/xmltar -c -f $tarfile.id.64.id.xmltar --base64 $targetpath &> xmltar.id.64.id.out

echo 'xmltar no precompression, postcompression'
rm -f $tarfile.id.64.gz.xmltar.gz
$basepath/xmltar -c -f $tarfile.id.64.gz.xmltar.gz --base64 --gzip $targetpath &> xmltar.id.64.gz.out

echo 'xmltar precompression, no postcompression'
rm -f $tarfile.gz.64.id.xmltar
$basepath/xmltar -c -f $tarfile.gz.64.id.xmltar --base64 --pre-gzip $targetpath &> xmltar.gz.64.id.out

echo 'xmltar precompression, postcompression'
rm -f $tarfile.gz.64.gz.xmltar.gz
$basepath/xmltar -c -f $tarfile.gz.64.gz.xmltar.gz --base64 --gzip --pre-gzip $targetpath &> xmltar.gz.64.gz.out

echo 'xmltar mv, no precompression, no postcompression'
rm -f $tarfile.id.64.id.mv.xmltar.[0-9][0-9]
$basepath/xmltar -c -f $tarfile.id.64.id.mv.xmltar.%02d --multi-volume --starting-volume=1 --tape-length=100000000 --base64 $targetpath &> xmltar.id.64.id.mv.out

echo 'xmltar mv, no precompression, postcompression'
rm -f $tarfile.id.64.gz.mv.xmltar.[0-9][0-9].gz
$basepath/xmltar -c -f $tarfile.id.64.gz.mv.xmltar.%02d.gz --multi-volume --starting-volume=1 --tape-length=100000000 --base64 --gzip $targetpath &> xmltar.id.64.gz.mv.out

echo 'xmltar mv, precompression, no postcompression'
rm -f $tarfile.gz.64.id.mv.xmltar.[0-9][0-9]
$basepath/xmltar -c -f $tarfile.gz.64.id.mv.xmltar.%02d --multi-volume --starting-volume=1 --tape-length=100000000 --base64 --pre-gzip $targetpath &> xmltar.gz.64.id.mv.out

echo 'xmltar mv, precompression, postcompression'
rm -f $tarfile.gz.64.gz.mv.xmltar.[0-9][0-9].gz
$basepath/xmltar -c -f $tarfile.gz.64.gz.mv.xmltar.%02d.gz --multi-volume --starting-volume=1 --tape-length=100000000 --base64 --gzip --pre-gzip $targetpath &> xmltar.gz.64.gz.mv.out

