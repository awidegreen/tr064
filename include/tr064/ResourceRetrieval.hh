#ifndef RESOURCERETRIEVAL_HH
#define RESOURCERETRIEVAL_HH

#include <memory>
#include <string>
#include <istream>

namespace tr064
{  

typedef std::unique_ptr<std::istream> ResourceStream;

/**
 * Interface class for retrieving a resouce. A resource in this case is a 
 * xml file.
 * Subclasses shall therefore implement either from the network or filesystem
 */
class ResourceRetreival
{
public:
  // typedefs
  typedef std::shared_ptr<ResourceRetreival> Ptr;

  // class interface
  virtual ~ResourceRetreival() { }

  /**
   * Get the a resource stream (stdlib resource stream for the matter).
   */
  virtual ResourceStream get(const std::string& path) const = 0;

protected:
  ResourceRetreival() { }
   
  // hide Assignment operator and cctor
  ResourceRetreival(const ResourceRetreival& );
  ResourceRetreival& operator=(const ResourceRetreival& );

};

//------------------------------------------------------------------------------

class ResourceRetreivalHttp : public ResourceRetreival
{
public:
  /**
   * ctor of resoruce retrieval
   * @param host is the host, it should be an ip addresss, hostname resolution 
   *             needs to be done.
   * @param port Is the port, this is corresponding to the boost port 
   *             specification, either numarical or e.g. http which implies 80.
   */
  ResourceRetreivalHttp(const std::string& host, 
                        const std::string& port = "http");

  /**
   * Gets the resource from a HTTP
   */
  virtual ResourceStream get(const std::string& path) const;

private:
  std::string _host;
  std::string _port;
};

//------------------------------------------------------------------------------

class ResourceRetreivalFile : public ResourceRetreival
{
public:

  /**
   * Gets the stream from a file
   */
  virtual ResourceStream get(const std::string& path) const;

private:
};

//------------------------------------------------------------------------------

class ResourceRetreivalEmpty : public ResourceRetreival
{
public:

  /**
   * Gets the stream from a file
   */
  virtual ResourceStream get(const std::string& path) const;

private:
};
}

#endif /* RESOURCERETRIEVAL_HH */
