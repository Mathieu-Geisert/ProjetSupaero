#include "mpcsolver.h"
#include "input.h"
#include <iostream>

using std::cout; using std::endl;


int main()
{
    Input input;
    while (true)
    {
        input.test();
        sf::sleep(sf::milliseconds(100));
    }

//    MPCSolver solver;
//    double t = 0;
//    double dt = 0.02;

//    solver.controlMPC();
//    solver.systemEvol(t,dt);
//    cout << solver.stateVector()[0] << " " << solver.stateVector()[1] << " " << solver.stateVector()[2] << endl;
//    solver.controlMPC();
//    solver.systemEvol(t,dt);
//    cout << solver.stateVector()[0] << " " << solver.stateVector()[1] << " " << solver.stateVector()[2] << endl;
//    solver.controlMPC();
//    solver.systemEvol(t,dt);
//    cout << solver.stateVector()[0] << " " << solver.stateVector()[1] << " " << solver.stateVector()[2] << endl;
//    solver.controlMPC();
//    solver.systemEvol(t,dt);
//    cout << solver.stateVector()[0] << " " << solver.stateVector()[1] << " " << solver.stateVector()[2] << endl;

    return 0;
}
