{ pkgs ? import <nixpkgs> {} }:

let
  qtBase = pkgs.qt6.qtbase;
  qtWayland = pkgs.qt6.qtwayland;
  openCv = pkgs.opencv;
in
pkgs.mkShell {
  packages = with pkgs; [
    cmake
    ninja
    pkg-config
    gcc
    gdb
    openCv
    qtBase
    qtWayland
  ];

  shellHook = ''
    export OpenCV_DIR="${openCv}/lib/cmake/opencv4"
    export Qt6_DIR="${qtBase}/lib/cmake/Qt6"
    export CMAKE_PREFIX_PATH="${qtBase}:${openCv}:''${CMAKE_PREFIX_PATH:-}"

    echo "BadApple dev shell ready"
    echo "  OpenCV_DIR=$OpenCV_DIR"
    echo "  Qt6_DIR=$Qt6_DIR"
  '';
}
