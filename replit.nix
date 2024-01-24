{ pkgs }: {
	deps = [
   pkgs.cmakeWithGui
		pkgs.clang_12
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}