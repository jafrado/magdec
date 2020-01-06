
#include <stdlib.h>
#include <stdio.h>
#include "magdec.h"



/** 
 * @brief Look up the magnetic compass declination angle at a given
 * latitude/longitude for compass correction
 * 
 *  @param lat the latitude for the location to lookup (decimal degrees)
 *  @param lon the longitude for the location to lookup (decimal degrees)
 *  @param year the year for which the declination angle is needed
 *  @return The magnetic declination angle (in degrees)
 */
float lookup_magdev(float lat, float lon, int year)
{
	int i, n;
	int lati, loni;
	magdec_correction_t* c = NULL;
	
	lati = (int)lat;
	loni = (int)lon;
	
	//printf("lookup: %d,%d\n", lati, loni);

	if (year <= 2019) { 
		c = &corr[0];
		n = n_magdec_entries;
	}
	else { 
		c = &corr_2020[0];
		n = n_magdec_entries_2020;
	}
	
	for (i = 0; i < n; i++){ 
		if ((c[i].lat == lati) && (c[i].lon == loni) && 
		    (c[i].year == year)){
		  // printf("entry %d - %d, %d\n", i, corr[i].lat, corr[i].lon);
			break;
		}
	}
	if (i < n)
		return c[i].magdec;
	else { 
		printf("WARNING: %2.2f, %2.2f not in database for %d,"
		       " correction most likely incorrect\n", lat, lon, year);
		return 0;
	}
}

