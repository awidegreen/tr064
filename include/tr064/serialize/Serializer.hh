#ifndef SERIALIZER_HH
#define SERIALIZER_HH

#include "Serializable.hh"

// stl
#include <ostream>
#include <istream>

namespace tr064
{
namespace serialize
{

class Serializer
{
public:
  virtual ~Serializer() { }

  virtual void serialize(std::ostream& out, const Serializeable& root) const = 0;
  virtual void deserialize(std::istream& in, Serializeable& root) const = 0;

protected:
  Serializer() { }

};

} // ns
} // ns


#endif /* SERIALIZER_HH */
