#define AVOID_SINGULARITIES 1

#include "gepetto/viewer/corba/client.hh"
#include "viewer.h"

int main( ){

	Viewer viewer;

	const char*  dronename=PIE_SOURCE_DIR"/data/quadrotor_base.stl";
    viewer.createDrone(dronename);

	for(float i=0;i<500;i++)
	{
		float x=1.0;
		float y=1.0;
		float z=1.0+i/20;
		viewer.moveDrone(x,y,z);
	}
    


    return 0;
}
/* <<< end tutorial code <<< */
