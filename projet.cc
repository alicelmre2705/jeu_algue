/**
 * @file projet.cc
 * @author LEMAIRE Alice 0.5, CAPDEVILLE Maud 0.5
 * @version 0.3
 */

#include <string>

#include "gui.h"

using namespace std;

int main(int argc, char *argv[])
{
  Simulation simulation;
  std::ofstream outFile("output.log");
  std::cout.rdbuf(outFile.rdbuf());
  std::cerr.rdbuf(outFile.rdbuf());

  if (argc == 2)
  {
    string fichier;
    fichier = argv[1];

    if (simulation.read_contain_invalid_data(fichier))
      simulation.destroy_lifeforms();
  }

  auto app = Gtk::Application::create();
  return app->make_window_and_run<MyEvent>(1, argv, simulation);
}
