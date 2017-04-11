////////////////////////////////////////////////////////////////////////////////
/// @file Particle.h
/// @author Carola Gille
/// @author Ramon Blanquer
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_H
#define PARTICLE_H

// Native
#include <memory>
#include <vector>

// Qt
#include<QVector3D>

// Custom
#include "PointLight.h"

////////////////////////////////////////////////////////////////////////////////
/// @class Particle
/// @brief Base particle class providing common methods and attributes that will
/// be common to subclasses. The advance() method will need to be reimplemented.
////////////////////////////////////////////////////////////////////////////////
class Particle
{

public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Default constructor placing particle at the origin.
  //////////////////////////////////////////////////////////////////////////////
  Particle();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Custom constructor allowing user input for position.
  /// @param[in] _x x Position of the particle.
  /// @param[in] _y y Position of the particle.
  /// @param[in] _z z Position of the particle.
  /// @param[in] _size size of particle
  //////////////////////////////////////////////////////////////////////////////
  Particle(
      qreal _x,
      qreal _y,
      qreal _z,
      float _size);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Custom constructor allowing user input for position as well as
  /// which particles it is connected to.
  /// @param[in] _x x Position of the particle.
  /// @param[in] _y y Position of the particle.
  /// @param[in] _z z Position of the particle.
  /// @param[in] _connectedParticles List of particle IDs to be connected to
  /// the newly generated particle.
  /// @param[in] _size size of particle
  //////////////////////////////////////////////////////////////////////////////
  Particle(
      qreal _x,
      qreal _y,
      qreal _z,
      std::vector<unsigned int> _connectedParticles,
      float _size);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Default constructor.
  //////////////////////////////////////////////////////////////////////////////
  ~Particle() = default;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Adds the velocity to the position, could be overwritten if
  /// inherited if other custom behaviours would be needed.
  //////////////////////////////////////////////////////////////////////////////
  void advance();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Calculates the new velocity of the particle based on the forces
  /// that act on it.
  /// @param _particleCentre Position of the average centre of all particles
  /// @param [in] _particleList List of all particles
  /// @param [in] _averageDistance Average distance between particles
  /// @param [in] _particleCount Total number of particles in the system
  /// @param [in] _lightPos Holds the position of the point light
  //////////////////////////////////////////////////////////////////////////////
  virtual void calculate(QVector3D _particleCentre, std::vector<std::unique_ptr<Particle>> &_particleList, QVector3D _averageDistance, unsigned int _particleCount, QVector3D _lightPos, int _cohesionFactor, int _springFactor) {}

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Moves the particles closest to the centre to create a bulge effect.
  /// @param [in] _particleCentre Position of the average centre of all particles
  //////////////////////////////////////////////////////////////////////////////
  virtual void bulge(QVector3D _particleCentre) {}

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns a list containing all particles closest to the light.
  /// @param [in] _particleList List of all particles
  /// @param [in] _lightPos Holds the position of the point light
  /////////////////////////////////////////////////////////////////////////////
  virtual std::vector<unsigned int> getHitParticles(std::vector<std::unique_ptr<Particle>> &_particleList, QVector3D _lightPos) = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the child threshold.
  /// only applicable for Growth particle
  /// @param[in] _amount amount of children allowed per branch.
  //////////////////////////////////////////////////////////////////////////////
  virtual void setChildThreshold(int _amount) {}

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the branch length of a branch.
  /// only applicable for Growth particle
  /// @param[in] _value length of the branch.
  //////////////////////////////////////////////////////////////////////////////
  virtual void setBranchLength(float _value) {}

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Particle dependent function for splitting, needs to be overwritten
  /// on subclasses. Each type of particle will have a different one.
  //////////////////////////////////////////////////////////////////////////////
  virtual void split(QVector3D, std::vector<std::unique_ptr<Particle>>&) {}


  //////////////////////////////////////////////////////////////////////////////
  /// @brief Particle dependent function for splitting, needs to be overwritten
  /// on subclasses. Each type of particle will have a different one.
  //////////////////////////////////////////////////////////////////////////////

