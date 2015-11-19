#!/bin/sh
# uprev.sh - shell script to update the latest library tags.

git describe --tags > build-aux/.version
