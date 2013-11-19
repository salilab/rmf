#include "backend/avro2/validate.h"
#include <boost/filesystem.hpp>
#include <backend/AvroCpp/api/DataFile.hh>
#include <backend/AvroCpp/api/Compiler.hh>
#include <generated/embed_jsons.h>

int main(int, char * []) {
  RMF::avro2::validate();
  return 0;
}
