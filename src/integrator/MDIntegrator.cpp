#include "error.hpp"
#include "python.hpp"

#include "integrator/MDIntegrator.hpp"
#include "particles/Storage.hpp"

using namespace espresso;
using namespace espresso::integrator;

/* -- define the Logger for the class  ------------------------------------------- */

LOG4ESPP_LOGGER(MDIntegrator::theLogger, "Integrator");

MDIntegrator::MDIntegrator(particles::Set::SelfPtr _set,
                           Property< Real3D >::SelfPtr _posProperty,
                           Property< Real3D >::SelfPtr _velProperty,
                           Property< Real3D >::SelfPtr _forceProperty,
			   real timestep) 
  : set(_set),
    posProperty(_posProperty),
    velProperty(_velProperty),
    forceProperty(_forceProperty)
{ setTimestep(timestep); }

void MDIntegrator::run(int nsteps)
{
  LOG4ESPP_INFO(theLogger, "MDIntegrator will run " << nsteps << " steps");

  if (timestep <= 0.0) {
     ARGERROR(theLogger, "Illegal timestep in MDIntegrator: " << timestep <<
                         ", value must be positive");
  }
  if (nsteps < 0) {
     ARGERROR(theLogger, "Illegal value for nsteps in MDIntegrator: " << nsteps);
  }

  startIntegration(*this);

  for (nTimestep = 0; nTimestep < nsteps; nTimestep++) {
     LOG4ESPP_DEBUG(theLogger, "Integrator runs step " << nTimestep << " of " << nsteps);
     startStep(*this);

     // runSingleStep is the routine provided by the derived class
     step(); 

     endStep(*this);
  }

  endIntegration(*this);
}

//////////////////////////////////////////////////
// REGISTRATION WITH PYTHON
//////////////////////////////////////////////////
void
MDIntegrator::registerPython() {
  using namespace espresso::python;

  class_< MDIntegrator, boost::noncopyable >
    ("integrator_MDIntegrator", no_init)
    .add_property("timestep", &MDIntegrator::getTimestep, 
		  &MDIntegrator::setTimestep)
    .def("getPosProperty", &MDIntegrator::getPosProperty)
    .def("getVelProperty", &MDIntegrator::getVelProperty)
    .def("getForceProperty", &MDIntegrator::getForceProperty)
    .def("run", &MDIntegrator::run)
    .def("step", pure_virtual(&MDIntegrator::step))
  ;
}

