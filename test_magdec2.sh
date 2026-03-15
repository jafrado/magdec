#!/bin/bash
#
# test_magdec2.sh - Test magdec2 with 13 GPS locations from test.c
#
# Usage: ./test_magdec2.sh [decimal_year]
#   decimal_year defaults to 2025.0 (WMM epoch)
#

YEAR=${1:-2025.0}

echo "magdec2 WMM-2025 test — decimal year $YEAR"
echo "================================================"

./magdec2  25.7907   -80.1300  $YEAR && echo "  ^ Miami Beach, FLA, USA"
./magdec2  40.7128   -74.0059  $YEAR && echo "  ^ New York, NY, USA"
./magdec2  33.9526   -84.5499  $YEAR && echo "  ^ Marietta, GA, USA"
./magdec2  39.7392  -104.9903  $YEAR && echo "  ^ Denver, CO, USA"
./magdec2  40.85    -119.12    $YEAR && echo "  ^ Fresno, CA, USA"
./magdec2  36.52    -120.07    $YEAR && echo "  ^ Blackrock, NV, USA"
./magdec2  21.2850  -157.8357  $YEAR && echo "  ^ Waikiki, Honolulu, HI, USA"
./magdec2  19.8206  -155.4681  $YEAR && echo "  ^ Mauna Kea, HI, USA"
./magdec2   8.7232   167.7361  $YEAR && echo "  ^ Kwajalein, Marshall Islands"
./magdec2  55.3160    -7.3714  $YEAR && echo "  ^ Dochartaigh Castle, Donegal, Ireland"
./magdec2  71.1297    25.8744  $YEAR && echo "  ^ Mageroya, Nordkapp, Norway"
./magdec2 -89.0000     0.0000  $YEAR && echo "  ^ South Pole, Antarctica"
./magdec2  89.0000     0.0000  $YEAR && echo "  ^ North Pole, Arctic Ocean"

echo "================================================"
echo "Done."
