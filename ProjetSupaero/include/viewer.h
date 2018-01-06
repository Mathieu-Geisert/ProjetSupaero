#ifndef VIEWER
#define VIEWER

#include "gepetto/viewer/corba/client.hh"
#include <string>
#include "environmentparser.h"

using namespace graphics;
using namespace corbaServer;
using namespace std;

typedef CORBA::ULong WindowID;

class Viewer
{
	public:
		Viewer();
		void createEnvironment(std::vector<Ecylinder> cylinder_list);
		void createDrone(const char*  t);
		void moveDrone(double x, double y, double z, double roll, double pitch, double yaw);
		void setArrow(int vx, int vy, int vz);
		void addNode(double x, double y, double z);

	private:
		ClientCpp client;
		WindowID w_id;
		se3::SE3 se3Drone;
		int nb_node_traj;

};

#endif

