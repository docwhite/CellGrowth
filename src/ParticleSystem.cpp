////////////////////////////////////////////////////////////////////////////////
/// @file ParticleSystem.cpp
/// @author Carola Gille
/// @author Ramon Blanquer
/// @author Esme Prior
/// @author Lydia Kenton
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////

// Native
#include <math.h>
#include <iostream>
#include <random>

// Custom
#include "include/ParticleSystem.h"

// Default constructor creates a 2500 (50*50) distribution of particles
ParticleSystem::ParticleSystem() :
  m_gen(m_rd())
{
  qDebug("Default constructor called");

  m_currentParticleSize=2.0;
  m_particleCount=0;
  m_particleType= 'L';
  fill(12);
  m_forces = true;
  m_particleDeath = false;
  m_cohesion = 30; //percent
  m_localCohesion = 30;
  m_automataRadius = 4;
  m_automataTime = 200;
  m_nearestParticleState=true;
  m_GP_growtoLight=true;
}

//filling particle system with input particle type
ParticleSystem::ParticleSystem(char _particleType):
  m_gen(m_rd())
{
  qDebug("Custom constructor called");

  m_currentParticleSize=2.0;
  m_particleCount=0;
  m_particleType = _particleType;

  //if it's a linked particle we need 12 particle
  if (m_particleType=='L')
  {
    fill(12);

    m_cohesion = 30; //percent
    m_localCohesion = 30;
    m_forces = true;
    m_particleDeath = false;
  }
  //if it's a Growth particle we need 1 particle to start with
  else if (m_particleType== 'G')
  {
    fill(1);
  }
  else if (m_particleType=='A')
  {
    fill(1);
    m_automataRadius = 4;
    m_automataTime = 200;
  }
}


void ParticleSystem::advance()
{
  //reseting the particle count to the size of the particle list
  m_particleCount=m_particles.size();

  //calculating the forces
  if (m_forces==true)
  {
    for (unsigned int i = 0; i < m_particleCount; ++i)
    {
      switch(m_particleType)
      {
      case 'A':
        m_particles[i]->calculate(m_particles, m_automataRadius, m_automataTime);
        if(m_particles[i]->isAlive() == false)
        {
          m_iterID.push_back(i); //Pushes dead particles into a vector of IDs
          deleteParticle(); //Function call to deleteParticle
        }
        break;
      case 'L':
        m_particles[i]->calculate(m_particles, m_averageDistance, m_cohesion, m_localCohesion, m_particleDeath);
        break;
      default:
        break;
      }
    }

    for (unsigned int i = 0; i < m_particleCount; ++i)
    {
      m_particles[i]->advance();
    }
  }

  m_iterID.resize(0); //Resizes the vector of dead particles
}

void ParticleSystem::bulge()
{
  //Bulges the innermost particles outwards
  m_particleCount=m_particles.size();
  for (unsigned int i = 0; i < m_particleCount; ++i)
  {
    m_particles[i]->bulge(m_particleCentre);
    m_particles[i]->advance();
  }
  calculateParticleCentre();
}

void ParticleSystem::addFood()
{
  for (unsigned int i=0; i<=m_particles.size()/3; i++)
  {
    unsigned int randomIndex = rand() % m_particleCount;
    m_particles[randomIndex]->setFoodLevelTrue();
  }

  for (unsigned int i = 0; i < m_particleCount; ++i)
  {
    m_particles[i]->advance();
  }
}

