#include "server.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server(5005);

    return a.exec();
}
