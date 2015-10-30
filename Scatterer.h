#ifndef SCATTERER_H_
#define SCATTERER_H_

#include "Spherical.h"
#include "ElectroMagnetic.h"
#include "Tools.h"

/**
 * The Scatterer class is the highest level element of a geometry.
 * This is the only class used to create the geometry. The radius property
 * will define a virtual sphere encompassing the entire structure. Different
 * behaviors will be created based on the type of scatterer. Current
 * implementation includes only spherical type scatterers.
 */
class Scatterer
{
private:
	bool initDone;		/**< Specifies if the object has been initialized. */
public:
	/**
	 * Default Scatterer constructor.
	 * Does NOT initialize the object.
	 */
	Scatterer();

	/**
	 * Initialization constructor for Scatterer.
	 * Creates a sphere scatterer centered in vR with properties elmag and
	 * radius r.
	 * @param vR_ the coordinates of the center of the scatterer.
	 * @param elmag_ the electromagnetic properties of the scatterer.
	 * @param radius_ the radius of the virtual sphere.
	 * @param nMax_ the maximum value of the n iterator.
	 * @see init()
	 */
	Scatterer(Spherical<double> vR_, ElectroMagnetic elmag_, double radius_, int nMax_);

	/**
	 * Initialization function for Scatterer.
	 * @param nMax_ the maximum value of the n iterator.
	 */
	void init(int nMax_);

	/**
	 * Default scatterer destructor.
	 */
	virtual ~Scatterer();

	Spherical<double> vR; 	/**< The coordinates of the center of the scatterer.*/
	ElectroMagnetic elmag;	/**< The electromagnetic properties of the scatterer.*/
	int nMax;				/**< Maximum value of the n iterator. */
	double radius;			/**< The radius of a sphere encompassing the scatterer.*/

	/**
	 * @warning The sourceCoef array needs to be initialized by the populating function in order to work!
	 */
	complex<double> *sourceCoef;	/**< The source coefficients needed for SH work.*/

	/**
	 * Initializes a sphere scatterer.
	 * @param vR_ the coordinates of the center of the scatterer.
	 * @param elmag_ the electromagnetic properties of the scatterer.
	 * @param radius_ the radius of the virtual sphere.
	 * @param nMax_ the maximum value of the n iterator.
	 * @see Scatterer()
	 */
	void init(Spherical<double> vR_, ElectroMagnetic elmag_, double radius_, int nMax_);
};

#endif /* SCATTERER_H_ */