void ParticleSystem::fill(unsigned int _amount)
{
  std::uniform_real_distribution<float> distribution(-10.0,10.0);
  std::vector<QVector3D> pos;

  const float X = 0.525731112119133606;
  const float Z = 0.850650808352039932;
  const float N= 0.0f;

  //Esme
  pos.push_back(QVector3D(-X,N,Z));
  pos.push_back(QVector3D(X,N,Z));
  pos.push_back(QVector3D(-X,N,-Z));
  pos.push_back(QVector3D(X,N,-Z));
  pos.push_back(QVector3D(N,Z,X));
  pos.push_back(QVector3D(N,Z,-X));
  pos.push_back(QVector3D(N,-Z,X));
  pos.push_back(QVector3D(N,-Z,-X));
  pos.push_back(QVector3D(Z,X,N));
  pos.push_back(QVector3D(-Z,X,N));
  pos.push_back(QVector3D(Z,-X,N));
  pos.push_back(QVector3D(-Z,-X,N));

  for (unsigned int i = 0; i < _amount; i++)
  {
    if(m_particleType=='G') //Growth particle
    {
      m_particles.emplace_back(std::unique_ptr<Particle>(new GrowthParticle(0.1,0.3,0.4,m_currentParticleSize)));

      m_particleCount++;
    }
    else if(m_particleType=='L') //Linked particle
    {
      m_particles.emplace_back(std::unique_ptr<Particle>(new LinkedParticle(pos[i].x(), pos[i].y(),pos[i].z(),m_currentParticleSize)));
    }
    else if(m_particleType=='A') //Automata particle
    {
      m_particles.emplace_back(std::unique_ptr<Particle>(new AutomataParticle(0,0,0)));
      m_particleCount++;
    }
    m_particleCount++;
  }

  if (_amount <= 12)
  {
   if (m_particleType=='L')
   {
    //Linking the icosahedron
    m_particles[0]->doubleConnect(1,m_particles);
    m_particles[0]->doubleConnect(4,m_particles);
    m_particles[0]->doubleConnect(6,m_particles);
    m_particles[0]->doubleConnect(9,m_particles);
    m_particles[0]->doubleConnect(11,m_particles);
    m_particles[1]->doubleConnect(4,m_particles);
    m_particles[1]->doubleConnect(6,m_particles);
    m_particles[1]->doubleConnect(8,m_particles);
    m_particles[1]->doubleConnect(10,m_particles);
    m_particles[2]->doubleConnect(3,m_particles);
    m_particles[2]->doubleConnect(5,m_particles);
    m_particles[2]->doubleConnect(7,m_particles);
    m_particles[2]->doubleConnect(9,m_particles);
    m_particles[2]->doubleConnect(11,m_particles);
    m_particles[3]->doubleConnect(5,m_particles);
    m_particles[3]->doubleConnect(7,m_particles);
    m_particles[3]->doubleConnect(8,m_particles);
    m_particles[3]->doubleConnect(10,m_particles);
    m_particles[4]->doubleConnect(5,m_particles);
    m_particles[4]->doubleConnect(8,m_particles);
    m_particles[4]->doubleConnect(9,m_particles);
    m_particles[5]->doubleConnect(8,m_particles);
    m_particles[5]->doubleConnect(9,m_particles);
    m_particles[6]->doubleConnect(7,m_particles);
    m_particles[6]->doubleConnect(10,m_particles);
    m_particles[6]->doubleConnect(11,m_particles);
    m_particles[7]->doubleConnect(10,m_particles);
    m_particles[7]->doubleConnect(11,m_particles);
    m_particles[8]->doubleConnect(10,m_particles);
    m_particles[9]->doubleConnect(11,m_particles);
   }
  }

  else
  {
    qDebug("To many particles to link");
  }
}

// Returns a NORMAL pointer to the linked particle, not a smart one, otherwise
// the copy constructor triggered by the = (assignment) operator would trigger
// a change of ownership. We do not want that. Read on unique_ptr and shared_ptr.

Particle* ParticleSystem::getParticle(unsigned int _idx)
{
  return m_particles[_idx].get();
}

// Gets the total number of particles
unsigned int ParticleSystem::getSize()
{
  return m_particles.size();
}

void ParticleSystem::getLinksForDraw(std::vector<uint> &_returnList)
{
  _returnList.clear();
  //loops through each particle and finds the links
  for (unsigned int i = 0; i < m_particles.size(); i++)
  {

    std::vector<unsigned int> tempList;
    m_particles[i]->getConnectionsID(tempList);
    for (unsigned int j = 0; j < tempList.size(); j++)
    {
      // Adding the positions to the return list making every second item the
      // position of the current particle so a line can be drawn
      for (unsigned int k = 0; k < m_particles.size(); k++)
      {
        if (m_particles[k]->getID() == tempList[j])
        {
          // Pushes back the ID of linked Particle
          _returnList.push_back(m_particles[k]->getID());

          // Pushes back the ID of current Particle
          _returnList.push_back(m_particles[i]->getID());
          break;
        }
      }
    }
  }
}

void ParticleSystem::setLightPos(QVector3D _lightPos)
{
  m_lightPos = _lightPos;
}

void ParticleSystem::splitRandomParticle()
{
  if(m_particleType=='A') return;

  bool split=false;
  std::vector<uint> toSplit;

  for(uint i=0;i<m_particles.size();i++)
  {
    toSplit.push_back(i);
  }
  

  while (split == false)
  {
    std::uniform_int_distribution<int> distribution(0,toSplit.size()-1);

    uint nearestParticle = getNearestParticle(toSplit);
    uint index;

    if(m_nearestParticleState==true)
        index=nearestParticle;
    else
        index=distribution(m_gen);
    // calling different split function based on the particle type

    if(m_particleType=='G')
    {
      split=m_particles[toSplit[index]]->split(m_lightPos,m_particles,m_gen,m_GP_growtoLight);
    }
    else if(m_particleType=='L')
    {
      split=m_particles[toSplit[index]]->split(m_particles,m_gen);
    }

    m_particleCount=m_particles.size();

    if(split==false)
    {
      toSplit.erase(toSplit.begin()+index);
    }
  }

  for (unsigned int i = 0; i < m_particleCount; ++i)
  {
    switch(m_particleType)
    {
    case 'A':
      m_particles[i]->calculate(m_particles, m_automataRadius, m_automataTime);
      break;
    case 'L':
      m_particles[i]->calculate(m_particles, m_averageDistance, m_cohesion, m_localCohesion, m_particleDeath);
      break;
    default:
      break;
    }
  }
}

