{
  description = "изучаю SDL shell";
  nixConfig.bash-prompt = "\[nix-develop\]$ ";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.05";

  outputs = { self, nixpkgs }:
    let pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in  {
    packages.x86_64-linux.playsdl = pkgs.stdenv.mkDerivation {
          pname = "playsdl";
          version = "0.1";
          src = self;

          installFlags = [ "PREFIX=$(out)" ];

          buildInputs = with pkgs; [ SDL2 ];
    };

    packages.x86_64-linux.default = self.packages.x86_64-linux.playsdl;

    devShell.x86_64-linux = pkgs.stdenv.mkDerivation {
        name = "ok";

        buildInputs = with pkgs; [ SDL2 plan9port];
    };

  };
}
