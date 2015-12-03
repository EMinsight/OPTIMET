#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <string>
/**
 * The Simulation class implements a full simulation.
 * A Simulation object will create a set of Cases and Requests based
 * on the input file.
 */
class Simulation
{
private:
  std::string caseFile; /**< Name of the case without extensions. */
public:
  /**
   * Default constructor for the Simulation class.
   * Does NOT initialize the object.
   */
  Simulation(std::string const & filename) : caseFile(filename) {};

  /**
   * Default destructor for the Simulation class.
   */
  virtual ~Simulation() {};

  /**
   * Starts a simulation.
   * @return 0 if successful, 1 otherwise.
   */
  int run();

  /**
   * Finishes a simulation.
   * Placeholder method. Not needed at the moment.
   * @return 0 if succesful, 1 otherwise.
   */
  int done();
};

#endif /* SIMULATION_H_ */
