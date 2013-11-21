#include "backend/avro2/validate.h"
#include <boost/filesystem.hpp>
#include <avrocpp/api/DataFile.hh>
#include <avrocpp/api/Compiler.hh>
#include <generated/embed_jsons.h>

int main(int, char * []) {
  RMF::avro2::validate();
  return 0;
}
