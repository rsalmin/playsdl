{
  description = "изучаю SDL shell";
  nixConfig.bash-prompt = "\[nix-develop\]$ ";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.05";

  outputs = { self, nixpkgs }:
    let pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in  {
    packages.x86_64-linux.sdlmouse = pkgs.stdenv.mkDerivation {
          pname = "sdlmouse";
          version = "0.1";
          src = self;

          installFlags = [ "PREFIX=$(out)" ];

          buildInputs = with pkgs; [ SDL2 SDL2_image SDL2_ttf SDL2_mixer pkg-config plan9port ];
    };

    packages.x86_64-linux.default = self.packages.x86_64-linux.sdlmouse;
  };
}
