{pkgs ? import <nixpkgs> {}}: let
  stdenv = pkgs.gccStdenv;
in {
  project = stdenv.mkDerivation {
    name = "c/cpp dev";
    buildInputs = with pkgs; [
      gcc
      lld
      libcxx
      cmake
      gnumake
      gmsh
    ];
    NIX_CFLAGS_COMPILE = "-I/home/plasmaa0/infa/current-lines/src";
    #   shellHook =
    # ''
    #   ${pkgs.fish}/bin/fish
    #   exit
    # '';
  };
}
