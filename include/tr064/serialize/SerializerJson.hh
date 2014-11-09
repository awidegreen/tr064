#ifndef SERIALIZERJSON_HH
#define SERIALIZERJSON_HH

#include "Serializer.hh"

namespace tr064
{
namespace serialize
{

class SerializerJson : public Serializer
{
public:
  void serialize(std::ostream& out, const Serializeable& root) const;
  void deserialize(std::istream& in, Serializeable& root) const;

};

} // ns
} // ns

#endif /* SERIALIZERJSON_HH */
