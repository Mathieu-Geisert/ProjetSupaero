#define AVOID_SINGULARITIES 1

#include "gepetto/viewer/corba/client.hh"
#include "my-file.hpp"

int main( ){

    TOTO tot;
    tot.disp();
    std::cout << "Hello" << std::endl;
    // graphics::corbaServer::ClientCpp client = graphics::corbaServer::ClientCpp();
    // client.createWindow("window");

    std::string filename = PIE_SOURCE_DIR"/data/put_here_your_meshes";
    std::cout << filename << std::endl;
    return 0;
}
/* <<< end tutorial code <<< */