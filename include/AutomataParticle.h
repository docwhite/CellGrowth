////////////////////////////////////////////////////////////////////////////////
/// @file AutomataParticle.h
/// @author Lydia Kenton
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOMATAPARTICLE_H
#define AUTOMATAPARTICLE_H

// Qt
#include <QTime>

// Project
#include "Particle.h"

////////////////////////////////////////////////////////////////////////////////
/// @class AutomataParticle
/// @brief Automata Particle inheriting from Particle class, imitates
/// Conway's Game of Life.
////////////////////////////////////////////////////////////////////////////////
class AutomataParticle : public Particle
{

public :
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Default constructor placing particle at the origin.
  //////////////////////////////////////////////////////////////////////////////
  AutomataParticle();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Custom constructor allowing user input for position.
  /// @param[in] _x X position of the particle.
  /// @param[in] _y Y position of the particle.
  /// @param[in] _z Z position of the particle.
  //////////////////////////////////////////////////////////////////////////////
  AutomataParticle(qreal _x, qreal _y, qreal _z);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Custom constructor allowing user input for position as well as
  /// which particles it is connected to.
  /// @param[in] _x X position of the particle.
  /// @param[in] _y Y position of the particle.
  /// @param[in] _z Z position of the particle.
  /// @param[in] _linkedParticles List of particle IDs to be connected to
  /// the newly generated particle.
  //////////////////////////////////////////////////////////////////////////////
  AutomataParticle(
      qreal _x,
      qreal _y,
      qreal _z,
      std::vector<uint> _automataParticles
  );

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Calculates the new velocity of the particle based on the forces
  /// that act on it.
  /// @param [in] _particleList List of all particles.
  /// @param [in] _automataRadius Controls the radius in which automata are created.
  /// @param [in] _automataTime Controls the speed at which automata are created.
  //////////////////////////////////////////////////////////////////////////////
  void calculate(
      std::vector<std::unique_ptr<Particle>> &_particleList,
      int _automataRadius,
      int _automataTime
  ) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Finds the neighbours of the particles.
  /// @param [in] _particleList List of all particles.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<uint> getNeighbours(
      std::vector<std::unique_ptr<Particle>> &_particleList
  );

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Defines the rules based on Conway's Game of Life.
  /// @param [in] _particleList List of all particles.
  //////////////////////////////////////////////////////////////////////////////
  void particleRules(std::vector<std::unique_ptr<Particle>> &_particleList);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Boolean to define when a particle is alive.
  /// @returns Flag for whether or not particle is alive.
  //////////////////////////////////////////////////////////////////////////////
  virtual bool isAlive();

private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the time using QTime.
  //////////////////////////////////////////////////////////////////////////////
  QTime m_time;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Boolean to flag if the particle is alive.
  //////////////////////////////////////////////////////////////////////////////
  bool m_alive;

};

#endif // AUTOMATAPARTICLE_H
