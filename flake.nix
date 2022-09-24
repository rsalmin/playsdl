{
  description = "изучаю SDL shell";
  nixConfig.bash-prompt = "\[nix-develop\]$ ";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.05";

  outputs = { self, nixpkgs }:
    let pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in  {

    devShell.x86_64-linux = pkgs.stdenv.mkDerivation {
        name = "ok";

        buildInputs = with pkgs; [ SDL2 plan9port];
    };

  };
}
