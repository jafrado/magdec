#include <stdlib.h>
#include <stdio.h>
#include "magdec.h"


location_t locations[] = {
	{"Miami Beach", "FLA", "USA", 25.7907, -80.1300},
	{"New York", "NY", "USA", 40.7128, -74.0059},
	{"Marietta", "GA", "USA", 33.9526, -84.5499},
	{"Denver", "CO", "USA", 39.7392, -104.9903},
	{"Fresno", "CA", "USA", 40.85,-119.12},
	{"Blackrock", "NV", "USA", 36.52,-120.07},
	{"Waikiki, Honolulu", "HI", "USA", 21.2850, -157.8357},
	{"Mauna Kea", "HI", "USA", 19.820600, -155.468100},
	{"Kwajalein", "Marshall Islands", "USA", 8.723157, 167.736122},
	{"Dochartaigh Castle", "Donegal", "Ireland", 55.315995, -7.371370},
	{"Mageroya", "Nordkapp", "Norway", 71.129743, 25.874366},
	{"South Pole", "SP", "Antartica", -89, 0},
	{"North Pole", "NP", "Artic Ocean", 89, 0},
	{"Brazil", "SAA", "South America", -14.242915, -53.189266}, /* Least change */
	{"North Magnetic Pole", "NMP", "Arctic Ocean", 86.38, 164.06} /* Most change */

	/* The North Magnetic Pole has migrated into the Russian hemisphere, 
	 * estimated for the 2025-2026 period to be around 86.38°N, 164.06°E. 
	 * 
	 * The pole is moving toward Siberia at a speed of approximately 35-40 kilometers 
	 * (20-25 miles) per year, causing massive changes in magnetic declination in the Arctic region. 
	 */

};
#define _n_locations ((sizeof(locations)/sizeof(locations[0])))
int n_locations = _n_locations;

int years[] = {2026, 2027, 2028, 2029};
#define _n_years ((sizeof(years)/sizeof(years[0])))
int n_years = _n_years;

#ifndef ARDUINO
int main(int argc, char* argv[])
{
	int i, year;
	double variance[100];

	printf("Showing Magnetic declination changes for "
	       "%ld locations over %ld years\n\n", n_locations, n_years);
	
	for (int i = 0; i < n_locations; i++) {
		for (int j = 0; j < n_years; j++) { 		
			year = years[j];
			printf("%s,%s, %s (%2.3f,%2.3f) in "
			       "%d: %2.2f degrees\n", 
			       locations[i].city, locations[i].state,
			       locations[i].country,
			       locations[i].lat,
			       locations[i].lon, year,
			       lookup_magdev(locations[i].lat,
					     locations[i].lon, year));

		}
		printf("---\n");
	}

}
#endif /* !ARDUINO */