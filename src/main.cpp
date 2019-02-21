#include "aegis/application.h"

using namespace ICONation;

int main (int argc, char **argv)
{
    std::unique_ptr<Aegis::Application> Aegis (new Aegis::Application (argc, argv));
    return Aegis->run();
}