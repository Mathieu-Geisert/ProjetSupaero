#define AVOID_SINGULARITIES 1

#include <acado_toolkit.hpp>

#include <acado_optimal_control.hpp>
#include <acado_gnuplot.hpp>

//#include <time.h>
#include<cstdlib>

/* Optimal Controller part codes:
	|-- Computation of optimal control one step further
		from the current initial state
	|-- The current initial state is read from an initial state file
		while the optimal control input is written in another file*/

int controlMPC(){

	USING_NAMESPACE_ACADO

	// INTRODUCE THE VARIABLES:
	// -------------------------
	DifferentialState     x,y,z,vx,vy,vz,phi,theta,psi,p,q,r,u1,u2,u3,u4;
	// x, y, z : position
	// vx, vy, vz : linear velocity
	// phi, theta, psi : orientation (Yaw-Pitch-Roll = Euler(3,2,1))
	// p, q, r : angular velocity
	// u1, u2, u3, u4 : velocity of the propellers
	Control               vu1,vu2,vu3,vu4;
	// vu1, vu2, vu3, vu4 : derivative of u1, u2, u3, u4
	DifferentialEquation  f;

	// Quad constants
	const double c = 0.00001;
	const double Cf = 0.00065;
	const double d = 0.250;
	const double Jx = 0.018;
	const double Jy = 0.018;
	const double Jz = 0.026;
	const double Im = 0.0001;
	const double m = 0.9;
	const double g = 9.81;
	const double Cx = 0.1;

	// Minimization Weights
	double coeffX = .00001;
	double coeffU = coeffX*0.0001;//0.000000000000001;
	double coeffX2 = coeffX * 100.;
	double coeffX3 = coeffX * 0.00001;
	double coeffO = -coeffX * 0.1;

	// final position (used in the cost function)
	double xf = 0., yf = 0., zf = 20.;
	
	//length (in second) of the trajectory predicted in the MPC
	double T = 8.;
	//time (in second) between two activation of the MPC algorithm
	double tmpc = 0.02;
	//number of nodes used in the Optimal Control Problem
	// 20 nodes means that the algorithm will discretized the trajectory equally into 20 pieces
	// If you increase the number of node, 
	// the solution will be more precise but calculation will take longer (~nb_nodes^2)
	// In ACADO, the commands are piecewise constant functions, constant between each node.
	int nb_nodes = 20.;

	// Write the final position into the data file
	// as the reference waypoint for each control calculation
	std::ofstream outFile;
	outFile.open("ref.txt");
	for(int i=0;i<nb_nodes;i++)
	{
		double temps = 0.5*i;
		outFile << temps << " " << xf << " "
				<< yf << " " << zf << " ";
		for(int j=0;j<12;j++)
			outFile << 0. << " ";
		outFile << 58.0 << " ";
		outFile << 58.0 << " ";
		outFile << 58.0 << " ";
		outFile << 58.0 << std::endl;
	}
	outFile.close();

	// DEFINE A OPTIMAL CONTROL PROBLEM
	// -------------------------------
	OCP ocp( 0.0, T, nb_nodes );

	// DEFINE THE COST FUNCTION
	// -------------------------------
	Function h, hf;
	h << x;
	h << y;
	h << z;
	h << vu1;
	h << vu2;
	h << vu3;
	h << vu4;
	h << p;
	h << q;
	h << r;

	hf << x;
	hf << y;
	hf << z;

	DMatrix Q(10,10), Qf(3,3);
	Q(0,0) = coeffX;
	Q(1,1) = coeffX;
	Q(2,2) = coeffX;
	Q(3,3) = coeffU;
	Q(4,4) = coeffU;
	Q(5,5) = coeffU;
	Q(6,6) = coeffU;
	Q(7,7) = coeffX2;
	Q(8,8) = coeffX2;
	Q(9,9) = coeffX2;

	Qf(0,0) = 1.;
	Qf(1,1) = 1.;
	Qf(2,2) = 1.;

	DVector reff(3), ref(10);
	ref(0) = xf;
	ref(1) = yf;
	ref(2) = zf;
	ref(3) = 58.;
	ref(4) = 58.;
	ref(5) = 58.;
	ref(6) = 58.;
	ref(7) = 0.;
	ref(8) = 0.;
	ref(9) = 0.;

	reff(0) = xf;
	reff(1) = yf;
	reff(2) = zf;


	ocp.minimizeLSQ ( Q, h, ref);
	//    ocp.minimizeLSQEndTerm(Qf, hf, reff);

	// DEFINE THE DYNAMIC EQUATION OF THE SYSTEM:
	// ----------------------------------
	f << dot(x) == vx;
	f << dot(y) == vy;
	f << dot(z) == vz;
	f << dot(vx) == Cf*(u1*u1+u2*u2+u3*u3+u4*u4)*sin(theta)/m;
	f << dot(vy) == -Cf*(u1*u1+u2*u2+u3*u3+u4*u4)*sin(psi)*cos(theta)/m;
	f << dot(vz) == Cf*(u1*u1+u2*u2+u3*u3+u4*u4)*cos(psi)*cos(theta)/m - g;
	f << dot(phi) == -cos(phi)*tan(theta)*p+sin(phi)*tan(theta)*q+r;
	f << dot(theta) == sin(phi)*p+cos(phi)*q;
	f << dot(psi) == cos(phi)/cos(theta)*p-sin(phi)/cos(theta)*q;
	f << dot(p) == (d*Cf*(u1*u1-u2*u2)+(Jy-Jz)*q*r)/Jx;
	f << dot(q) == (d*Cf*(u4*u4-u3*u3)+(Jz-Jx)*p*r)/Jy;
	f << dot(r) == (c*(u1*u1+u2*u2-u3*u3-u4*u4)+(Jx-Jy)*p*q)/Jz;
	f << dot(u1) == vu1;
	f << dot(u2) == vu2;
	f << dot(u3) == vu3;
	f << dot(u4) == vu4;

	// ---------------------------- DEFINE CONTRAINTES --------------------------------- //
	//Dynamic
	ocp.subjectTo( f );

	//State constraints
	//Constraints on the velocity of each propeller
	ocp.subjectTo( 16 <= u1 <= 95 );
	ocp.subjectTo( 16 <= u2 <= 95 );
	ocp.subjectTo( 16 <= u3 <= 95 );
	ocp.subjectTo( 16 <= u4 <= 95 );

	//Command constraints
	//Constraints on the acceleration of each propeller
	ocp.subjectTo( -100 <= vu1 <= 100 );
	ocp.subjectTo( -100 <= vu2 <= 100 );
	ocp.subjectTo( -100 <= vu3 <= 100 );
	ocp.subjectTo( -100 <= vu4 <= 100 );

	#if AVOID_SINGULARITIES == 1
	//Constraint to avoid singularity
	// In this example I used Yaw-Pitch-Roll convention to describe orientation of the quadrotor
	// when using Euler Angles representation, you always have a singularity, and we need to
	// avoid it otherwise the algorithm will crashed.
	ocp.subjectTo( -1. <= theta <= 1.);
	#endif


	//Example of Eliptic obstacle constraints (here, cylinders with eliptic basis)
	ocp.subjectTo( 16 <= ((x+3)*(x+3)+2*(z-5)*(z-5)) );
	ocp.subjectTo( 16 <= ((x-3)*(x-3)+2*(z-9)*(z-9)) );
	ocp.subjectTo( 16 <= ((x+3)*(x+3)+2*(z-15)*(z-15)) );


	// SETTING UP THE MPC CONTROLLER:
	// ------------------------------
	RealTimeAlgorithm alg(ocp, tmpc);

	//Usually, you do only one step of the optimisation algorithm (~Gauss-Newton here)
	//at each activation of the MPC, that way the delay between getting the state and
	//sending a command is as quick as possible.
	alg.set(MAX_NUM_ITERATIONS, 1);
	alg.set(PLOT_RESOLUTION,MEDIUM);
	//alg.set(GLOBALIZATION_STRATEGY, GS_LINESEARCH);
	alg.set(INTEGRATOR_TYPE, INT_RK45);
	//alg.set(KKT_TOLERANCE,1e-3);
	

	// StaticReferenceTrajectory:
	// Allows to define a static reference trajectory that 
	// the ControlLaw aims to track. The class 
	// StaticReferenceTrajectory allows to define a 
	// static reference trajectory 
 	// (given beforehand) that the ControlLaw aims to 
	// track while computing its output.
	StaticReferenceTrajectory zeroReference("TempData/ref.txt"); 
	Controller controller(alg,zeroReference);

	DVector stateInit(16);
	stateInit.setZero();
	
	// Generation of the initialState file
	// utilized for the first calculation step
	//stateInit(12) = 58.;
	//stateInit(13) = 58.;
	//stateInit(14) = 58.;
	//stateInit(15) = 58.;
	//std::ofstream outInit;
	//outInit.open("initialState.txt");
	//for(int i=0;i<16;i++)
	//{
	//	outInit << stateInit(i) << std::endl;
	//}
	//outInit.close();

	// Read the initial state from the file
	// which is changed in each step
	std::ifstream inInit;
	inInit.open("TempData/initialState.txt");
	for(int j=0;j<16;j++)
	{
		inInit >> stateInit(j);
	}
//	// Debug information output
//	std::cout << "stateInit:\n" << stateInit << std::endl;
	
	

	// Computation of the optimal control calculated
	controller.init(0.0,stateInit);
	controller.step(0.0,stateInit);

	DVector U(4);
	U.setZero();
	controller.getU(U);

	DVector stateFin(16);
	stateFin.setZero();
	controller.getP(stateFin);
	// Dimension of control input
	int NU = controller.getNU();

//	// Debug information output
//	std::cout << "Number of control computed:" << std::endl;
//	std::cout << NU << std::endl;
//	std::cout << "u=" << std::endl;
//	std::cout << U << std::endl;

	// Write the calculated control into an external file
	std::ofstream outControl;
	outControl.open("TempData/controlInput.txt");
	for(int i=0;i<NU;i++)
	{
		outControl << U(i) << std::endl;
	}
	outControl.close();
	
	system("clear");
	// Output termination information to the console
	std::cout << "Optimal control input calculation finished !" << std::endl;


	return 0;
}
/* <<< end tutorial code <<< */