  virtual void split(std::vector<std::unique_ptr<Particle>>&,std::mt19937_64) {}

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Checks the current particle and its children recursively to see if
  /// they collide with anything.
  //////////////////////////////////////////////////////////////////////////////

  virtual bool recursiveCollision(
      QVector3D,
      std::vector<std::unique_ptr<Particle>>&) { return false; }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief checks if the particle has reached its food threshold and therefore
  /// needs to be split.
  //////////////////////////////////////////////////////////////////////////////
  void testForSplit();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Particle position getter. Sets the vector passed in.
  /// @param[out] _pos Will hold the particles position
  //////////////////////////////////////////////////////////////////////////////
  void getPos(QVector3D &_pos);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the position of the particle.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D getPosition(){return m_pos;}

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the Particles position
  /// @param[in] _x New x position of the Particle
  /// @param[in] _y New y position of the Particle
  /// @param[in] _z New z position of the Particle
  //////////////////////////////////////////////////////////////////////////////
  void setPos(
      qreal _x,
      qreal _y,
      qreal _z);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Particle radius
  /// @param[out] _radius Will hold the particles radius
  //////////////////////////////////////////////////////////////////////////////
  void getRadius(float &_radius);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the Particles position
  /// @param[in] _radius Size of the particle.
  //////////////////////////////////////////////////////////////////////////////
  void setRadius(float _radius);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Adds a particle ID to the current particle, is used to create a
  /// *connection* between to particles and should be used by another particle
  /// to create the connection.
  /// @param[in] _ID ID of the particle that is to be connected to the current
  /// particle.
  //////////////////////////////////////////////////////////////////////////////
  void connect(unsigned int _ID);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Deletes an ID from the connection list. In other words, it breaks
  /// the connection between two particles.
  /// @param[in] _ID ID to be deleted.
  //////////////////////////////////////////////////////////////////////////////
  void deleteConnection(unsigned int _ID);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the number of connections.
  /// @return The number of connections.
  //////////////////////////////////////////////////////////////////////////////
  int getConnectionCount();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the particles ID.
  /// @return ID of the particle.
  //////////////////////////////////////////////////////////////////////////////
  uint getID();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Writes a list including all ID that are connected to the particle.
  /// @param[out] _returnList will hold the IDs.
  //////////////////////////////////////////////////////////////////////////////
  void getConnectionsID(std::vector<unsigned int> &_returnList);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Writes a list with all positions of the particles connections.
  /// @param[out] _linkPos list where to write the positions.
  /// @param[in] _particleList list from the particle system holding all
  /// existing particles.
  //////////////////////////////////////////////////////////////////////////////
  void getPosFromConnections(
      std::vector<QVector3D> &_linkPos,
      std::vector<std::unique_ptr<Particle>> &_particleList);


  //////////////////////////////////////////////////////////////////////////////
  /// @brief static function that resets the m_ID_counter to 0.
  //////////////////////////////////////////////////////////////////////////////
  static void resetIDCounter();

protected:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Particle position.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_pos;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Velocity that is used to move the particle each frame.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_vel;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Counts the number of objects created from this class.
  //////////////////////////////////////////////////////////////////////////////
  static unsigned int m_ID_counter;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Unique ID of particle used for storing connections.
  /// represents the index in the particle list
  //////////////////////////////////////////////////////////////////////////////
  unsigned int m_ID;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Particle size or radius.
  //////////////////////////////////////////////////////////////////////////////
  float m_size;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Flag that is set when the particle needs to be split.
  //////////////////////////////////////////////////////////////////////////////
  bool m_split;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief holds IDs of all particles connected to this particle.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<unsigned int> m_connectedParticles;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Food level, increses when the particle is hit by light.
  //////////////////////////////////////////////////////////////////////////////
  unsigned int m_foodLevel;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Food threshold, when reached particle is split.
  //////////////////////////////////////////////////////////////////////////////
  unsigned int m_foodThreshold;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Vector holding the IDs of all the particles being hit by light.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<unsigned int> m_hitParticles;

};

#endif // PARTICLE_H
