#define AVOID_SINGULARITIES 2

#include "gepetto/viewer/corba/client.hh"
#include "viewer.h"
#include "environmentparser.h"

// Demonstration d'affichage de l'environnement a partir d'un fichier xml

int main( ){

	Viewer viewer;

	const char*  dronename=PIE_SOURCE_DIR"/data/quadrotor_base.stl";
	std::string a=PIE_SOURCE_DIR"/data/envsave.xml";

	// Creation de l'environnement
	EnvironmentParser e;
	Epoint center1;
	Epoint center2;

	center1.x=0.0;
	center1.y=0.0;
	center1.z=0.00;
	center2.x=15.0;
	center2.y=0.0;
	center2.z=0.0;

	e.addCylinder(center1,center2,1.2);



	e.save(a);


	// Chargement de l'environnement
	EnvironmentParser g(a);
	std::vector<Ecylinder> cylinder_list=g.readData();

	viewer.createEnvironment(cylinder_list);

    return 0;
}
