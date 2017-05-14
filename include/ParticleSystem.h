////////////////////////////////////////////////////////////////////////////////
/// @file ParticleSystem.h
/// @author Carola Gille
/// @author Ramon Blanquer
/// @author Esme Prior
/// @author Lydia Kenton
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

// Native
#include <vector>

// Custom
#include "LinkedParticle.h"
#include "GrowthParticle.h"
#include "AutomataParticle.h"
#include "PointLight.h"

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
  /// @param[in] _particleType Type of particle that the particle system is
  /// filled with.
  //////////////////////////////////////////////////////////////////////////////
  ParticleSystem(char _particleType);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Steps all the particles in the system .
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
  /// @brief Gets the nearest particle to the point light
  /// @returns Index of nearest particle to point light
  //////////////////////////////////////////////////////////////////////////////
  unsigned int getNearestParticle(std::vector<uint> _toSplit);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Selects a particle randomly and splits it. Useful for debugging.
  //////////////////////////////////////////////////////////////////////////////
  void splitRandomParticle();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief This will delete a particle and all the connections to it.
  //////////////////////////////////////////////////////////////////////////////
  void deleteParticle();

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
  void getLinksForDraw(std::vector<uint> &_returnList);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Calculates the average position of all particles to use as
  /// their centre. Returns this position as a vector.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D calculateParticleCentre();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Calculates the average distance from centre to particles.
  /// @return This distance.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D calculateAverageDistanceFromCentre();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief This will delete a particle and all the connections to it.
  /// @param[in] _size The new size.
  //////////////////////////////////////////////////////////////////////////////
  void setParticleSize(double _size);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Changes state if m_forces.
  /// @param[in] _state that forces are set t.
  //////////////////////////////////////////////////////////////////////////////
  void toggleForces(bool _state);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Changes state if m_particleDeath
  /// @param[in] _state that forces are set to
  //////////////////////////////////////////////////////////////////////////////
  void toggleParticleDeath (bool _state);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets Cohesion Value.
  /// @param[in] _amount new Cohesion amount.
  //////////////////////////////////////////////////////////////////////////////
  void setCohesion(int _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets Bulge amount.
  //////////////////////////////////////////////////////////////////////////////
  void bulge();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Adds food.
  //////////////////////////////////////////////////////////////////////////////
  void addFood();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets localCohesion amount.
  /// @param[in] _amount new localCohesion amount.
  //////////////////////////////////////////////////////////////////////////////
  void setLocalCohesion(int _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets automataRadius amount for automata particles.
  /// @param[in] _amount new automataRadius amount.
  //////////////////////////////////////////////////////////////////////////////
  void setAutomataRadius(int _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets automataTime amount for automata particles' speed.
  /// @param[in] _amount new automataTime amount.
  //////////////////////////////////////////////////////////////////////////////
  void setAutomataTime(int _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets point light position.
  /// @param[in] _lightPos New position of the point light.
  //////////////////////////////////////////////////////////////////////////////
  void setLightPos(QVector3D _lightPos);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets the branch length for Growth Particles.
  /// @param[in] _amount length of a branch.
  //////////////////////////////////////////////////////////////////////////////
  void setBranchLength(float _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Resets all values in the particle system.
  /// @param[in] _particleType Type of particle the particle system is gonna be
  /// filled with.
  //////////////////////////////////////////////////////////////////////////////
  void reset(char _particleType);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets the child threshold for Growth Particles.
  /// @param[in] _value amount of children per particle.
  //////////////////////////////////////////////////////////////////////////////
  void setChildThreshold(int _value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets the colour of the light.
  /// @param[in] _colour The colour of the particles.
  //////////////////////////////////////////////////////////////////////////////
  void setColour(float _colour);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot tunring nearest particles on and off.
  /// @param[in] _state state of nearest particle
  //////////////////////////////////////////////////////////////////////////////
  void setNearestParticleState(bool _state);

  /////////////////////////////////////////////////////////////////////////////
  /// @brief sets the growtolight attribute
  /// @param _state
  /////////////////////////////////////////////////////////////////////////////
  void setGrowToLight(bool _state);

private:

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the state of the forces
  //////////////////////////////////////////////////////////////////////////////
  bool m_forces;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the state of particle death
  //////////////////////////////////////////////////////////////////////////////
  bool m_particleDeath;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the amount of cohesion in percentage
  //////////////////////////////////////////////////////////////////////////////
  int m_cohesion;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the amount of local cohesion in percentage
  //////////////////////////////////////////////////////////////////////////////
  int m_localCohesion;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the size of automata radius in percentage
  //////////////////////////////////////////////////////////////////////////////
  int m_automataRadius;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the speed of automata birth in percentage
  //////////////////////////////////////////////////////////////////////////////
  int m_automataTime;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the particle Type like a flag,
  /// G stands for GrowthParticle
  /// L stands for LinkedParticle
  //////////////////////////////////////////////////////////////////////////////
  char m_particleType;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the list of particles in the system.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<std::unique_ptr<Particle>> m_particles;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Keeps track of the number of particles.
  //////////////////////////////////////////////////////////////////////////////
  unsigned int m_particleCount;

  /////////////////////////////////////////////////////////////////////////////
  /// @brief m_rd random device for LinkedParticle calculations
  /////////////////////////////////////////////////////////////////////////////
  std::random_device m_rd;

  /////////////////////////////////////////////////////////////////////////////
  /// @brief m_gen random generator for LinkedParticle calculations
  /////////////////////////////////////////////////////////////////////////////
  std::mt19937_64 m_gen;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the average position of all the particles in the system.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_particleCentre;

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the position for average distance.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_averageDistance;

  ////////////////////////////////////////////////////////////////////////////
  /// @brief m_lightPos stores the position of the light
  ////////////////////////////////////////////////////////////////////////////
  QVector3D m_lightPos;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the ID iterations before deletion.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<uint> m_iterID;

  ////////////////////////////////////////////////////////////////////////////
  /// @brief m_currentParticleSize stores size of particles
  ////////////////////////////////////////////////////////////////////////////
  float m_currentParticleSize;

  ///////////////////////////////////////////////////////////////////////////
  /// @brief m_nearestParticle states if the splitting calculation will take
  /// in account the nearest particle or a random one
  ///////////////////////////////////////////////////////////////////////////
  bool m_nearestParticleState;

  ///////////////////////////////////////////////////////////////////////////
  /// @brief stores state for growth particles, decides if they grow towards
  /// light
  ///////////////////////////////////////////////////////////////////////////
  bool m_GP_growtoLight;

};

#endif // PARTICLESYSTEM_H
