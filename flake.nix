{
  description = "A collection of point, size and rect classes for C++";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";

    squirrel.url = "github:grumnix/squirrel";
    squirrel.inputs.nixpkgs.follows = "nixpkgs";
    squirrel.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, tinycmmc, squirrel }:
    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      {
        packages = rec {
          default = squip;

          squip = pkgs.callPackage ./squip.nix {
            inherit self;
            tinycmmc_lib = tinycmmc.lib;
            tinycmmc = tinycmmc.packages.${pkgs.system}.default;
            squirrel = squirrel.packages.${pkgs.system}.default;
          };
        };
      }
    );
}
