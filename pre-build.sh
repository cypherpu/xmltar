#!/bin/sh

/usr/bin/rsync --checksum  -r --exclude='*Test*' ../Piecewise/src/common/Geometry src/common/Generated
/usr/bin/rsync --checksum  -r --exclude='*Test*' ../Process/src/common/Process src/common/Generated
/usr/bin/rsync --checksum  -r --exclude='*Test*' ../Utilities/src/common/Utilities src/common/Generated
/usr/bin/rsync --checksum  -r --exclude='*Test*' ../XmlParser/src/common/XmlParser src/common/Generated

echo "#define GITHASH \""`/usr/bin/git rev-parse --short HEAD`"\"" >> src/common/githash.hpp
