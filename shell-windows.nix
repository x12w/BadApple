{ pkgs ? import <nixpkgs> {} }:

let
  cross = pkgs.pkgsCross.mingwW64;
in
pkgs.mkShell {
  packages = [
    cross.buildPackages.cmake
    cross.buildPackages.ninja
    cross.buildPackages.pkg-config
    cross.stdenv.cc
    cross.qt6.qtbase
  ];

  shellHook = ''
    export CC="${cross.stdenv.cc}/bin/${cross.stdenv.cc.targetPrefix}cc"
    export CXX="${cross.stdenv.cc}/bin/${cross.stdenv.cc.targetPrefix}c++"
    export RC="${cross.stdenv.cc.bintools}/bin/${cross.stdenv.cc.targetPrefix}windres"
    export Qt6_DIR="${cross.qt6.qtbase}/lib/cmake/Qt6"
    export CMAKE_PREFIX_PATH="${cross.qt6.qtbase}:''${CMAKE_PREFIX_PATH:-}"

    echo "BadApple Windows cross shell ready"
    echo "  CC=$CC"
    echo "  CXX=$CXX"
    echo "  Qt6_DIR=$Qt6_DIR"
  '';
}
