file(REMOVE_RECURSE
  "CMakeFiles/grpc_gen"
  "generated/adapter.grpc.pb.cc"
  "generated/adapter.grpc.pb.h"
  "generated/config.grpc.pb.cc"
  "generated/config.grpc.pb.h"
  "generated/server.grpc.pb.cc"
  "generated/server.grpc.pb.h"
  "generated/stream.grpc.pb.cc"
  "generated/stream.grpc.pb.h"
  "generated/unary.grpc.pb.cc"
  "generated/unary.grpc.pb.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/grpc_gen.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
