#!/bin/sh

/usr/bin/rsync --checksum  -r ../Bidirectional_Pipe/src/common/Bidirectional_Pipe/ src/common/Generated
/usr/bin/rsync --checksum  -r ../Process/src/common/Process/ src/common/Process
