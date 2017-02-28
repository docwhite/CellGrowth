#ifndef GROWTHPARTICLE_H
#define GROWTHPARTICLE_H

#include"Particle.h"

////////////////////////////////////////////////////////////////////////////////
/// @file GrowthParticle.h
/// @author Carola Gille
/// @version 0.0.1
/// @class GrowthParticle
/// @brief Growth Particle Inheriting from  Particle class, imitates plant like
/// growth.
////////////////////////////////////////////////////////////////////////////////
class GrowthParticle: public Particle
{

public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Default constructor placing particle at the origin.
  //////////////////////////////////////////////////////////////////////////////
  GrowthParticle();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Custom constructor allowing user input for position.
  /// @param[in] _x x Position of the particle.
  /// @param[in] _y y Position of the particle.
  /// @param[in] _z z Position of the particle.
  //////////////////////////////////////////////////////////////////////////////
  GrowthParticle(
      qreal _x,
      qreal _y,
      qreal _z);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Custom constructor allowing user input for position as well as
  /// which particles it is connected to.
  /// @param[in] _x x Position of the particle.
  /// @param[in] _y y Position of the particle.
  /// @param[in] _z z Position of the particle.
  /// @param[in] _connectedParticles List of particle IDs to be connected to
  /// the newly generated particle.
  //////////////////////////////////////////////////////////////////////////////
  GrowthParticle(
      qreal _x,
      qreal _y,
      qreal _z,
      std::vector<unsigned int> _connectedParticles);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Calculates the new velocity of the particle based on the forces
  /// that act on it.
  //////////////////////////////////////////////////////////////////////////////
  void calculate(QVector3D _particleCentre, std::vector<QVector3D> m_listOfPositions) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Called when particle needs to be split.
  /// @param[in] _lightDirection Light direction.
  /// @param[in] _particleList List of all particles.
  //////////////////////////////////////////////////////////////////////////////
  void split(
      QVector3D _lightDirection,
      std::vector<std::unique_ptr<Particle>> &_particleList) override;

private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Needs documentation.
  //////////////////////////////////////////////////////////////////////////////
  unsigned int m_childrenTreshold;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Needs documentation.
  /// @param[in] _levels Needs documentation.
  /// @param[in] _testPosition Needs documentation.
  /// @param[in] _particleList Needs documentation.
  /// @returns Needs documentation.
  //////////////////////////////////////////////////////////////////////////////
  bool collision(
      int _levels,
      QVector3D _testPosition,
      std::vector<std::unique_ptr<Particle>> &_particleList);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Needs documentation.
  /// @param[in] _particle Needs documentation.
  /// @param[in] _size Needs documentation.
  //////////////////////////////////////////////////////////////////////////////
  bool testCollision(QVector3D _particle,float _size);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Needs documentation.
  /// @param[in] _particle Needs documentation.
  /// @param[in] _particleList Needs documentation.
  //////////////////////////////////////////////////////////////////////////////
  bool recursiveCollision(
      QVector3D _particle,
      std::vector<std::unique_ptr<Particle>> &_particleList) override;

};

#endif // GROWTHPARTICLE_H
