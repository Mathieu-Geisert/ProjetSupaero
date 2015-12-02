#define AVOID_SINGULARITIES 1

#include <acado_toolkit.hpp>

#include <acado_optimal_control.hpp>
#include <acado_gnuplot.hpp>

#include <time.h>

/* >>> start tutorial code >>> */
clock_t t;

int main( ){

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

    //
    double T = 8.; //length (in second) of the trajectory predicted in the MPC
    int nb_nodes = 20.; //number of nodes used in the Optimal Control Problem
    // 20 nodes means that the algorithm will discretized the trajectory equally into 20 pieces
    // If you increase the number of node, the solution will be more precise but calculation will take longer (~nb_nodes^2)
    // In ACADO, the commands are piecewise constant functions, constant between each node.
    double tmpc = 0.2; //time (in second) between two activation of the MPC algorithm

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


    // The cost function is define as : integrale from 0 to T { transpose(h(x,u,t)-ref)*Q*(h(x,u,t)-ref) }   +    transpose(hf(x,u,T))*Qf*hf(x,u,T)
    //                                  ==     integrale cost (also called running cost or Lagrange Term)    +        final cost (Mayer Term)
    ocp.minimizeLSQ ( Q, h, ref);
//    ocp.minimizeLSQEndTerm(Qf, hf, reff);

    // When doing MPC, you need terms in the cost function to stabilised the system => p, q, r and vu1, vu2, vu3, vu4. You can check that if you reduce their weights "coeffX2" or "coeffU" too low, the optimization will crashed.

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


    // SETTING UP THE (SIMULATED) PROCESS:
    // -----------------------------------
    OutputFcn identity;
    // The next line define the equation used to simulate the system :
    // here "f" is used (=the same as the one used for the ocp) so the trajectory simulated
    // from the commands will be exactly the same as the one calculated by the MPC.
    // If for instance you want to test robusness, you can define an other dynamic equation "f2"
    // with changed parameters and put it here.
    DynamicSystem dynamicSystem( f,identity );
    Process process( dynamicSystem,INT_RK45 );

    // SETTING UP THE MPC CONTROLLER:
    // ------------------------------
    RealTimeAlgorithm alg( ocp, tmpc );

    //Usually, you do only one step of the optimisation algorithm (~Gauss-Newton here)
    //at each activation of the MPC, that way the delay between getting the state and
    //sending a command is as quick as possible.
    alg.set( MAX_NUM_ITERATIONS, 1 );

    StaticReferenceTrajectory zeroReference;
    Controller controller( alg,zeroReference );


    // SET AN INITIAL GUESS FOR THE FIRST MPC LOOP (NEXT LOOPS WILL USE AS INITIAL GUESS THE SOLUTION FOUND AT THE PREVIOUS MPC LOOP)
    Grid timeGrid(0.0,T,nb_nodes+1);
    VariablesGrid x_init(16, timeGrid);
    // init with static
    for (int i = 0 ; i<nb_nodes+1 ; i++ ) {
            x_init(i,0) = 0.;
            x_init(i,1) = 0.;
            x_init(i,2) = 0.;
            x_init(i,3) = 0.;
            x_init(i,4) = 0.;
            x_init(i,5) = 0.;
            x_init(i,6) = 0.;
            x_init(i,7) = 0.;
            x_init(i,8) = 0.;
            x_init(i,9) = 0.;
            x_init(i,10) = 0.;
            x_init(i,11) = 0.;
            x_init(i,12) = 58.; //58. is the propeller rotation speed so the total thrust balance the weight of the quad
            x_init(i,13) = 58.;
            x_init(i,14) = 58.;
            x_init(i,15) = 58.;
        }
    alg.initializeDifferentialStates(x_init);

    // SET OPTION AND PLOTS WINDOW
    // ---------------------------
    // Linesearch is an algorithm which will try several points along the descent direction to choose a better step length.
    // It looks like activating this option produice more stable trajectories.198
    alg.set( GLOBALIZATION_STRATEGY, GS_LINESEARCH );

    alg.set(INTEGRATOR_TYPE, INT_RK45);

    // You can uncomment those lines to see how the predicted trajectory involve along time
    // (but be carefull because you will have 1 ploting window per MPC loop)
//    GnuplotWindow window1(PLOT_AT_EACH_ITERATION);
//    window1.addSubplot( z,"DifferentialState z" );
//    window1.addSubplot( x,"DifferentialState x" );
//    window1.addSubplot( theta,"DifferentialState theta" );
//    window1.addSubplot( 16./((x+3)*(x+3)+4*(z-5)*(z-5)),"Dist obs1" );
//    window1.addSubplot( 16./((x-3)*(x-3)+4*(z-9)*(z-9)),"Dist obs2" );
//    window1.addSubplot( 16./((x+2)*(x+2)+4*(z-15)*(z-15)),"Dist obs3" );
//    alg<<window1;


    // SETTING UP THE SIMULATION ENVIRONMENT,  RUN THE EXAMPLE...
    // ----------------------------------------------------------
    // The first argument is the starting time, the second the end time.
    SimulationEnvironment sim( 0.0,10.,process,controller );

    //Setting the state of the sytem at the beginning of the simulation.
    DVector x0(16);
    x0.setZero();
    x0(0) = 0.;
    x0(12) = 58.;
    x0(13) = 58.;
    x0(14) = 58.;
    x0(15) = 58.;

    t = clock();
    if (sim.init( x0 ) != SUCCESSFUL_RETURN)
        exit( EXIT_FAILURE );
    if (sim.run( ) != SUCCESSFUL_RETURN)
        exit( EXIT_FAILURE );
    t = clock() - t;
    std::cout << "total time : " << (((float)t)/CLOCKS_PER_SEC)<<std::endl;

    // ...SAVE THE RESULTS IN FILES
    // ----------------------------------------------------------

    std::ofstream file;
    file.open("/tmp/log_state.txt",std::ios::out);
    std::ofstream file2;
    file2.open("/tmp/log_control.txt",std::ios::out);

    VariablesGrid sampledProcessOutput;
    sim.getSampledProcessOutput( sampledProcessOutput );
    sampledProcessOutput.print(file);

    VariablesGrid feedbackControl;
    sim.getFeedbackControl( feedbackControl );
    feedbackControl.print(file2);


    // ...AND PLOT THE RESULTS
    // ----------------------------------------------------------

    GnuplotWindow window;
    window.addSubplot( sampledProcessOutput(0), "x " );
    window.addSubplot( sampledProcessOutput(1), "y " );
    window.addSubplot( sampledProcessOutput(2), "z " );
    window.addSubplot( sampledProcessOutput(6),"phi" );
    window.addSubplot( sampledProcessOutput(7),"theta" );
    window.addSubplot( sampledProcessOutput(8),"psi" );
    window.plot( );

    return 0;
}
/* <<< end tutorial code <<< */