unsigned int ParticleSystem::getNearestParticle(std::vector<uint> _toSplit)
{
  //Finds the particle nearest to the point light radius so that they may be split
  std::vector<float> m_lightDistances;

  for (unsigned int i=0; i<_toSplit.size(); i++)
  {
    QVector3D lightDist = (m_particles[_toSplit[i]]->getPosition() - m_lightPos);
    m_lightDistances.push_back(lightDist.lengthSquared());
  }

  std::vector<float>::iterator minElement = std::min_element (std::begin(m_lightDistances), std::end(m_lightDistances));
  unsigned int minElementIndex = std::distance(std::begin(m_lightDistances), minElement);

  return minElementIndex;
}

void ParticleSystem::deleteParticle()
{
  //Deletes the particles - only used for automata particles
  int originalArraySize = m_particles.size();

  if(m_iterID.size()!=0)
  {
    for(uint j=0; j<m_iterID.size(); ++j)
    {
      m_particles.erase(m_particles.begin()+m_iterID[j]-(j));
    }

    m_particles.resize(originalArraySize - m_iterID.size());
    m_particleCount -= m_iterID.size();
  }
}

void ParticleSystem::packageDataForDrawing(std::vector<float> &_packagedData)
{
  _packagedData.clear();
  for_each(m_particles.begin(), m_particles.end(), [&_packagedData](std::unique_ptr<Particle> &p)
  {
    QVector3D pos;
    p->getPos(pos);

    float radius;
    p->getRadius(radius);

    _packagedData.push_back(pos.x());
    _packagedData.push_back(pos.y());
    _packagedData.push_back(pos.z());
    _packagedData.push_back(radius);
  });
}

QVector3D ParticleSystem::calculateParticleCentre()
{
  //Finds the average positon of all the particles
  m_particleCentre.setX(0);
  m_particleCentre.setY(0);
  m_particleCentre.setZ(0);

  for (auto &particle : m_particles)
  {
    QVector3D particlePosition = particle->getPosition();
    m_particleCentre += particlePosition;
  }

  m_particleCentre = m_particleCentre/(m_particles.size());
  return m_particleCentre;
}

QVector3D ParticleSystem::calculateAverageDistanceFromCentre()
{
  QVector3D averageDistance;

  for (auto &particle : m_particles)
  {
    QVector3D particlePosition = particle->getPosition();
    QVector3D particleCentre = calculateParticleCentre();
    QVector3D distance = particleCentre - particlePosition;
    QVector3D fabsDistance;
    fabsDistance.setX(fabs (distance.x()));
    fabsDistance.setY(fabs (distance.y()));
    fabsDistance.setZ(fabs(distance.z()));

    m_averageDistance += fabsDistance;
  }

  averageDistance = averageDistance/(m_particles.size());
  return averageDistance;
}

void ParticleSystem::setParticleSize(double _size)
{
  m_currentParticleSize=_size;
  for(unsigned int i=0;i< m_particles.size();i++)
  {
    m_particles[i]->setRadius(_size);
  }
}

void ParticleSystem::toggleForces(bool _state)
{
  m_forces=_state;
}

void ParticleSystem::toggleParticleDeath(bool _state)
{
  m_particleDeath=_state;
}

void ParticleSystem::setCohesion(int _amount)
{
  m_cohesion = 100 - (_amount);
}

void ParticleSystem::setLocalCohesion(int _amount)
{
  m_localCohesion = 100 - (_amount);
}

void ParticleSystem::setAutomataRadius(int _amount)
{
  //Only for automata particles
  m_automataRadius = _amount;
}

void ParticleSystem::setAutomataTime(int _amount)
{
  //Only for automata particles
  m_automataTime = _amount;
}

void ParticleSystem::setBranchLength(float _amount)
{
  for(unsigned int i=0;i< m_particles.size();i++)
  {
    m_particles[i]->setBranchLength(_amount);
  }
}

void ParticleSystem::setChildThreshold(int _value)
{
  for(unsigned int i=0;i< m_particles.size();i++)
  {
    m_particles[i]->setChildThreshold(_value);
  }
}

void ParticleSystem::reset(char _particleType)
{
  m_particles.clear();
  m_particleCount=0;
  Particle::resetIDCounter();
  m_particleType=_particleType;
  if (m_particleType=='L') //Linked Particles
  {
    fill(12);

    m_forces = true;
    m_particleDeath = false;
    m_cohesion = 30; //percent
    m_localCohesion = 30;
    m_nearestParticleState=true;
  }
  else if (m_particleType== 'G') //Growth particles
  {
    m_nearestParticleState=false;
    fill(1);
  }
  else if (m_particleType == 'A') //Automata particles
  {
    fill(1);
    m_automataRadius = 4;
    m_automataTime = 200;
  }
  m_GP_growtoLight=true;
}

void ParticleSystem::setNearestParticleState(bool _state)
{
  m_nearestParticleState=_state;
}

void ParticleSystem::setGrowToLight(bool _state)
{
  m_GP_growtoLight=_state;
}

