
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


#include "GeomagnetismHeader.h"
#include "EGM9615.h"

char MAG_GeomagIntroduction_WMM(MAGtype_MagneticModel *MagneticModel,
				char *VersionDate, char *ModelDate);


int main(int argc, char* argv[])
{
	MAGtype_MagneticModel * MagneticModels[1], *TimedMagneticModel;
	MAGtype_Ellipsoid Ellip;
	MAGtype_CoordSpherical CoordSpherical;
	MAGtype_CoordGeodetic CoordGeodetic;
	MAGtype_Date UserDate;
	MAGtype_GeoMagneticElements GeoMagneticElements, Errors;
	MAGtype_Geoid Geoid;
	
	char* filename = "WMM.COF";
	
	char VersionDate[12];
	int NumTerms, nMax = 0;
	int epochs = 1;
	
	/* Memory allocation */
	
	strncpy(VersionDate, VERSIONDATE_LARGE + 39, 11);
	VersionDate[11] = '\0';
	
	if(!MAG_robustReadMagModels(filename, &MagneticModels, epochs)) {
		printf("error: [WMM.COF] not found.\n ");
		return -1;
	}
	
	if(nMax < MagneticModels[0]->nMax)
		nMax = MagneticModels[0]->nMax;
	
	NumTerms = ((nMax + 1) * (nMax + 2) / 2);
	
	/* For storing the time modified WMM Model parameters */
	TimedMagneticModel = MAG_AllocateModelMemory(NumTerms); 
	if(MagneticModels[0] == NULL || TimedMagneticModel == NULL) {
		MAG_Error(2);
	}
	/* Set default values and constants */    
	MAG_SetDefaults(&Ellip, &Geoid);
	/* Check for Geographic Poles */
	
	/* Set EGM96 Geoid parameters */
	Geoid.GeoidHeightBuffer = GeoidHeightBuffer;
	Geoid.Geoid_Initialized = 1;
	
	// {"Miami Beach", "FLA", "USA", 25.7907, -80.1300},
	CoordGeodetic.phi = 25.7907; /* lat */
	CoordGeodetic.lambda = -80.1300; /* lon */

	/* Ground Level */
	CoordGeodetic.HeightAboveGeoid = CoordGeodetic.HeightAboveEllipsoid = 0; /* Not above geoid */
	Geoid.UseGeoid = 0;
	
	/* User entered height above MSL, convert it to the height above WGS-84 ellipsoid */
	Geoid.UseGeoid = 1;    
	CoordGeodetic.HeightAboveGeoid = 0.001; /* .001 Km = 1 meter */	
	MAG_ConvertGeoidToEllipsoidHeight(&CoordGeodetic, &Geoid);
	
	/* Input Year */
	UserDate.DecimalYear = 2020;
	UserDate.Year = 2020;    
	UserDate.Month = 12;
	UserDate.Day = 1;
    
	/*Convert from geodetic to Spherical Equations: 17-18, WMM Technical report*/		
	MAG_GeodeticToSpherical(Ellip, CoordGeodetic, &CoordSpherical);

	/* Time adjust the coefficients, Equation 19, WMM Technical report */
	MAG_TimelyModifyMagneticModel(UserDate, MagneticModels[0], TimedMagneticModel);

	/* Computes the geoMagnetic field elements and their time change*/
	MAG_Geomag(Ellip, CoordSpherical, CoordGeodetic, TimedMagneticModel, &GeoMagneticElements); 
	MAG_CalculateGridVariation(CoordGeodetic, &GeoMagneticElements);
	MAG_WMMErrorCalc(GeoMagneticElements.H, &Errors);

	/* Print the results */		    
	MAG_PrintUserDataWithUncertainty(GeoMagneticElements, Errors, CoordGeodetic,
					     UserDate, TimedMagneticModel, &Geoid); 

	MAG_FreeMagneticModelMemory(TimedMagneticModel);
	MAG_FreeMagneticModelMemory(MagneticModels[0]);
	
	return 0;
}

