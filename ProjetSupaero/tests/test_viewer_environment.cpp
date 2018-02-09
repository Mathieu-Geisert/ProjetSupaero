#define AVOID_SINGULARITIES 2

#include "gepetto/viewer/corba/client.hh"
#include "viewer.h"
#include "environmentparser.h"

// Demonstration d'affichage de l'environnement a partir d'un fichier xml

int main( ){

    Viewer viewer;

//    const char*  dronename=PIE_SOURCE_DIR"/data/quadrotor_base.stl";
    std::string a=PIE_SOURCE_DIR"/data/envsave.xml";

    // Creation de l'environnement
    EnvironmentParser e;
    Epoint center1;
    Epoint center2;

    center1.x=0.f;
    center1.y=0.f;
    center1.z=0.f;
    center2.x=15.f;
    center2.y=0.f;
    center2.z=0.f;

    e.addCylinder(center1,center2,1.2f);
    e.save(a);


    // Chargement de l'environnement
    EnvironmentParser g(a);
    std::vector<Ecylinder> cylinder_list=g.readData();

    viewer.createEnvironment(cylinder_list);

    return 0;
}
