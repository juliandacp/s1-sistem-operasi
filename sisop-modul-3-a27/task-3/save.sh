#!/bin/bash
zip v_protocol_HSS.zip bzImage rootfs.cpio.gz aha.iso

shopt -s extglob

rm -rf !(kernel.sh|herta.sh|curioDisc.sh|save.sh|v_protocol_HSS.zip)
