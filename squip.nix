{ self
, stdenv
, lib
, cmake
, gtest
, squirrel
, tinycmmc
, tinycmmc_lib
}:

stdenv.mkDerivation {
  pname = "squip";
  version = tinycmmc_lib.versionFromFile self;

  src = ./.;

  postPatch = ''
    echo "v${tinycmmc_lib.versionFromFile self}" > VERSION
  '';

  doCheck = true;

  cmakeFlags = [
    "-DWARNINGS=ON"
    "-DWERROR=ON"
    "-DBUILD_TESTS=ON"
  ];

  makeFlags = [
    "VERBOSE=1"
    "ARGS=-V"
  ];

  nativeBuildInputs = [
    cmake
  ];

  buildInputs = [
    tinycmmc
    squirrel
  ];

  checkInputs = [
    gtest
  ];
}
