////////////////////////////////////////////////////////////////////////////////
/// @file ParticleSystem.h
/// @author Carola Gille
/// @author Ramon Blanquer
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

// Native
#include <vector>

// Custom
#include "LinkedParticle.h"
#include "GrowthParticle.h"

////////////////////////////////////////////////////////////////////////////////
/// @class ParticleSystem
/// @brief Wraps particle system functionality.
///
/// This class will be having large numbers of particles. It will be contained
/// under a Scene. The scene will make the ParticleSystem advance(). The
/// packageDataForDrawing() will prepare all the particle data for the client
/// application to be able to draw it.
////////////////////////////////////////////////////////////////////////////////
class ParticleSystem
{
public:

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Constructor.
  //////////////////////////////////////////////////////////////////////////////
  ParticleSystem();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Constructor.
  /// @param[in] _amount Number of particles to initialize the system with.
  //////////////////////////////////////////////////////////////////////////////
  ParticleSystem(int _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Steps all the particles in the system (calls internally the
  /// Particle::advance() method of each particle).
  //////////////////////////////////////////////////////////////////////////////
  void advance();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Populates the system with this amount of particles.
  /// @param[in] _amount Number of particles to initialize the system with.
  //////////////////////////////////////////////////////////////////////////////
  void fill(unsigned int _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Retrieves the particle at that index, returns a pointer.
  /// @param[in] _idx Index of the particle we want to get.
  /// @returns The particle object.
  //////////////////////////////////////////////////////////////////////////////
  Particle* getParticle(unsigned int _idx);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Get the number of particles in the system
  /// @returns Number of particles in the system.
  //////////////////////////////////////////////////////////////////////////////
  unsigned int getSize();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Selects randomly a particle and splits it. Useful for debugging.
  //////////////////////////////////////////////////////////////////////////////
  void splitRandomParticle();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief This will delete a particle and all the connections to it.
  /// @param[in] _idx Index of the particle to remove.
  //////////////////////////////////////////////////////////////////////////////
  void deleteParticle(unsigned int _idx);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Populates an OpenGL-friendly flat vector of floats that will be
  /// used for drawing them.
  /// @param[out] _packagedData Flat array that will be populated.
  //////////////////////////////////////////////////////////////////////////////
  void packageDataForDrawing(std::vector<float> &_packagedData);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Populates a list of three dimensional vectors that will be used
  /// for debug-drawing the links.
  /// @param[out] _returnList Vector to populate.
  //////////////////////////////////////////////////////////////////////////////
  void getLinksForDraw(std::vector<QVector3D> &_returnList);

  QVector3D calculateParticleCentre();

  QVector3D calculateAverageDistanceFromCentre();

  void bulge();

private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the list of particles in the system.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<std::unique_ptr<Particle>> m_particles;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Keeps track of the number of particles.
  //////////////////////////////////////////////////////////////////////////////
  unsigned int m_particleCount;

  QVector3D m_particleCentre;

  QVector3D m_averageDistance;

};

#endif // PARTICLESYSTEM_H
