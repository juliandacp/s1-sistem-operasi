#!/bin/bash

timestamp=$(date +%d%m%Y_%H%M%S)

zip -q osboot/backup_$timestamp.zip \
    osboot/bzImage \
    osboot/single.gz \
    osboot/multi.gz \
    osboot/mylinux.iso

echo "backup_$timestamp.zip"