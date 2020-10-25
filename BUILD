# bazel build :all
# (see Target Pattern Syntax in the Command-Line Reference
# bazel query --noimplicit_deps --nohost_deps 'deps(//:HTTPFormServer)' --output graph > graph.in
# dot -Tpng < graph.in > graph.png
# nautilus graph.png

cc_binary(
	name = "random_file_tree",
	srcs = ["src/random_file_tree.cpp"]
			+glob(["src/common/**/*.cpp"] )
			+glob(["src/common/**/*.hpp"])
			,
	includes = [],
	copts = ["-g","-std=c++2a","-Isrc","-Isrc/common","-Isrc/common/Generated"],
	linkopts = [
				"-lgtest",
				"-lzstd",
				"-lz",
				"-lxxhash",
				"-lboost_system",
				"-lxerces-c",
				"-lexpat",
				"-lboost_date_time",
				"-lcryptopp",
				"-lpthread",
				"-L/usr/lib/gcc/x86_64-redhat-linux/8/",
				"-lstdc++fs",
				"-lcrypto",
				"-lsodium",
				"-licui18n",
				"-licuio",
				"-licutu",
				"-licuuc",
				]
)

cc_binary(
	name = "Xmltar.Test",
	srcs = ["src/Xmltar.Test.cpp"]
			+glob(["src/common/**/*.cpp"] )
			+glob(["src/common/**/*.hpp"])
			,
	includes = [],
	copts = ["-g","-std=c++2a","-Isrc","-Isrc/common","-Isrc/common/Generated"],
	linkopts = [
				"-lgtest",
				"-lzstd",
				"-lz",
				"-lxxhash",
				"-lboost_system",
				"-lxerces-c",
				"-lexpat",
				"-lboost_date_time",
				"-lcryptopp",
				"-lpthread",
				"-L/usr/lib/gcc/x86_64-redhat-linux/8/",
				"-lstdc++fs",
				"-lcrypto",
				"-lsodium",
				]
)

cc_binary(
	name = "xmltar",
	srcs = ["src/xmltar.cpp"]
			+glob(["src/common/**/*.cpp"] )
			+glob(["src/common/**/*.hpp"])
			,
	includes = [],
	copts = ["-pg","-std=c++2a","-Isrc","-Isrc/common","-Isrc/common/Generated"],
	linkopts = [
				"-pg",
				"-lgtest",
				"-lzstd",
				"-lz",
				"-lxxhash",
				"-lboost_system",
				"-lxerces-c",
				"-lexpat",
				"-lboost_date_time",
				"-lcryptopp",
				"-lpthread",
				"-L/usr/lib/gcc/x86_64-redhat-linux/8/",
				"-lstdc++fs",
				"-lcrypto",
				"-lsodium",
				]
)


cc_binary(
	name = "bdr_writer",
	srcs = ["src/bdr_writer.cpp"]
			+glob(["src/common/**/*.cpp"] )
			+glob(["src/common/**/*.hpp"])
			,
	includes = [],
	copts = ["-pg","-std=c++2a","-Isrc","-Isrc/common","-Isrc/common/Generated"],
	linkopts = [
				"-pg",
				"-lgtest",
				"-lzstd",
				"-lz",
				"-lxxhash",
				"-lboost_system",
				"-lxerces-c",
				"-lexpat",
				"-lboost_date_time",
				"-lcryptopp",
				"-lpthread",
				"-L/usr/lib/gcc/x86_64-redhat-linux/8/",
				"-lstdc++fs",
				"-lcrypto",
				"-lsodium",
				]
)

cc_binary(
	name = "bdr_reader",
	srcs = ["src/bdr_reader.cpp"]
			+glob(["src/common/**/*.cpp"] )
			+glob(["src/common/**/*.hpp"])
			,
	includes = [],
	copts = ["-pg","-std=c++2a","-Isrc","-Isrc/common","-Isrc/common/Generated"],
	linkopts = [
				"-pg",
				"-lgtest",
				"-lzstd",
				"-lz",
				"-lxxhash",
				"-lboost_system",
				"-lxerces-c",
				"-lexpat",
				"-lboost_date_time",
				"-lcryptopp",
				"-lpthread",
				"-L/usr/lib/gcc/x86_64-redhat-linux/8/",
				"-lstdc++fs",
				"-lcrypto",
				"-lsodium",
				]
)

cc_binary(
	name = "decrypt",
	srcs = ["src/decrypt.cpp"]
			+glob(["src/common/**/*.cpp"] )
			+glob(["src/common/**/*.hpp"])
			,
	includes = [],
	copts = ["-pg","-std=c++2a","-Isrc","-Isrc/common","-Isrc/common/Generated"],
	linkopts = [
				"-pg",
				"-lgtest",
				"-lzstd",
				"-lz",
				"-lxxhash",
				"-lboost_system",
				"-lxerces-c",
				"-lexpat",
				"-lboost_date_time",
				"-lcryptopp",
				"-lpthread",
				"-L/usr/lib/gcc/x86_64-redhat-linux/8/",
				"-lstdc++fs",
				"-lcrypto",
				"-lsodium",
				]
)
