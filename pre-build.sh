#!/bin/sh

/usr/bin/rsync --checksum  -r ../Piecewise/src/common/Geometry src/common/Generated
/usr/bin/rsync --checksum  -r ../Process/src/common/Process src/common/Generated
/usr/bin/rsync --checksum  -r ../Transform/src/common/Transform src/common/Generated
/usr/bin/rsync --checksum  -r ../Utilities/src/common/Utilities src/common/Generated
/usr/bin/rsync --checksum  -r ../XmlParser/src/common/XmlParser src/common/Generated

