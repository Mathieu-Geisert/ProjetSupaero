#include "viewer.h"

#include <iostream>
#include <unistd.h>
#include <omniORB4/CORBA.h>
#include <string>
#include <cmath>
#include "environmentparser.h"

using namespace std;
typedef CORBA::ULong WindowID;
using namespace Eigen;

Viewer::Viewer(): client()
{
    WindowID w_id = client.createWindow("window");
    client.createScene("/world");
    client.addSceneToWindow("/world",w_id);
    client.createGroup("/world/env");

    se3Drone = se3::SE3::Identity();
    se3Drone.translation({0.,0.,2.});
    nb_node_traj=0;
}


void Viewer::createEnvironment(std::vector<Ecylinder> cylinder_list)
{
    float yellow[4] = {1.f,1.f,.1f,1.f};
    se3::SE3 se3position = se3::SE3::Identity();
    int i = 1;
    for(Ecylinder cyl : cylinder_list)
    {
        string n = "/world/env/cylinder"+std::to_string(i);
        const char* name = n.c_str();
        client.addCylinder(name, cyl.radius, sqrt(pow(cyl.x2-cyl.x1,2.f)+pow(cyl.y2-cyl.y1,2.f)+pow(cyl.z2-cyl.z1,2.f)), yellow);


        se3position.translation({(cyl.x1+cyl.x2)/2.f,(cyl.y1+cyl.y2)/2.f,(cyl.z1+cyl.z2)/2.f});

        double x = cyl.x2-cyl.x1;
        double y = cyl.y2-cyl.y1;
        double z = cyl.z2-cyl.z1;

        // Rotation en z
        double theta = atan2(y,x);
        Matrix3d m_z(3,3);
        m_z(0,0) = cos(theta);
        m_z(0,1) = -sin(theta);
        m_z(0,2) = 0.;

        m_z(1,0) = sin(theta);
        m_z(1,1) = cos(theta);
        m_z(1,2) = 0.;

        m_z(2,0) = 0.;
        m_z(2,1) = 0.;
        m_z(2,2) = 1.;


        // Rotation en y
        double phi = -atan2(sqrt(pow(x,2)+pow(y,2)),z);
        Matrix3d m_y(3,3);
        m_y(0,0) = cos(phi);
        m_y(0,1) = 0.;
        m_y(0,2) = -sin(phi);

        m_y(1,0) = 0.;
        m_y(1,1) = 1.;
        m_y(1,2) = 0.;

        m_y(2,0) = sin(phi);
        m_y(2,1) = 0.;
        m_y(2,2) = cos(phi);

        se3position.rotation(m_z.cast<float>()*m_y.cast<float>());
        client.applyConfiguration(name, se3position) ;
        i++;
    }
    client.refresh();
}

void Viewer::createDrone(const char*  t)
{
    bool a = client.addMesh("/world/drone", t) ;
    if(a == 0)
    {
        std::cout << "Erreur de chargement du modèle du drone"<< std::endl;
    }

    se3::SE3 se3position = se3::SE3::Identity();
    se3position.translation({0.,0.,1.});
    client.applyConfiguration("/world/drone", se3position);

    float red[4] = {1.f,0.f,.0f,1.f};
    client.addCylinder("/world/arrow", .1f, 4.f, red);

    client.refresh();
}

void Viewer::moveDrone(double x, double y, double z, double roll, double pitch, double yaw)
{

    Matrix3d identity(3,3);
    identity(0,0) = 1.;
    identity(0,1) = 0.;
    identity(0,2) = 0.;

    identity(1,0) = 0.;
    identity(1,1) = 1.;
    identity(1,2) = 0.;

    identity(2,0) = 0.;
    identity(2,1) = 0.;
    identity(2,2) = 1.;

    se3Drone.rotation() = identity.cast<float>();
    se3Drone.translation({-(float)x,-(float)y,-(float)z});
    client.applyConfiguration("/world/env", se3Drone);
    se3Drone.translation({0.,0.,0.});

    // Roll
    Matrix3d m_roll(3,3);
    m_roll(0,0) = 1.;
    m_roll(0,1) = 0.;
    m_roll(0,2) = 0.;

    m_roll(1,0) = 0.;
    m_roll(1,1) = cos(roll);
    m_roll(1,2) = -sin(roll);

    m_roll(2,0) = 0.;
    m_roll(2,1) = sin(roll);
    m_roll(2,2) = cos(roll);

    // Pitch
    Matrix3d m_pitch(3,3);
    m_pitch(0,0) = cos(pitch);
    m_pitch(0,1) = 0.;
    m_pitch(0,2) = sin(pitch);

    m_pitch(1,0) = 0.;
    m_pitch(1,1) = 1.;
    m_pitch(1,2) = 0.;

    m_pitch(2,0) = -sin(pitch);
    m_pitch(2,1) = 0.;
    m_pitch(2,2) = cos(pitch);

    // Yaw
    Matrix3d m_yaw(3,3);
    m_yaw(0,0) = cos(yaw);
    m_yaw(0,1) = -sin(yaw);
    m_yaw(0,2) = 0.;

    m_yaw(1,0) = sin(yaw);
    m_yaw(1,1) = cos(yaw);
    m_yaw(1,2) = 0.;

    m_yaw(2,0) = 0.;
    m_yaw(2,1) = 0.;
    m_yaw(2,2) = 1.;

    //cout << m_yaw << endl;
    se3Drone.rotation() = m_roll.cast<float>() * m_pitch.cast<float>() * m_yaw.cast<float>();
    client.applyConfiguration("/world/drone", se3Drone);
    client.refresh();
}

void Viewer::setArrow(int vx, int vy, int vz)
{
    auto dronePos = se3Drone.translation();
    se3::SE3 se3position = se3::SE3::Identity();

    if (vx == 0 && vy == 0 && vz == 0)
        se3position.translation({ 0.f,0.f,10000.f });
    else
    {
        se3position.translation({ dronePos[0] + 2.5f*vx , dronePos[1] + 2.5f*vy, dronePos[2] + 2.5f*vz });

        // Rotation en z
        double theta = atan2(vy,vx);
        Matrix3d m_z(3,3);
        m_z(0,0) = cos(theta);
        m_z(0,1) = -sin(theta);
        m_z(0,2) = 0.;

        m_z(1,0) = sin(theta);
        m_z(1,1) = cos(theta);
        m_z(1,2) = 0.;

        m_z(2,0) = 0.;
        m_z(2,1) = 0.;
        m_z(2,2) = 1.;

        // Rotation en y
        double phi = -atan2(sqrt(pow(vx,2)+pow(vy,2)),vz);
        Matrix3d m_y(3,3);
        m_y(0,0) = cos(phi);
        m_y(0,1) = 0.;
        m_y(0,2) = -sin(phi);

        m_y(1,0) = 0.;
        m_y(1,1) = 1.;
        m_y(1,2) = 0.;

        m_y(2,0) = sin(phi);
        m_y(2,1) = 0.;
        m_y(2,2) = cos(phi);

        se3position.rotation(m_z.cast<float>()*m_y.cast<float>());
    }

    client.applyConfiguration("/world/arrow", se3position) ;
}

void Viewer::addNode(double x, double y, double z)
{
	nb_node_traj++;
	se3::SE3 se3position = se3::SE3::Identity();
	se3position.translation({x, y, z});
	float color[4] = {1.f,0.f,0.f,0.3f};
	client.addSphere(("/world/env/traj"+std::to_string(nb_node_traj)).c_str(), 0.05, color);
	client.applyConfiguration(("/world/env/traj"+std::to_string(nb_node_traj)).c_str(), se3position);
}
