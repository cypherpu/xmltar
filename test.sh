#!/bin/bash

exepath=/home/dbetz/git/xmltar/bazel-bin/xmltar
targetpath=/home/dbetz/git/Utilities
archive=Utilities

if [ ! -f test ]; then
	echo "Creating directory 'test'"
	mkdir test
fi

if [ ! -f test/foo ]; then
	echo "Creating directory 'test/foo'"
	mkdir test/foo
fi

pushd test/foo
rm -f $archive.*.xmltar

echo 'xmltar no precompression, no postcompression'
rm -f $archive.xmltar $archive.tar
$exepath -c -f $archive.xmltar $targetpath &> $archive.xmltar.out
tar cf $archive.tar $targetpath &> $archive.tar.out

echo 'xmltar no precompression, postcompression'
rm -f $archive.xmltar.gz $archive.xmltar.zstd $archive.tar.gz $archive.tar.zstd
$exepath -c -f $archive.xmltar.gz --gzip $targetpath &> $archive.xmltar.gz.out
$exepath -c -f $archive.xmltar.zstd --zstd $targetpath &> $archive.xmltar.zstd.out
tar cf $archive.tar.gz --gzip $targetpath &> $archive.tar.gz.out
tar cf $archive.tar.zstd --zstd $targetpath &> $archive.tar.zstd.out

echo 'xmltar precompression, no postcompression'
rm -f $archive.gz.xmltar $archive.zstd.xmltar
$exepath -c -f $archive.gz.xmltar --file-gzip $targetpath &> $archive.gz.xmltar.out
$exepath -c -f $archive.zstd.xmltar --file-zstd $targetpath &> $archive.zstd.xmltar.out

echo 'xmltar precompression, postcompression'
rm -f $archive.gz.xmltar.gz $archive.zstd.xmltar.zstd
$exepath -c -f $archive.gz.xmltar.gz --file-gzip --gzip $targetpath &> $archive.gz.xmltar.gz.out
$exepath -c -f $archive.zstd.xmltar.zstd --file-zstd --zstd $targetpath &> $archive.zstd.xmltar.zstd.out

#echo 'xmltar no precompression, postcompression'
#rm -f $tarfile.id.16.gz.xmltar.gz
#$basepath/xmltar -c -f $tarfile.id.16.gz.xmltar.gz --base16 --gzip $targetpath &> xmltar.id.16.gz.out
#
#echo 'xmltar precompression, no postcompression'
#rm -f $tarfile.gz.16.id.xmltar
#$basepath/xmltar -c -f $tarfile.gz.16.id.xmltar --base16 --pre-gzip $targetpath &> xmltar.gz.16.id.out
#
#echo 'xmltar precompression, postcompression'
#rm -f $tarfile.gz.16.gz.xmltar.gz
#$basepath/xmltar -c -f $tarfile.gz.16.gz.xmltar.gz --base16 --gzip --pre-gzip $targetpath &> xmltar.gz.16.gz.out
#
#echo 'xmltar mv, no precompression, no postcompression'
#rm -f $tarfile.id.16.id.mv.xmltar.[0-9][0-9]
#$basepath/xmltar -c -f $tarfile.id.16.id.mv.xmltar.%02d --multi-volume --starting-volume=1 --tape-length=100000000 --base16 $targetpath &> xmltar.id.16.id.mv.out
#
#echo 'xmltar mv, no precompression, postcompression'
#rm -f $tarfile.id.16.gz.mv.xmltar.[0-9][0-9].gz
#$basepath/xmltar -c -f $tarfile.id.16.gz.mv.xmltar.%02d.gz --multi-volume --starting-volume=1 --tape-length=100000000 --base16 --gzip $targetpath &> xmltar.id.16.gz.mv.out
#
#echo 'xmltar mv, precompression, no postcompression'
#rm -f $tarfile.gz.16.id.mv.xmltar.[0-9][0-9]
#$basepath/xmltar -c -f $tarfile.gz.16.id.mv.xmltar.%02d --multi-volume --starting-volume=1 --tape-length=100000000 --base16 --pre-gzip $targetpath &> xmltar.gz.16.id.mv.out
#
#echo 'xmltar mv, precompression, postcompression'
#rm -f $tarfile.gz.16.gz.mv.xmltar.[0-9][0-9].gz
#$basepath/xmltar -c -f $tarfile.gz.16.gz.mv.xmltar.%02d.gz --multi-volume --starting-volume=1 --tape-length=100000000 --base16 --gzip --pre-gzip $targetpath &> xmltar.gz.16.gz.mv.out
#
## base64
#
#echo 'xmltar no precompression, no postcompression'
#rm -f $tarfile.id.64.id.xmltar
#$basepath/xmltar -c -f $tarfile.id.64.id.xmltar --base64 $targetpath &> xmltar.id.64.id.out
#
#echo 'xmltar no precompression, postcompression'
#rm -f $tarfile.id.64.gz.xmltar.gz
#$basepath/xmltar -c -f $tarfile.id.64.gz.xmltar.gz --base64 --gzip $targetpath &> xmltar.id.64.gz.out
#
#echo 'xmltar precompression, no postcompression'
#rm -f $tarfile.gz.64.id.xmltar
#$basepath/xmltar -c -f $tarfile.gz.64.id.xmltar --base64 --pre-gzip $targetpath &> xmltar.gz.64.id.out
#
#echo 'xmltar precompression, postcompression'
#rm -f $tarfile.gz.64.gz.xmltar.gz
#$basepath/xmltar -c -f $tarfile.gz.64.gz.xmltar.gz --base64 --gzip --pre-gzip $targetpath &> xmltar.gz.64.gz.out
#
#echo 'xmltar mv, no precompression, no postcompression'
#rm -f $tarfile.id.64.id.mv.xmltar.[0-9][0-9]
#$basepath/xmltar -c -f $tarfile.id.64.id.mv.xmltar.%02d --multi-volume --starting-volume=1 --tape-length=100000000 --base64 $targetpath &> xmltar.id.64.id.mv.out
#
#echo 'xmltar mv, no precompression, postcompression'
#rm -f $tarfile.id.64.gz.mv.xmltar.[0-9][0-9].gz
#$basepath/xmltar -c -f $tarfile.id.64.gz.mv.xmltar.%02d.gz --multi-volume --starting-volume=1 --tape-length=100000000 --base64 --gzip $targetpath &> xmltar.id.64.gz.mv.out
#
#echo 'xmltar mv, precompression, no postcompression'
#rm -f $tarfile.gz.64.id.mv.xmltar.[0-9][0-9]
#$basepath/xmltar -c -f $tarfile.gz.64.id.mv.xmltar.%02d --multi-volume --starting-volume=1 --tape-length=100000000 --base64 --pre-gzip $targetpath &> xmltar.gz.64.id.mv.out
#
#echo 'xmltar mv, precompression, postcompression'
#rm -f $tarfile.gz.64.gz.mv.xmltar.[0-9][0-9].gz
#$basepath/xmltar -c -f $tarfile.gz.64.gz.mv.xmltar.%02d.gz --multi-volume --starting-volume=1 --tape-length=100000000 --base64 --gzip --pre-gzip $targetpath &> xmltar.gz.64.gz.mv.out
#
