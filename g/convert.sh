#!/bin/sh

set +x

for i in *.dot; do
  dot -Tpng ${i} > ${i}.png
done
