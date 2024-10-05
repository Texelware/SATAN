{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system: let
        pkgs = (import nixpkgs { inherit system; });
        pkgs-crosssystem = (import nixpkgs {
          inherit system;
          crossSystem = {
            config = "i686-elf";
          };
        });
      in
        {
          devShell = pkgs.mkShell {
            buildInputs = with pkgs; [
              gnumake
              nasm
              qemu
              clang
              libisoburn mtools
              pkgs-crosssystem.buildPackages.grub2
              pkgs-crosssystem.buildPackages.gcc
            ];
          };
        }
      );
}
