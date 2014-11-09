#include <tr064/RootDeviceDiscovery.hh>
#include <tr064/serialize/SerializerJson.hh>

// stdl
#include <iostream>
#include <fstream>
#include <sstream>

// c lib
#include <getopt.h>

#define no_argument 0
#define required_argument 1 
#define optional_argument 2


using namespace tr064;

const char* EXEC = "tr064export";
enum EXPORT_TYPE { JSON, XML };

//------------------------------------------------------------------------------

void help()
{
  std::cout <<
    "Usage: " << EXEC << " [options] export-file" << std::endl <<
    "  -t[ype]        The export type: JSON, XML"<< std::endl
    ;
}

//------------------------------------------------------------------------------

bool export_to_file(EXPORT_TYPE type, const char* file)
{
  if ( type == XML )
  {
    std::cout << "Sorry it can not be export to XML yet!" << std::endl;
    return false;
  }


  RootDeviceDiscovery discovey;
  auto devs = discovey.discover();

  auto amount_of_devs = devs.size();

  if ( amount_of_devs > 1 )
  {
    std::cerr << "I got more then one root device; bailing out!";
    return false;
  }
  else if ( amount_of_devs == 0 )
  {
    std::cerr << "I got more then one root device; bailing out!";
    return false;
  }

  auto& dev = devs.at(0);

  std::ofstream outfile(file, std::ofstream::out);
  if ( !outfile.is_open() )
  {
    std::cerr << "Unable to open outfile: " << file << std::endl;
    return false;
  }
  serialize::SerializerJson json_exporter;
  json_exporter.serialize(outfile, *dev);
  outfile.close();

  return true;
}

//------------------------------------------------------------------------------



int main(int argc, char* argv[])
{
  const struct option longopts[] = 
  {
    {"help", no_argument,         0, 'h'},
    {"type", optional_argument,   0, 't'}
  };

  EXPORT_TYPE type = JSON;

  //turn off getopt error message
  //opterr=1; 
  // https://stackoverflow.com/questions/8793020/using-getopt-long-c-how-do-i-code-up-a-long-short-option-to-both-require-a
  
  int index = 0, iarg=0;
  while (iarg != -1 )
  {
    iarg = getopt_long(argc, argv, "ht:", longopts, &index);

    switch (iarg)
    {
      case 'h':
        help();
        break;
      case 't':
        {
          std::string opt_arg = optarg;
          if ( opt_arg == "JSON" )
            type = JSON;
          if ( opt_arg == "XML" )
            type = XML;
          else 
          {
            std::cerr << "The export type: " << opt_arg 
                      << " is not available!" << std::endl;
            return 1;
          }
        }
        break;
      default:
        break;
    }
  }

  if ( optind+1 != argc )
  {
    help();
    return 1;
  }

  export_to_file(type, argv[optind]);
  
  //std::cout << "optind: " << optind << std::endl;
  //for (index = optind; index < argc; ++index)
  //{
    //std::cout << "index: " << index << " argc: " << argc << std::endl;
    //std::cout << "opt: " <<  << std::endl;
  //}

}
